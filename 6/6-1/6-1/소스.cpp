#include <Windows.h>
#include "resource.h"

HINSTANCE g_hinst;
HWND g_hWnd;
LPCTSTR lpszClass = "6-1";

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);
void CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);

struct Circle{
	int x, y, r, vctx, vcty;
};

Circle shape, Wshape;

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevinstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hinst = hinstance;

	//윈도우 클래스 구조체 값 설정
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hinstance;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;

	//윈도우 클래스 등록
	RegisterClass(&WndClass);

	//윈도우 생성
	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, NULL, (HMENU)NULL, hinstance, NULL);

	//윈도우 출력
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	//이벤트 루프 처리
	while (GetMessage(&Message, 0, 0, 0)){
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	static Circle ball;
	switch (uMsg){
	case WM_CREATE:
		ball.x = 20, ball.y = 50, ball.r = 20;
		ball.vctx = 1, ball.vcty = 1;
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		EndPaint(hWnd, &ps);
		break;

	case WM_LBUTTONDOWN:
		DialogBox(g_hinst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DialogProc);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));	//나머지는 OS로
}

BOOL CALLBACK DialogProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	static int Radio;
	static bool winS;
	switch (iMsg){

	case WM_INITDIALOG:	// WM_CREATE랑 같은 친구
		shape.x = 20, shape.y = 50, shape.r = 20;
		shape.vctx = 1, shape.vcty = 1;
		Radio = 0;
		return true;
	
	case WM_COMMAND:
		switch (LOWORD(wParam)){
		case IDC_STARTB:
			SetTimer(hDlg, 1, 100, TimerProc);
			break;

		case IDC_ENDB:
			KillTimer(hDlg, 1);
			break;

		case IDC_STARTW:
			winS = TRUE;
			break;

		case IDC_ENDW:
			winS = FALSE;
			break;

		case IDC_CIRCLE:
			Radio = 0;
			break;

		case IDC_RECT:
			Radio = 1;
			break;

		case ID_END: // 버튼
			PostQuitMessage(0);
			break;

		case IDC_END: // 버튼
			EndDialog(hDlg, 0);
			break;
		}
		break;
	case WM_PAINT:
		if (shape.x - shape.r < 0){
			shape.vctx *= -1;
		}
		else if (shape.x + shape.r > 400){
			shape.vctx *= -1;
		}
		else if (shape.y - shape.r < 30){
			shape.vcty *= -1;
		}
		else if (shape.y + shape.r > 200){
			shape.vcty *= -1;
		}
		shape.x += (shape.vctx * 2);
		shape.y += (shape.vcty * 2);

		hdc = GetDC(hDlg);

		if (Radio == 0)
			Ellipse(hdc, shape.x, shape.y, shape.x + shape.r, shape.y + shape.r);
		else
			Rectangle(hdc, shape.x, shape.y, shape.x + shape.r, shape.y + shape.r);
		
		ReleaseDC(hDlg, hdc);
		if (winS){
			if (Wshape.x - Wshape.r < 0){
				Wshape.vctx *= -1;
			}
			else if (Wshape.x + Wshape.r > 400){
				Wshape.vctx *= -1;
			}
			else if (Wshape.y - Wshape.r < 30){
				Wshape.vcty *= -1;
			}
			else if (Wshape.y + Wshape.r > 200){
				Wshape.vcty *= -1;
			}
			Wshape.x += (Wshape.vctx * 2);
			Wshape.y += (Wshape.vcty * 2);
			hdc = GetDC(g_hWnd);
			Ellipse(hdc, Wshape.x, Wshape.y, Wshape.x + Wshape.r, Wshape.y + Wshape.r);
			ReleaseDC(g_hWnd, hdc);
		}
		break;
	}
	return 0;
}
void CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime){
	
	InvalidateRect(hWnd, NULL, FALSE);
}