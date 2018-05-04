#include <Windows.h>

HINSTANCE g_hInst;
LPCTSTR lpszClass = "window programe 2-4";	// 제목

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

	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	PAINTSTRUCT ps;
	static int count, yPos, line, save;
	static char str[10][99];
	static SIZE size;
	int i;
	switch (uMsg)
	{
	case WM_CREATE:
		CreateCaret(hWnd, NULL, 5, 15);
		ShowCaret(hWnd);
		count = 0;
		line = 0;
		yPos = 0;
		save = 0;
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		for (i = 0; i <= line; i++) {
			GetTextExtentPoint(hdc, str[i], strlen(str[i]), &size);
			TextOut(hdc, 0, i * 20, str[i], strlen(str[i]));
			SetCaretPos(size.cx, i * 20);
		}
		EndPaint(hWnd, &ps);
		break;

	case WM_CHAR:
		hdc = GetDC(hWnd);
		if (wParam == VK_BACK) {
			--count;
		}
		else if (count <= 0) {
			yPos -= 20;
			--line;
			count = strlen(str[line]);
		}
		else if (wParam == VK_RETURN)
		{
			if (line < 9) {
				count = 0;
				yPos += 20;
				line++;
			}
		}
		else if (wParam == VK_ESCAPE)
		{
			for (; count > 0; --count)
				str[line][count] = '\0';
			yPos = 0;
			line = 0;
		}
		else if (wParam == VK_TAB)
		{
			for (i = 0; i < 4; ++i) {
				str[line][count++] = ' ';
			}
		}
		else {
			if (count < 98) {
				str[line][count++] = wParam;
			}
		}
		str[line][count] = '\0';
		InvalidateRect(hWnd, NULL, TRUE);
		ReleaseDC(hWnd, hdc);
		break;

	case WM_DESTROY:
		HideCaret(hWnd);
		DestroyCaret();
		PostQuitMessage(0);
		return 0;
	default:
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}