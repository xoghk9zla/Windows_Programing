#include <Windows.h>
#include "resource.h"

HINSTANCE g_hInst;
LPCTSTR lpszClass = "5-4";	// 제목

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW, 0, 0, 1200, 700, NULL, (HMENU)NULL, hInstance, NULL); //윈도우 클래스 이름, 타이틀 이름, 스타일, x좌표, y좌표, 폭, 높이, 부모윈도우 핸들, 메뉴 핸들, 응용 프로그램id, 생성된 윈도우 정보

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&Message, 0, 0, 0)){
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	HDC hdc, memdc;
	PAINTSTRUCT ps;
	static HBITMAP hBitmap1, hBitmap2, oldBitmap1, oldBitmap2;
	static int x, y, cnt;
	switch (uMsg)
	{
	case WM_CREATE:
		x = 0, y = 0, cnt = 0;
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		memdc = CreateCompatibleDC(hdc);// 배경
		hBitmap1 = (HBITMAP)LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));
		oldBitmap1 = (HBITMAP)SelectObject(memdc, hBitmap1);
		SelectObject(memdc, hBitmap1);
		StretchBlt(hdc, 0, 0, 1200, 700, memdc, 0, 0, 96, 99, SRCCOPY);
		SelectObject(memdc, oldBitmap1);
		DeleteObject(hBitmap1);
		DeleteDC(memdc);

		memdc = CreateCompatibleDC(hdc);// 팩맨
		hBitmap2 = (HBITMAP)LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP2));
		oldBitmap2 = (HBITMAP)SelectObject(memdc, hBitmap2);
		SelectObject(memdc, hBitmap2);		
		TransparentBlt(hdc, 0 + x, 0 + y, 232, 219, memdc, 0, 0, 232, 219, RGB(255, 255, 255));
		SelectObject(memdc, oldBitmap2);
		DeleteObject(hBitmap2);
		DeleteDC(memdc);

		EndPaint(hWnd, &ps);
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_LEFT:
			--x;
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case VK_RIGHT:
			++x;
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case VK_UP:
			--y;
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case VK_DOWN:
			++y;
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		default:
			break;
		}
		break;
	case WM_CHAR:
		if (wParam == 'j')
			SetTimer(hWnd, 1, 100, NULL);
		break;
	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			y-=3, cnt++;
			if (cnt % 5 == 0){
				KillTimer(hWnd, 1);
				SetTimer(hWnd, 2, 100, NULL);
				cnt = 0;
			}
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case 2:
			y+=3, cnt++;
			if (cnt % 5 == 0){
				KillTimer(hWnd, 2);
				cnt = 0;
			}
			InvalidateRect(hWnd, NULL, FALSE);
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