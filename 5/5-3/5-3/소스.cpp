#include <Windows.h>
#include "resource1.h"

HINSTANCE g_hInst;
LPCTSTR lpszClass = "window programe 5-3";	// 제목

#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;

	g_hInst = hInstance;	//글로벌로 사용

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW, 0, 0, 1100, 700, NULL, (HMENU)NULL, hInstance, NULL); //윈도우 클래스 이름, 타이틀 이름, 스타일, x좌표, y좌표, 폭, 높이, 부모윈도우 핸들, 메뉴 핸들, 응용 프로그램id, 생성된 윈도우 정보 

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);


	while (GetMessage(&Message, NULL, 0, 0)){
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	HDC hdc, memdc;
	PAINTSTRUCT ps;
	static int wW, wH, mW, mH;	// 윈도우 너비, 높이, 소스 너비, 높이
	static int x, y, oldx, oldy, endx, endy;
	static int x1, y1, x2, y2;
	static int mx, my, cx, cy;
	static double per;
	static RECT rect;
	static HBITMAP hBitmap;
	static DWORD dwRop;	//래스터연산
	static bool Drag, sel, copy, paste, move, adjust;
	switch (uMsg)
	{
	case WM_CREATE:
		per = 1;
		hBitmap = (HBITMAP)LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));
		GetClientRect(hWnd, &rect);
		wW = rect.right;
		wH = rect.bottom;
		dwRop = SRCCOPY, Drag = FALSE, sel = FALSE, copy = FALSE, move = FALSE, adjust=FALSE;
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		memdc = CreateCompatibleDC(hdc);
		SelectObject(memdc, hBitmap);
		StretchBlt(hdc, 0, 0, mW, mH, memdc, 0, 0, wW, wH, dwRop);
		
		Rectangle(hdc, x, y, oldx, oldy);
		StretchBlt(hdc, x, y, oldx - x, oldy - y, memdc, x, y, (oldx - x)*per, (oldy - y)*per, dwRop);
		if (paste)
			StretchBlt(hdc, 0, 0, x2-x1, y2 - y1, memdc, x1, y1, (x2-x1)*per, (y2 - y1)*per, dwRop);
		DeleteDC(memdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_SIZE:
		GetClientRect(hWnd, &rect);
		mW = rect.right;
		mH = rect.bottom;
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_LBUTTONDOWN:
		if (sel == TRUE){
			Drag = TRUE;
			x = LOWORD(lParam);
			y = HIWORD(lParam);
		}
		else if (adjust)
			Drag = TRUE;
		else
			move = TRUE;
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_MOUSEMOVE:
		if (Drag == TRUE){
			oldx = LOWORD(lParam);
			oldy = HIWORD(lParam);
		}
		else if(move){
			mx = LOWORD(lParam);
			my = HIWORD(lParam);
			cx = (oldx - x) / 2;
			cy = (oldy - y) / 2;
			x = mx - cx;
			y = my - cy;
			oldx = mx + cx;
			oldy = my + cy;
		}
		
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_LBUTTONUP:
		Drag = FALSE;
		sel = FALSE;
		move = FALSE;
		adjust = FALSE;
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_40001:
			per = 1;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case ID_40002:
			per = 0.8;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case ID_40003:
			per = 0.6;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case ID_40004:
			sel = TRUE;
			break;
		case ID_40005:
			copy = TRUE;
			break;
		case ID_40006:
			if (copy){
				paste = TRUE;
				x1 = x, y1 = y, x2 = oldx, y2 = oldy;
			}
			break;
		case ID_40007:
			adjust = TRUE;
			break;
		default:
			break;
		}
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_RIGHT:
			++x, ++oldx;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case VK_LEFT:
			--x, --oldx;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case VK_DOWN:
			++y, ++oldy;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case VK_UP:
			--y, --oldy;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		default:
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}