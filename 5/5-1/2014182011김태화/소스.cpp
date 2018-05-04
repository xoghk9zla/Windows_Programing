#include <Windows.h>
#include "resource.h"

HINSTANCE g_hInst;
LPCTSTR lpszClass = "window programe 5-2";	// 제목

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	HACCEL hAcc;
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
	hAcc = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACC1));

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	
	while (GetMessage(&Message, NULL, 0, 0)){
		if (!TranslateAccelerator(hWnd, hAcc, &Message)){
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
	}
	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	HDC hdc, memdc;
	PAINTSTRUCT ps;
	static int wW, wH, mW, mH;	// 윈도우 너비, 높이, 소스 너비, 높이
	static int a, b, c;
	static RECT rect;
	static HBITMAP hBitmap1, hBitmap2, hBitmap3;
	static DWORD dwRop;	//래스터연산
	switch (uMsg)
	{
	case WM_CREATE:
		GetClientRect(hWnd, &rect);
		wW = rect.right;
		wH = rect.bottom;
		dwRop = SRCCOPY;
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		hBitmap1 = (HBITMAP)LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));
		memdc = CreateCompatibleDC(hdc);
		SelectObject(memdc, hBitmap1);
		StretchBlt(hdc, a, 0, mW/3, mH, memdc, 0, 0, wW, wH, dwRop);
		DeleteDC(memdc);
		
		hBitmap2 = (HBITMAP)LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP2));
		memdc = CreateCompatibleDC(hdc);
		SelectObject(memdc, hBitmap2);
		StretchBlt(hdc, b, 0, mW/3, mH, memdc, 0, 0, wW, wH, dwRop);
		DeleteDC(memdc);

		hBitmap3 = (HBITMAP)LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP3));
		memdc = CreateCompatibleDC(hdc);
		SelectObject(memdc, hBitmap3);
		StretchBlt(hdc, c, 0, mW/3, mH, memdc, 0, 0, wW, wH, dwRop);
		DeleteDC(memdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_SIZE:
		GetClientRect(hWnd, &rect);
		mW = rect.right;
		mH = rect.bottom;
		a = 0, b = mW / 3, c = mW / 3 * 2;
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_MOVE:
			SetTimer(hWnd, 1, 1000, NULL);
			break;
		case ID_NOTSRCCOPY:
			dwRop = NOTSRCCOPY;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case ID_SRCPAINT:
			dwRop = SRCPAINT;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case ID_SRCCOPY:
			dwRop = SRCCOPY;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case ID_SRCAND:
			dwRop = SRCAND;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case ID_END:
			PostQuitMessage(0);
			break;
		default:
			break;
		}
		break;
	case WM_TIMER:
		int temp;
		switch (wParam)
		{
		case 1:
			temp = a;
			a = c;
			c = b;
			b = temp;
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