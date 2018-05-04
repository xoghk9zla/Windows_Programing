#include <Windows.h>
#include <math.h>
#include "resource.h"
HINSTANCE g_hinst;
HWND g_hWmd;
LPCTSTR lpszClass = L"6-3";

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK Dig_Proc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
void CALLBACK UP(HWND hDlg, UINT uMsg, UINT idEvent, DWORD dwTime);
void CALLBACK DOWN(HWND hDlg, UINT uMsg, UINT idEvent, DWORD dwTime);
void CALLBACK MOVE(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);
double n, x;
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
	WndClass.lpszMenuName = MAKEINTRESOURCE(NULL);
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
int Radio, color[3];
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{	
	HDC hdc, memdc;
	PAINTSTRUCT ps;
	HBITMAP hBitMap;
	static RECT rect;
	//메시지 처리하기
	switch (uMsg){
	case WM_CREATE:
		Radio = 0, n = 1;
		color[0] = 0, color[1] = 0, color[2] = 0;
		g_hWmd = hWnd;
		GetClientRect(hWnd, &rect);
		break;
	case WM_PAINT:
		hdc = BeginPaint(g_hWmd, &ps);
		memdc = CreateCompatibleDC(hdc);
		hBitMap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
		SelectObject(memdc, hBitMap);
		if (Radio == 0){
			for (double i = 0; i < 720; i += 0.1)
				SetPixel(memdc, (i*3.14 / 180) * 35 + 10 + x, sin(i*3.14 / 180 + x) * n * 35 + 150, RGB(color[0], color[1], color[2]));
		}
		else if (Radio == 1){
			for (double i = 0; i < 720; i += 0.01)
				SetPixel(memdc, (i*3.14 / 180) * 35 + 10 + x, cos(i*3.14 / 180 + x) * n * 35 + 150, RGB(color[0], color[1], color[2]));
		}
		else if (Radio == 2){
			for (double i = 0; i < 720; i += 0.01)
				SetPixel(memdc, (i*3.14 / 180) * 35 + 10 + x, tan(i*3.14 / 180 + x) * n * 35 + 150, RGB(color[0], color[1], color[2]));
		}
		BitBlt(hdc, 0, 0, 1200, 700, memdc, 0, 0, SRCCOPY);
		DeleteObject(hBitMap);
		DeleteDC(memdc);
		EndPaint(g_hWmd, &ps);
		break;
	case WM_LBUTTONDOWN:
		DialogBox(g_hinst, MAKEINTRESOURCE(IDD_DIALOG1), g_hWmd, Dig_Proc);
		break;
	
	case WM_DESTROY:
		EndDialog(hWnd, 0);
		PostQuitMessage(0);
		break;
	}
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));	//나머지는 OS로
}

BOOL CALLBACK Dig_Proc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam){//대화상자 메시지 처리함수

	switch (iMsg){
	case WM_INITDIALOG:	//대화상자초기화
		CheckRadioButton(hDlg, IDC_SIN, IDC_TAN, IDC_SIN);
		return true;
	case WM_COMMAND:
		switch (LOWORD(wParam)){
		case IDC_SIN:
			Radio = 0;
			InvalidateRect(g_hWmd, NULL, FALSE);
			break;
		case IDC_COS:
			Radio = 1;
			InvalidateRect(g_hWmd, NULL, FALSE);
			break;
		case IDC_TAN:
			Radio = 2;
			InvalidateRect(g_hWmd, NULL, FALSE);
			break;
		case IDC_RED:
			color[0] = 255-color[0];
			InvalidateRect(g_hWmd, NULL, FALSE);
			break;
		case IDC_GREEN:
			color[1] = 255 - color[1];
			InvalidateRect(g_hWmd, NULL, FALSE);
			break;
		case IDC_BLUE:
			color[2] = 255 - color[2];
			InvalidateRect(g_hWmd, NULL, FALSE);
			break;
		case IDC_MOVE:
			SetTimer(g_hWmd, 1, 200, MOVE);
			break;
		case IDC_UPDOWN:
			SetTimer(g_hWmd, 2, 100, UP);
			break;
		case IDC_STOP:
			KillTimer(g_hWmd, 1);
			KillTimer(g_hWmd, 2);
			KillTimer(g_hWmd, 3);
			break;
		case IDC_RESET:
			n = 1, x = 0;
			InvalidateRect(g_hWmd, NULL, FALSE);
			break;
		case IDCANCEL:
			EndDialog(g_hWmd, 0);
			PostQuitMessage(0);
			break;
		}
		break;
	}
	return 0;
}
void CALLBACK MOVE(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime){
	x += 0.1;
	InvalidateRect(hWnd, NULL, FALSE);
}
void CALLBACK UP(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime){
	n -= 0.2;
	if (n < -2){
		KillTimer(hWnd, 2);
		SetTimer(hWnd, 3, 100, DOWN);
	}
	InvalidateRect(hWnd, NULL, FALSE);
}
void CALLBACK DOWN(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime){
	n += 0.2;
	if (n > 2){
		KillTimer(hWnd, 3);
		SetTimer(hWnd, 2, 100, UP);
	}
	InvalidateRect(hWnd, NULL, FALSE);
}