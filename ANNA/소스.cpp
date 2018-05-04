#include <Windows.h>
#include <time.h>	// �����߻�
#include <math.h>	// sin, cos
#include <mmsystem.h>	// ����
#include "resource.h"

#define LEN 100		// �ȳ�,���Ӹ� �Ÿ�����

HINSTANCE g_hInst;
LPCTSTR lpszClass = "ANNA (������)";	// ����

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
struct Object{
	int Left, Top, Right, Bottom;
	BOOL Sel;
	HBITMAP hBitMap, oldBitMap, hBitMap1, oldBitMap1;	// ������Ʈ Ŭ���� ��Ÿ���� �޽���
};
struct Inventory{	//������ �����
	BOOL Get, Equip, USE;
	HBITMAP hBitMap, oldBitMap, hBitMap1, oldBitMap1;	// ������Ʈ Ŭ���� ��Ÿ���� �޽���
};
struct Character{	// �ȳ�, �����
	double x, y;	// ��ǥ
	double angle, scale_x, scale_y;	// ����, cos(angle), sin(angle)
	double vel_x, vel_y, speed, acc;	// vel_x=speed*scale_x, vel_y=speed*scale_y, �ӵ�, ���ӵ�
	double gravity;	// �߷�
	BOOL Jump;
};
// ���콺 �����Ͱ� ������Ʈ �ȿ� �ִ��� �Ǻ�
BOOL InBox(int x, int y, Object obj){	// ������Ʈ Ŭ��
	if ((obj.Left < x && x < obj.Right) && (obj.Top < y && y < obj.Bottom))
		return TRUE;
	else
		return FALSE;
};
BOOL InBoxFULL(int x, int y, int cnt, Object* obj){	// ��ü ������Ʈ �� Ŭ��
	for (int i = 0; i < cnt; ++i){
		if ((obj[i].Left < x && x < obj[i].Right) && (obj[i].Top < y && y < obj[i].Bottom))
			return TRUE;
	}
	return FALSE;
};
BOOL InOk(int x, int y){	// "��" ��ư�� ��������
	if ((510 < x && x < 610) && (585 < y && y < 610))
		return TRUE;
	else
		return FALSE;
};
BOOL InNo(int x, int y){	// "�ƴϿ�" ��ư�� ��������
	if ((660 < x && x < 770) && (585 < y && y < 610))
		return TRUE;
	else
		return FALSE;
};
BOOL InStart(int x, int y){	// "���ӽ���" ��ư�� ��������
	if ((445 < x && x < 670) && (280 < y && y < 340))
		return TRUE;
	else
		return FALSE;
};
BOOL InEnd(int x, int y){	// ������ ��ư�� ��������
	if ((445 < x && x < 670) && (400 < y && y < 460))
		return TRUE;
	else
		return FALSE;
};
BOOL SelCheck(Object* o, int cnt){	// Sel�� �ִ��� Ȯ��
	for (int i = 0; i < cnt; ++i){
		if (o[i].Sel == TRUE){
			return TRUE;
			break;
		}
	}
	return FALSE;
}
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
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszClass, WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU, 0, 0, 1200, 700, NULL, (HMENU)NULL, hInstance, NULL); //������ Ŭ���� �̸�, Ÿ��Ʋ �̸�, ��Ÿ��, x��ǥ, y��ǥ, ��, ����, �θ������� �ڵ�, �޴� �ڵ�, ���� ���α׷�id, ������ ������ ����

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&Message, 0, 0, 0)){
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	HDC hdc, backdc, memdc1, memdc2, memdc3;
	PAINTSTRUCT ps;
	char buf[30];	//������
	static int Lock;	//stage1 ��Ź �����������
	static Character Anna, Witch;	//ĳ���� �ȳ�
	static RECT rect;	// ������ ������ ����
	static int w, w1, mx, my, wcnt, wcnt1, n, Ecnt, speed;	// �ִϸ��̼�, ���콺 ��ǥ, �ȱ� ������, �κ��丮 ��ġ
	static Inventory Inven[10];	// �κ��丮
	static Object Left, Right, Arrow, Blood;	// �¿� ȭ��ǥ, ����
	static Object stage1[9], stage2[3], stage3[7], stage4[10], stage5[7];	// ������Ʈ ��ġ����
	static BOOL ROAP, LAMP, DIARY, Message, WITCH[2], ENDDING[3];	// ���ĵ� ����, �޼����� ���ִ���
	static BOOL Main, Stage1, Stage2, Stage3, Stage4, Stage5, Stage6, Gameover, TrueEnd, HappyEnd;	// �� ����
	static HBITMAP bgBitMap, bgoldBitMap;	// ��� ��ȭ��
	static HBITMAP hBitMap1_1, oldBitMap1_1, hBitMap1_2, oldBitMap1_2, hBitMap2, oldBitMap2, hBitMap3, oldBitMap3;	// ���, ĳ����
	static HBITMAP hBitMap4_1, oldBitMap4_1, hBitMap4_2, oldBitMap4_2, hBitMap5, oldBitMap5, hBitMap6_1, oldBitMap6_1, hBitMap6_2, oldBitMap6_2;	// ���, ĳ����
	static HBITMAP hBitMap7_1, oldBitMap7_1, hBitMap7_2, oldBitMap7_2;	// ����
	switch (uMsg)
	{
	case WM_CREATE:
		w = 3, speed = 10;
		GetClientRect(hWnd, &rect);
		PlaySound("���� ���", NULL, SND_ASYNC | SND_LOOP);
		Main = TRUE, LAMP = TRUE, Message = FALSE, ENDDING[2] = TRUE;
		// �ȳ�
		Anna.x = 50, Anna.y = 435, Anna.acc = 0.01, wcnt = 0, Anna.gravity = 3;
		Anna.angle = 0, Anna.scale_x = 0, Anna.scale_y = 0, Anna.Jump = FALSE;
		Anna.speed = 0, Anna.vel_x = Anna.speed*Anna.scale_x, Anna.vel_y = Anna.speed * Anna.scale_y;
		// ����
		Witch.x = 50, Witch.y = 435, Witch.acc = 0.02, wcnt = 0, Witch.gravity = 3;
		Witch.angle = 0, Witch.scale_x = 0, Witch.scale_y = 0, Witch.Jump = FALSE;
		Witch.speed = 0, Witch.vel_x = Witch.speed*Witch.scale_x, Witch.vel_y = Witch.speed * Witch.scale_y;
		// ���, �ȳ�
		hBitMap1_1 = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_STAGE1_1));	// �� ��(����ON)	
		hBitMap1_2 = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_STAGE1_2));	// �� ��(����OFF)	
		hBitMap2 = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_ANNA));		// �ȳ�
		hBitMap3 = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_STAGE2));	// �� ��
		hBitMap4_1 = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_MOON));		// ��
		hBitMap4_2 = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_MOON_S));		// ��
		hBitMap5 = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_WITCH));		// ����
		hBitMap6_1 = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_MAIN));		// ����ȭ��
		hBitMap6_2 = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_GAMEOVER));		// ���ӿ���
		hBitMap7_1 = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_ENDDING));
		hBitMap7_2 = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_ENDDINGTRUE));
		// ȭ��ǥ
		Left.Left = 0, Left.Top = 125, Left.Right = 100, Left.Bottom = 200;
		Left.hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_LEFT));
		Right.Left = rect.right - 100, Right.Top = 125, Right.Right = rect.right, Right.Bottom = 200;
		Right.hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_RIGHT));
		// �κ��丮
		Inven[0].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_INVEMPTY));
		Inven[1].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_INVROAP));
		Inven[2].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_INVDISH));
		Inven[3].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_INVLANTERN));
		Inven[4].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_INVKNIFE));
		Inven[5].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_INVBOTTLE));
		Inven[6].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_INVBLACK));
		Inven[7].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_INVLADDER));
		Inven[8].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_INVKEY1));
		Inven[9].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_INVKEY2));
		//��Ź ��� ��ġ����
		srand((unsigned int)time(NULL));
		Lock = rand() % 2;
		if (Lock == 0){
			stage1[3].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_LOCK));
			stage1[4].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_ROAP));
		}
		else if (Lock == 1){
			stage1[3].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_ROAP));
			stage1[4].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_LOCK));
		}

		//Stage1 - �ٶ���
		stage1[0].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_DOOR1));
		stage1[0].Left = 0, stage1[0].Top = 375, stage1[0].Right = 200, stage1[0].Bottom = 515;	//��
		stage1[1].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_LADDER));
		stage1[1].Left = 0, stage1[1].Top = 50, stage1[1].Right = 180, stage1[1].Bottom = 300;	//��ٸ�
		stage1[2].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BOOK));
		stage1[2].Left = 200, stage1[2].Top = 180, stage1[2].Right = 390, stage1[2].Bottom = 210;	//å��
		stage1[3].Left = 660, stage1[3].Top = 210, stage1[3].Right = 775, stage1[3].Bottom = 220;	//����1
		stage1[4].Left = 660, stage1[4].Top = 230, stage1[4].Right = 775, stage1[4].Bottom = 240;	//����2
		stage1[5].Left = 730, stage1[5].Top = 85, stage1[5].Right = 805, stage1[5].Bottom = 130;	//���ĵ�
		stage1[6].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_CUSHION));
		stage1[6].Left = 930, stage1[6].Top = 145, stage1[6].Right = 1120, stage1[6].Bottom = 235;	//����
		stage1[7].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BED));
		stage1[7].hBitMap1 = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_DIARY));
		stage1[7].Left = 800, stage1[7].Top = 480, stage1[7].Right = 1050, stage1[7].Bottom = 540;	//ħ���
		stage1[8].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_CHAIN));
		stage1[8].Left = 310, stage1[8].Top = 490, stage1[8].Right = 450, stage1[8].Bottom = 565;	//��罽

		//stage2 - ����
		stage2[0].Left = 100, stage2[0].Top = 100, stage2[0].Right = 225, stage2[0].Bottom = 440;	// ��ٸ�
		stage2[1].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_DOOR2));
		stage2[1].Left = 500, stage2[1].Top = 270, stage2[1].Right = 525, stage2[1].Bottom = 280;	// ��
		stage2[2].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_UMBRELLA));
		stage2[2].Left = 730, stage2[2].Top = 455, stage2[2].Right = 840, stage2[2].Bottom = 545;	// ���

		//Stage3 - ������
		stage3[0].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_UMBRELLA));
		stage3[0].Left = 230, stage3[0].Top = 515, stage3[0].Right = 330, stage3[0].Bottom = 580;	// ���
		stage3[1].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_DISH));
		stage3[1].Left = 385, stage3[1].Top = 530, stage3[1].Right = 515, stage3[1].Bottom = 595;	// ����1
		stage3[2].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_PICTURE));
		stage3[2].Left = 565, stage3[2].Top = 535, stage3[2].Right = 670, stage3[2].Bottom = 600;	// ����
		stage3[3].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_FIREWALL));
		stage3[3].Left = 480, stage3[3].Top = 350, stage3[3].Right = 750, stage3[3].Bottom = 445;	// ����
		stage3[4].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_DEAR));
		stage3[4].Left = 470, stage3[4].Top = 85, stage3[4].Right = 720, stage3[4].Bottom = 205;	// �罿
		stage3[5].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_WOOD));
		stage3[5].Left = 860, stage3[5].Top = 375, stage3[5].Right = 985, stage3[5].Bottom = 465;	// ����
		stage3[6].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BRUSH));
		stage3[6].Left = 870, stage3[6].Top = 510, stage3[6].Right = 940, stage3[6].Bottom = 630;	// ���ڷ�
		

		//stage4 - �ξ�
		stage4[0].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BUCKET));
		stage4[0].Left = 170, stage4[0].Top = 450, stage4[0].Right = 305, stage4[0].Bottom = 535;	// ����
		stage4[1].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_LANTERN));
		stage4[1].Left = 340, stage4[1].Top = 425, stage4[1].Right = 510, stage4[1].Bottom = 560;	// ���� ������
		stage4[2].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_DRAWER_RIGHT));
		stage4[2].Left = 510, stage4[2].Top = 425, stage4[2].Right = 675, stage4[2].Bottom = 540;	// ������ ������
		stage4[3].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_DRAWER1));
		stage4[3].Left = 675, stage4[3].Top = 425, stage4[3].Right = 805, stage4[3].Bottom = 450;	//�߰� ��������ĭ
		stage4[4].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_DRAWER2));
		stage4[4].Left = 675, stage4[4].Top = 450, stage4[4].Right = 805, stage4[4].Bottom = 495;	//�߰� ������ �߰�ĭ
		stage4[5].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_DRAWER3));
		stage4[5].Left = 675, stage4[5].Top = 495, stage4[5].Right = 805, stage4[5].Bottom = 540;	//�߰� ������ �Ʒ�ĭ
		stage4[6].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_DRAWER4));
		stage4[6].Left = 805, stage4[6].Top = 425, stage4[6].Right = 1030, stage4[6].Bottom = 485;	//�ǿ����� ������ �߰�
		stage4[7].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_DRAWER1));
		stage4[7].Left = 810, stage4[7].Top = 485, stage4[7].Right = 1035, stage4[7].Bottom = 540;	//�ǿ����� ������ �Ʒ�
		stage4[8].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_KNIFE));
		stage4[8].Left = 745, stage4[8].Top = 365, stage4[8].Right = 760, stage4[8].Bottom = 400;	//Į
		stage4[9].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_DISHES));
		stage4[9].Left = 805, stage4[9].Top = 270, stage4[9].Right = 1030, stage4[9].Bottom = 425;	//�ǿ����� ������ ��

		//Stage5 - ��Ź
		stage5[0].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_FRAME));
		stage5[0].Left = 180, stage5[0].Top = 130, stage5[0].Right = 550, stage5[0].Bottom = 270;	// ����
		stage5[1].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_WATERBOTTLE));
		stage5[1].Left = 235, stage5[1].Top = 370, stage5[1].Right = 360, stage5[1].Bottom = 405;	// ����
		stage5[2].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_CHAIR));
		stage5[2].Left = 300, stage5[2].Top = 475, stage5[2].Right = 455, stage5[2].Bottom = 625;	// ����
		stage5[3].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_CLOCK));
		stage5[3].Left = 750, stage5[3].Top = 265, stage5[3].Right = 860, stage5[3].Bottom = 330;	// �ð�
		stage5[4].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_PENDULUM));
		stage5[4].Left = 750, stage5[4].Top = 335, stage5[4].Right = 860, stage5[4].Bottom = 460;	// �ð���
		stage5[5].Left = 960, stage5[5].Top = 110, stage5[5].Right = 1010, stage5[5].Bottom = 545;	// ��ٸ�
		stage5[6].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_FLOWER));
		stage5[6].Left = 405, stage5[6].Top = 365, stage5[6].Right = 530, stage5[6].Bottom = 395;	// ��
		// Stage6 - ����
		Arrow.Left = rect.right + 100, Arrow.Top = Anna.y;
		Arrow.hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_ARROW));
		Blood.Left = 285, Blood.Top = 515, Blood.Bottom = 575;
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		backdc = CreateCompatibleDC(hdc);
		bgBitMap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
		bgoldBitMap = (HBITMAP)SelectObject(backdc, bgBitMap);

		memdc1 = CreateCompatibleDC(backdc);
		memdc2 = CreateCompatibleDC(memdc1);
		memdc3 = CreateCompatibleDC(memdc2);

		//����ȭ��
		if (Main){
			SelectObject(memdc1, hBitMap6_1);
			oldBitMap6_1 = (HBITMAP)SelectObject(memdc1, hBitMap6_1);

			StretchBlt(backdc, 0, 0, rect.right, rect.bottom, memdc1, 0, 0, 1280, 1280, SRCCOPY);

			SelectObject(memdc1, oldBitMap6_1);
		}
		// �� ��
		else if (Stage1){
			if (LAMP){
				SelectObject(memdc2, hBitMap1_1);
				oldBitMap1_1 = (HBITMAP)SelectObject(memdc2, hBitMap1_1);
			}
			else{
				SelectObject(memdc2, hBitMap1_2);
				oldBitMap1_2 = (HBITMAP)SelectObject(memdc2, hBitMap1_2);
			}

			StretchBlt(backdc, 0, 0, rect.right, rect.bottom, memdc2, 1280 * 0, 0, 1280, 1280, SRCCOPY);

			if (stage1[0].Sel){
				SelectObject(memdc1, stage1[0].hBitMap);
				stage1[0].oldBitMap = (HBITMAP)SelectObject(memdc1, stage1[0].hBitMap);
				StretchBlt(backdc, 200, 500, 800, 125, memdc1, 0, 0, 700, 200, SRCCOPY);
				SelectObject(memdc1, stage1[0].oldBitMap);
			}
			else if (stage1[1].Sel){
				SelectObject(memdc1, stage1[1].hBitMap);
				stage1[1].oldBitMap = (HBITMAP)SelectObject(memdc1, stage1[1].hBitMap);
				StretchBlt(backdc, 200, 500, 800, 125, memdc1, 0, 0, 700, 200, SRCCOPY);
				SelectObject(memdc1, stage1[1].oldBitMap);
			}
			else if (stage1[2].Sel){
				SelectObject(memdc1, stage1[2].hBitMap);
				stage1[2].oldBitMap = (HBITMAP)SelectObject(memdc1, stage1[2].hBitMap);
				StretchBlt(backdc, 200, 500, 800, 125, memdc1, 0, 0, 700, 200, SRCCOPY);
				SelectObject(memdc1, stage1[2].oldBitMap);
			}
			else if (stage1[3].Sel){
				SelectObject(memdc1, stage1[3].hBitMap);
				stage1[3].oldBitMap = (HBITMAP)SelectObject(memdc1, stage1[3].hBitMap);
				StretchBlt(backdc, 200, 500, 800, 125, memdc1, 0, 0, 700, 200, SRCCOPY);
				SelectObject(memdc1, stage1[3].oldBitMap);
			}
			else if (stage1[4].Sel){
				SelectObject(memdc1, stage1[4].hBitMap);
				stage1[4].oldBitMap = (HBITMAP)SelectObject(memdc1, stage1[4].hBitMap);
				StretchBlt(backdc, 200, 500, 800, 125, memdc1, 0, 0, 700, 200, SRCCOPY);
				SelectObject(memdc1, stage1[4].oldBitMap);
			}
			else if (stage1[6].Sel){
				SelectObject(memdc1, stage1[6].hBitMap);
				stage1[6].oldBitMap = (HBITMAP)SelectObject(memdc1, stage1[6].hBitMap);
				StretchBlt(backdc, 200, 500, 800, 125, memdc1, 0, 0, 700, 200, SRCCOPY);
				SelectObject(memdc1, stage1[6].oldBitMap);
			}
			else if (stage1[7].Sel){
				if (DIARY){
					SelectObject(memdc1, stage1[7].hBitMap1);
					stage1[7].oldBitMap1 = (HBITMAP)SelectObject(memdc1, stage1[7].hBitMap1);
					StretchBlt(backdc, 200, 50, 800, 525, memdc1, 0, 0, 295, 225, SRCCOPY);
					SelectObject(memdc1, stage1[7].oldBitMap1);
				}
				else if (DIARY == FALSE){
					SelectObject(memdc1, stage1[7].hBitMap);
					stage1[7].oldBitMap = (HBITMAP)SelectObject(memdc1, stage1[7].hBitMap);
					StretchBlt(backdc, 200, 500, 800, 125, memdc1, 0, 0, 700, 200, SRCCOPY);
					SelectObject(memdc1, stage1[7].oldBitMap);
				}
			}
			else if (stage1[8].Sel){
				SelectObject(memdc1, stage1[8].hBitMap);
				stage1[8].oldBitMap = (HBITMAP)SelectObject(memdc1, stage1[8].hBitMap);
				StretchBlt(backdc, 200, 500, 800, 125, memdc1, 0, 0, 700, 200, SRCCOPY);
				SelectObject(memdc1, stage1[8].oldBitMap);
			}
			else{
				if (Inven[n].Get){
					SelectObject(memdc1, Inven[n].hBitMap);
					Inven[n].oldBitMap = (HBITMAP)SelectObject(memdc1, Inven[n].hBitMap);
					StretchBlt(backdc, 1070, 525, rect.right - 1070, rect.bottom - 525, memdc1, 0, 0, 800, 600, SRCCOPY);
					SelectObject(memdc1, Inven[n].oldBitMap);
				}
				else{
					SelectObject(memdc1, Inven[0].hBitMap);
					Inven[0].oldBitMap = (HBITMAP)SelectObject(memdc1, Inven[0].hBitMap);
					StretchBlt(backdc, 1070, 525, rect.right - 1070, rect.bottom - 525, memdc1, 0, 0, 800, 600, SRCCOPY);
					SelectObject(memdc1, Inven[0].oldBitMap);
				}
			}
				/*wsprintf(buf, "%d, %d", mx, my);
				TextOut(backdc, mx + 10, my, buf, strlen(buf));*/
			if (LAMP){
				SelectObject(memdc2, oldBitMap1_1);
			}
			else{
				SelectObject(memdc2, oldBitMap1_2);
			}
		}
		// 1�� ����
		else if (Stage2){
			if (LAMP){
				SelectObject(memdc3, hBitMap1_1);
				oldBitMap1_1 = (HBITMAP)SelectObject(memdc3, hBitMap1_1);
			}
			else{
				SelectObject(memdc3, hBitMap1_2);
				oldBitMap1_2 = (HBITMAP)SelectObject(memdc3, hBitMap1_2);
			}

			StretchBlt(backdc, 0, 0, rect.right, rect.bottom, memdc3, 1280 * 1, 0, 1280, 1280, SRCCOPY);

			if (stage2[1].Sel){
				SelectObject(memdc2, stage2[1].hBitMap);
				stage2[1].oldBitMap = (HBITMAP)SelectObject(memdc2, stage2[1].hBitMap);
				StretchBlt(backdc, 200, 500, 800, 125, memdc2, 0, 0, 700, 200, SRCCOPY);
				SelectObject(memdc2, stage2[1].oldBitMap);
			}
			else if (stage2[2].Sel){
				SelectObject(memdc2, stage2[2].hBitMap);
				stage2[2].oldBitMap = (HBITMAP)SelectObject(memdc2, stage2[2].hBitMap);
				StretchBlt(backdc, 200, 500, 800, 125, memdc2, 0, 0, 700, 200, SRCCOPY);
				SelectObject(memdc2, stage2[2].oldBitMap);
			}
			else{
				if (Inven[n].Get){
					SelectObject(memdc2, Inven[n].hBitMap);
					Inven[n].oldBitMap = (HBITMAP)SelectObject(memdc2, Inven[n].hBitMap);
					StretchBlt(backdc, 1070, 525, rect.right - 1070, rect.bottom - 525, memdc2, 0, 0, 800, 600, SRCCOPY);
					SelectObject(memdc2, Inven[n].oldBitMap);
				}
				else{
					SelectObject(memdc2, Inven[0].hBitMap);
					Inven[0].oldBitMap = (HBITMAP)SelectObject(memdc2, Inven[0].hBitMap);
					StretchBlt(backdc, 1070, 525, rect.right - 1070, rect.bottom - 525, memdc2, 0, 0, 800, 600, SRCCOPY);
					SelectObject(memdc2, Inven[0].oldBitMap);
				}
			}

			SelectObject(memdc1, Right.hBitMap);
			Right.oldBitMap = (HBITMAP)SelectObject(memdc1, Right.hBitMap);
			TransparentBlt(backdc, Right.Left, Right.Top, LEN, LEN, memdc1, 0, 0, 150, 115, RGB(255, 255, 255));
			SelectObject(memdc1, Right.oldBitMap);

			/*	wsprintf(buf, "%d, %d", mx, my);
				TextOut(hdc, mx + 10, my, buf, strlen(buf));*/

			if (LAMP){
				SelectObject(memdc3, oldBitMap1_1);
			}
			else{
				SelectObject(memdc3, oldBitMap1_2);
			}
		}
		// 1�� ������
		else if (Stage3){
			if (LAMP){
				SelectObject(memdc3, hBitMap1_1);
				oldBitMap1_1 = (HBITMAP)SelectObject(memdc3, hBitMap1_1);
			}
			else{
				SelectObject(memdc3, hBitMap1_2);
				oldBitMap1_2 = (HBITMAP)SelectObject(memdc3, hBitMap1_2);
			}
			StretchBlt(backdc, 0, 0, rect.right, rect.bottom, memdc3, 1280 * 2, 0, 1280, 1280, SRCCOPY);

			if (stage3[0].Sel){
				SelectObject(memdc2, stage3[0].hBitMap);
				stage3[0].oldBitMap = (HBITMAP)SelectObject(memdc2, stage3[0].hBitMap);
				StretchBlt(backdc, 200, 500, 800, 125, memdc2, 0, 0, 700, 200, SRCCOPY);
				SelectObject(memdc2, stage3[0].oldBitMap);
			}
			else if (stage3[1].Sel){
				SelectObject(memdc2, stage3[1].hBitMap);
				stage3[1].oldBitMap = (HBITMAP)SelectObject(memdc2, stage3[1].hBitMap);
				StretchBlt(backdc, 200, 500, 800, 125, memdc2, 0, 0, 700, 200, SRCCOPY);
				SelectObject(memdc2, stage3[1].oldBitMap);
			}
			else if (stage3[2].Sel){
				SelectObject(memdc2, stage3[2].hBitMap);
				stage3[2].oldBitMap = (HBITMAP)SelectObject(memdc2, stage3[2].hBitMap);
				StretchBlt(backdc, 400, 50, 450, 525, memdc2, 0, 0, 600, 800, SRCCOPY);
				SelectObject(memdc2, stage3[2].oldBitMap);
			}
			else if (stage3[3].Sel){
				SelectObject(memdc2, stage3[3].hBitMap);
				stage3[3].oldBitMap = (HBITMAP)SelectObject(memdc2, stage3[3].hBitMap);
				StretchBlt(backdc, 200, 500, 800, 125, memdc2, 0, 0, 700, 200, SRCCOPY);
				SelectObject(memdc2, stage3[3].oldBitMap);
			}
			else if (stage3[4].Sel){
				SelectObject(memdc2, stage3[4].hBitMap);
				stage3[4].oldBitMap = (HBITMAP)SelectObject(memdc2, stage3[4].hBitMap);
				StretchBlt(backdc, 200, 500, 800, 125, memdc2, 0, 0, 700, 200, SRCCOPY);
				SelectObject(memdc2, stage3[4].oldBitMap);
			}
			else if (stage3[5].Sel){
				SelectObject(memdc2, stage3[5].hBitMap);
				stage3[5].oldBitMap = (HBITMAP)SelectObject(memdc2, stage3[5].hBitMap);
				StretchBlt(backdc, 200, 500, 800, 125, memdc2, 0, 0, 700, 200, SRCCOPY);
				SelectObject(memdc2, stage3[5].oldBitMap);
			}
			else if (stage3[6].Sel){
				SelectObject(memdc2, stage3[6].hBitMap);
				stage3[6].oldBitMap = (HBITMAP)SelectObject(memdc2, stage3[6].hBitMap);
				StretchBlt(backdc, 200, 500, 800, 125, memdc2, 0, 0, 700, 200, SRCCOPY);
				SelectObject(memdc2, stage3[6].oldBitMap);
			}
			else{
				if (Inven[n].Get){
					SelectObject(memdc2, Inven[n].hBitMap);
					Inven[n].oldBitMap = (HBITMAP)SelectObject(memdc2, Inven[n].hBitMap);
					StretchBlt(backdc, 1070, 525, rect.right - 1070, rect.bottom - 525, memdc2, 0, 0, 800, 600, SRCCOPY);
					SelectObject(memdc2, Inven[n].oldBitMap);
				}
				else{
					SelectObject(memdc2, Inven[0].hBitMap);
					Inven[0].oldBitMap = (HBITMAP)SelectObject(memdc2, Inven[0].hBitMap);
					StretchBlt(backdc, 1070, 525, rect.right - 1070, rect.bottom - 525, memdc2, 0, 0, 800, 600, SRCCOPY);
					SelectObject(memdc2, Inven[0].oldBitMap);
				}
			}

			SelectObject(memdc1, Left.hBitMap);
			Left.oldBitMap = (HBITMAP)SelectObject(memdc1, Left.hBitMap);
			TransparentBlt(backdc, Left.Left, Left.Top, LEN, LEN, memdc1, 0, 0, 150, 115, RGB(255, 255, 255));
			SelectObject(memdc1, Left.oldBitMap);

			SelectObject(memdc1, Right.hBitMap);
			Right.oldBitMap = (HBITMAP)SelectObject(memdc1, Right.hBitMap);
			TransparentBlt(backdc, Right.Left, Right.Top, LEN, LEN, memdc1, 0, 0, 150, 115, RGB(255, 255, 255));
			SelectObject(memdc1, Right.oldBitMap);

			/*wsprintf(buf, "%d, %d", mx, my);
			TextOut(backdc, mx + 10, my, buf, strlen(buf));*/

			if (LAMP){
				SelectObject(memdc3, oldBitMap1_1);
			}
			else{
				SelectObject(memdc3, oldBitMap1_2);
			}
		}
		// 1�� �ξ�
		else if (Stage4){
			if (LAMP){
				SelectObject(memdc3, hBitMap1_1);
				oldBitMap1_1 = (HBITMAP)SelectObject(memdc3, hBitMap1_1);
			}
			else{
				SelectObject(memdc3, hBitMap1_2);
				oldBitMap1_2 = (HBITMAP)SelectObject(memdc3, hBitMap1_2);
			}

			StretchBlt(backdc, 0, 0, rect.right, rect.bottom, memdc3, 1280 * 3, 0, 1280, 1280, SRCCOPY);

			if (stage4[0].Sel){
				SelectObject(memdc2, stage4[0].hBitMap);
				stage4[0].oldBitMap = (HBITMAP)SelectObject(memdc2, stage4[0].hBitMap);
				StretchBlt(backdc, 200, 500, 800, 125, memdc2, 0, 0, 700, 200, SRCCOPY);
				SelectObject(memdc2, stage4[0].oldBitMap);
			}
			else if (stage4[1].Sel){
				SelectObject(memdc2, stage4[1].hBitMap);
				stage4[1].oldBitMap = (HBITMAP)SelectObject(memdc2, stage4[1].hBitMap);
				StretchBlt(backdc, 200, 500, 800, 125, memdc2, 0, 0, 700, 200, SRCCOPY);
				SelectObject(memdc2, stage4[1].oldBitMap);
			}
			else if (stage4[2].Sel){
				SelectObject(memdc2, stage4[2].hBitMap);
				stage4[2].oldBitMap = (HBITMAP)SelectObject(memdc2, stage4[2].hBitMap);
				StretchBlt(backdc, 200, 500, 800, 125, memdc2, 0, 0, 700, 200, SRCCOPY);
				SelectObject(memdc2, stage4[2].oldBitMap);
			}
			else if (stage4[3].Sel){
				SelectObject(memdc2, stage4[3].hBitMap);
				stage4[3].oldBitMap = (HBITMAP)SelectObject(memdc2, stage4[3].hBitMap);
				StretchBlt(backdc, 200, 500, 800, 125, memdc2, 0, 0, 700, 200, SRCCOPY);
				SelectObject(memdc2, stage4[3].oldBitMap);
			}
			else if (stage4[4].Sel){
				SelectObject(memdc2, stage4[4].hBitMap);
				stage4[4].oldBitMap = (HBITMAP)SelectObject(memdc2, stage4[4].hBitMap);
				StretchBlt(backdc, 200, 500, 800, 125, memdc2, 0, 0, 700, 200, SRCCOPY);
				SelectObject(memdc2, stage4[4].oldBitMap);
			}
			else if (stage4[5].Sel){
				SelectObject(memdc2, stage4[5].hBitMap);
				stage4[5].oldBitMap = (HBITMAP)SelectObject(memdc2, stage4[5].hBitMap);
				StretchBlt(backdc, 200, 500, 800, 125, memdc2, 0, 0, 700, 200, SRCCOPY);
				SelectObject(memdc2, stage4[5].oldBitMap);
			}
			else if (stage4[6].Sel){
				SelectObject(memdc2, stage4[6].hBitMap);
				stage4[6].oldBitMap = (HBITMAP)SelectObject(memdc2, stage4[6].hBitMap);
				StretchBlt(backdc, 200, 500, 800, 125, memdc2, 0, 0, 700, 200, SRCCOPY);
				SelectObject(memdc2, stage4[6].oldBitMap);
			}
			else if (stage4[7].Sel){
				SelectObject(memdc2, stage4[7].hBitMap);
				stage4[7].oldBitMap = (HBITMAP)SelectObject(memdc2, stage4[7].hBitMap);
				StretchBlt(backdc, 200, 500, 800, 125, memdc2, 0, 0, 700, 200, SRCCOPY);
				SelectObject(memdc2, stage4[7].oldBitMap);
			}
			else if (stage4[8].Sel){
				SelectObject(memdc2, stage4[8].hBitMap);
				stage4[8].oldBitMap = (HBITMAP)SelectObject(memdc2, stage4[8].hBitMap);
				StretchBlt(backdc, 200, 500, 800, 125, memdc2, 0, 0, 700, 200, SRCCOPY);
				SelectObject(memdc2, stage4[8].oldBitMap);
			}
			else if (stage4[9].Sel){
				SelectObject(memdc2, stage4[9].hBitMap);
				stage4[9].oldBitMap = (HBITMAP)SelectObject(memdc2, stage4[9].hBitMap);
				StretchBlt(backdc, 200, 500, 800, 125, memdc2, 0, 0, 700, 200, SRCCOPY);
				SelectObject(memdc2, stage4[9].oldBitMap);
			}
			else{
				if (Inven[n].Get){
					SelectObject(memdc2, Inven[n].hBitMap);
					Inven[n].oldBitMap = (HBITMAP)SelectObject(memdc2, Inven[n].hBitMap);
					StretchBlt(backdc, 1070, 525, rect.right - 1070, rect.bottom - 525, memdc2, 0, 0, 800, 600, SRCCOPY);
					SelectObject(memdc2, Inven[n].oldBitMap);
				}
				else{
					SelectObject(memdc2, Inven[0].hBitMap);
					Inven[0].oldBitMap = (HBITMAP)SelectObject(memdc2, Inven[0].hBitMap);
					StretchBlt(backdc, 1070, 525, rect.right - 1070, rect.bottom - 525, memdc2, 0, 0, 800, 600, SRCCOPY);
					SelectObject(memdc2, Inven[0].oldBitMap);
				}
			}

			SelectObject(memdc1, Left.hBitMap);
			Left.oldBitMap = (HBITMAP)SelectObject(memdc1, Left.hBitMap);
			TransparentBlt(backdc, Left.Left, Left.Top, LEN, LEN, memdc1, 0, 0, 150, 115, RGB(255, 255, 255));
			SelectObject(memdc1, Left.oldBitMap);

			SelectObject(memdc1, Right.hBitMap);
			Right.oldBitMap = (HBITMAP)SelectObject(memdc1, Right.hBitMap);
			TransparentBlt(backdc, Right.Left, Right.Top, LEN, LEN, memdc1, 0, 0, 150, 115, RGB(255, 255, 255));
			SelectObject(memdc1, Right.oldBitMap);

			/*wsprintf(buf, "%d, %d", mx, my);
			TextOut(hdc, mx + 10, my, buf, strlen(buf));*/

			if (LAMP){
				SelectObject(memdc3, oldBitMap1_1);
			}
			else{
				SelectObject(memdc3, oldBitMap1_2);
			}
		}
		// 1�� ��Ź
		else if (Stage5){
			if (LAMP){
				SelectObject(memdc3, hBitMap1_1);
				oldBitMap1_1 = (HBITMAP)SelectObject(memdc3, hBitMap1_1);
			}
			else{
				SelectObject(memdc3, hBitMap1_2);
				oldBitMap1_2 = (HBITMAP)SelectObject(memdc3, hBitMap1_2);
			}

			StretchBlt(backdc, 0, 0, rect.right, rect.bottom, memdc3, 1280 * 4, 0, 1280, 1280, SRCCOPY);

			if (stage5[0].Sel){
				SelectObject(memdc2, stage5[0].hBitMap);
				stage5[0].oldBitMap = (HBITMAP)SelectObject(memdc2, stage5[0].hBitMap);
				StretchBlt(backdc, 200, 500, 800, 125, memdc2, 0, 0, 700, 200, SRCCOPY);
				SelectObject(memdc2, stage5[0].oldBitMap);
			}
			else if (stage5[1].Sel){
				SelectObject(memdc2, stage5[1].hBitMap);
				stage5[1].oldBitMap = (HBITMAP)SelectObject(memdc2, stage5[1].hBitMap);
				StretchBlt(backdc, 200, 500, 800, 125, memdc2, 0, 0, 700, 200, SRCCOPY);
				SelectObject(memdc2, stage5[1].oldBitMap);
			}
			else if (stage5[2].Sel){
				SelectObject(memdc2, stage5[2].hBitMap);
				stage5[2].oldBitMap = (HBITMAP)SelectObject(memdc2, stage5[2].hBitMap);
				StretchBlt(backdc, 200, 500, 800, 125, memdc2, 0, 0, 700, 200, SRCCOPY);
				SelectObject(memdc2, stage5[2].oldBitMap);
			}
			else if (stage5[3].Sel){
				SelectObject(memdc2, stage5[3].hBitMap);
				stage5[3].oldBitMap = (HBITMAP)SelectObject(memdc2, stage5[3].hBitMap);
				StretchBlt(backdc, 200, 500, 800, 125, memdc2, 0, 0, 700, 200, SRCCOPY);
				SelectObject(memdc2, stage5[3].oldBitMap);
			}
			else if (stage5[4].Sel){
				SelectObject(memdc2, stage5[4].hBitMap);
				stage5[4].oldBitMap = (HBITMAP)SelectObject(memdc2, stage5[4].hBitMap);
				StretchBlt(backdc, 200, 500, 800, 125, memdc2, 0, 0, 700, 200, SRCCOPY);
				SelectObject(memdc2, stage5[4].oldBitMap);
			}
			else if (stage5[6].Sel){
				SelectObject(memdc2, stage5[6].hBitMap);
				stage5[6].oldBitMap = (HBITMAP)SelectObject(memdc2, stage5[6].hBitMap);
				StretchBlt(backdc, 200, 500, 800, 125, memdc2, 0, 0, 700, 200, SRCCOPY);
				SelectObject(memdc2, stage5[6].oldBitMap);
			}
			else{
				if (Inven[n].Get){
					SelectObject(memdc2, Inven[n].hBitMap);
					Inven[n].oldBitMap = (HBITMAP)SelectObject(memdc2, Inven[n].hBitMap);
					StretchBlt(backdc, 1070, 525, rect.right - 1070, rect.bottom - 525, memdc2, 0, 0, 800, 600, SRCCOPY);
					SelectObject(memdc2, Inven[n].oldBitMap);
				}
				else{
					SelectObject(memdc2, Inven[0].hBitMap);
					Inven[0].oldBitMap = (HBITMAP)SelectObject(memdc2, Inven[0].hBitMap);
					StretchBlt(backdc, 1070, 525, rect.right - 1070, rect.bottom - 525, memdc2, 0, 0, 800, 600, SRCCOPY);
					SelectObject(memdc2, Inven[0].oldBitMap);
				}
			}

			SelectObject(memdc1, Left.hBitMap);
			Left.oldBitMap = (HBITMAP)SelectObject(memdc1, Left.hBitMap);
			TransparentBlt(backdc, Left.Left, Left.Top, LEN, LEN, memdc1, 0, 0, 150, 115, RGB(255, 255, 255));
			SelectObject(memdc1, Left.oldBitMap);

			wsprintf(buf, "%d, %d", mx, my);
			TextOut(backdc, mx + 10, my, buf, strlen(buf));

			if (LAMP){
				SelectObject(memdc3, oldBitMap1_1);
			}
			else{
				SelectObject(memdc3, oldBitMap1_2);
			}
		}
		// ĳ����, �� ��
		else if (Stage6){
			// ���
			SelectObject(memdc3, hBitMap3);
			oldBitMap3 = (HBITMAP)SelectObject(memdc3, hBitMap3);
			StretchBlt(backdc, 0, 0, 1280 * 4, rect.bottom, memdc3, Anna.x * 8 - LEN*4, 0, 1280 * 8-LEN*9 , 1280, SRCCOPY);
			// �ȳ�
			SelectObject(memdc2, hBitMap2);
			oldBitMap2 = (HBITMAP)SelectObject(memdc2, hBitMap2);
			TransparentBlt(backdc, Anna.x, Anna.y, 120, 120, memdc2, 500 * w, 0, 500, 955, RGB(255, 255, 255));
			// ����
			if (WITCH[0] && WITCH[1] == FALSE){
				SelectObject(memdc2, hBitMap5);
				oldBitMap5 = (HBITMAP)SelectObject(memdc2, hBitMap5);
				TransparentBlt(backdc, rect.right - Witch.x, Witch.y, 120, 120, memdc2, 500 * w1, 0, 500, 955, RGB(255, 255, 255));
				// ��
				SelectObject(memdc1, hBitMap4_2);
				oldBitMap4_2 = (HBITMAP)SelectObject(memdc1, hBitMap4_2);
				TransparentBlt(backdc, rect.right - 150, 0, 100, 100, memdc1, 0, 0, 180, 150, RGB(255, 255, 255));
			}
			else{
				// ��
				SelectObject(memdc1, hBitMap4_1);
				oldBitMap4_1 = (HBITMAP)SelectObject(memdc1, hBitMap4_1);
				TransparentBlt(backdc, rect.right - 150, 0, 100, 100, memdc1, 0, 0, 125, 125, RGB(255, 255, 255));
			}
			
			// ����
			if (Arrow.Sel){
				SelectObject(memdc1, Arrow.hBitMap);
				Arrow.oldBitMap = (HBITMAP)SelectObject(memdc1, Arrow.hBitMap);
				TransparentBlt(backdc, Arrow.Left, Arrow.Top, 100, 100, memdc1, 0, 0, 800, 600, RGB(255, 255, 255));
			}

			SelectObject(memdc2, oldBitMap2);
			SelectObject(memdc3, oldBitMap3);
			SelectObject(memdc1, oldBitMap4_1);
			SelectObject(memdc1, oldBitMap4_2);

		}
		else if (Gameover){
			//���
			SelectObject(memdc1, hBitMap6_2);
			oldBitMap6_2 = (HBITMAP)SelectObject(memdc1, hBitMap6_2);
			StretchBlt(backdc, 0, 0, rect.right, rect.bottom, memdc1, 0, 0, 544, 416, SRCCOPY);

			SelectObject(memdc1, oldBitMap6_2);

			Gameover = FALSE;
			Main = TRUE;
		}
		else if (HappyEnd){
			//���
			SelectObject(memdc1, hBitMap7_1);
			oldBitMap7_1 = (HBITMAP)SelectObject(memdc1, hBitMap7_1);
			StretchBlt(backdc, 0, 0, rect.right, rect.bottom, memdc1, 0, 0, 2000, 2000, SRCCOPY);

			SelectObject(memdc1, oldBitMap7_1);

			HappyEnd = FALSE;
			Main = TRUE;
		}
		else if (TrueEnd){
			//���
			SelectObject(memdc1, hBitMap7_2);
			oldBitMap7_2 = (HBITMAP)SelectObject(memdc1, hBitMap7_2);
			StretchBlt(backdc, 0, 0, rect.right, rect.bottom, memdc1, 0, 0, 1280, 1280, SRCCOPY);

			SelectObject(memdc1, oldBitMap7_2);

			TrueEnd = FALSE;
			Main = TRUE;
		}


		BitBlt(hdc, 0, 0, rect.right, rect.bottom, backdc, 0, 0, SRCCOPY);

		SelectObject(hdc, bgoldBitMap);
		DeleteObject(bgoldBitMap);

		DeleteDC(memdc3);
		DeleteDC(memdc2);
		DeleteDC(memdc1);
		DeleteDC(backdc);

		EndPaint(hWnd, &ps);
		break;
	case WM_LBUTTONDOWN:
		mx = LOWORD(lParam);
		my = HIWORD(lParam);
		if (Stage6 == FALSE){
			if (Inven[n].Get)
				Inven[n].Equip = TRUE;
			if (Main){
				if (InStart(mx, my)){
					Main = FALSE;
					Stage1 = TRUE;
					PlaySound("������ �Ҹ�", NULL, SND_ASYNC | SND_LOOP);
				}
				else if (InEnd(mx, my)){
					PostQuitMessage(0);
				}
			}
			else if (Stage1){

				if (SelCheck(stage1, 9)){
					Message = TRUE;
					// ������ �״� å �а� ���
					if (stage1[2].Sel && InOk(mx, my)){
						stage1[2].Sel = FALSE;

						Stage1 = FALSE;
						Gameover = TRUE;
						ENDDING[2] = FALSE;
						InvalidateRect(hWnd, NULL, FALSE);

						PlaySound("NULL", 0, 0);
						PlaySound("��� ������ �Ҹ�", NULL, SND_SYNC);
						// �ʱ�ȭ �����ֱ�
						Message = FALSE;
						// �ȳ�
						Anna.x = 50, Anna.y = 435, Anna.acc = 0.1, wcnt = 0, Anna.gravity = 3;
						Anna.angle = 0, Anna.scale_x = 0, Anna.scale_y = 0, Anna.Jump = FALSE;
						Anna.speed = 0, Anna.vel_x = Anna.speed*Anna.scale_x, Anna.vel_y = Anna.speed * Anna.scale_y;
						// �κ��丮
						ROAP = FALSE;
						for (int i = 0; i < 8; ++i)
							Inven[i].Get = FALSE;
						stage1[1].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_LADDER));	//��ٸ�
						stage1[8].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_CHAIN));	//��罽
						PlaySound("���� ���", NULL, SND_ASYNC | SND_LOOP);
						break;
					}
					else if (stage1[2].Sel && InNo(mx, my)){
						stage1[2].Sel = FALSE;
					}
					// ���� ȹ��
					if (Lock == 0 && stage1[4].Sel){
						Inven[1].Get = TRUE;
					}
					else if (Lock == 1 && stage1[3].Sel){
						Inven[1].Get = TRUE;
					}
					// �б��� �б� ����
					if (stage1[7].Sel == TRUE && InOk(mx, my)){
						DIARY = TRUE, ENDDING[0] = TRUE;
						InvalidateRect(hWnd, NULL, FALSE);
						break;
					}
					else if (stage1[7].Sel == TRUE && InNo(mx, my)){
						DIARY = FALSE;
						stage1[7].Sel = FALSE;
					}
				}
				else{
					DIARY = FALSE;
					Message = FALSE;
				}

				if (InBoxFULL(mx, my, 9, stage1)){
					if (Message == FALSE){
						for (int i = 0; i < 9; ++i){
							if (InBox(mx, my, stage1[i]))
								stage1[i].Sel = TRUE;

							// ����� ��ٸ��� Ÿ�� ������
							if (stage1[0].Sel && ROAP){
								Inven[1].Get = FALSE;
								PlaySound("NULL", 0, 0);
								PlaySound("�ð���ħ �Ҹ�", NULL, SND_ASYNC | SND_LOOP);
								Stage2 = TRUE;
								Main = FALSE;
								Stage1 = FALSE;
								Stage3 = FALSE;
								Stage4 = FALSE;
								Stage5 = FALSE;
								Stage6 = FALSE;
								InvalidateRect(hWnd, NULL, FALSE);
								break;
							}
							// ������ ��ٸ��� ����
							if (stage1[1].Sel && Inven[1].Equip){
								stage1[1].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_HANGEDLADDER));
								Inven[1].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_INVLADDER));
								Inven[1].USE = TRUE;
								break;
							}
							// ��Ź�� ����� ��
							if (Lock == 0 && stage1[3].Sel && Inven[8].Equip){
								Inven[9].Get = TRUE;
								stage1[3].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_KEY1));
								break;
							}
							else if (Lock == 1 && stage1[4].Sel && Inven[8].Equip){
								Inven[9].Get = TRUE;
								stage1[4].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_KEY1));
								break;
							}
							// ���ĵ� ���� Ű��
							if (stage1[5].Sel && LAMP == TRUE){
								LAMP = FALSE;
								stage1[5].Sel = FALSE;
								break;
							}
							else if (stage1[5].Sel && LAMP == FALSE){
								LAMP = TRUE;
								stage1[5].Sel = FALSE;
								break;
							}
							// ��Į�� ���� ����
							if (stage1[6].Sel && Inven[4].Equip){
								stage1[6].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_KEY1));
								Inven[8].Get = TRUE;
								break;
							}
							else if (stage1[6].Sel && Inven[4].Equip == FALSE){
								stage1[6].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_CUSHION));
								break;
							}
							// ��ٸ��� ����� ������ ��罽���� ����
							if (stage1[8].Sel && Inven[1].USE && Inven[1].Equip){
								stage1[8].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_HANGEDCHAIN));
								ROAP = TRUE;
								break;
							}
						}
					}
					else{
						break;
					}
				}
				else{
					for (int i = 0; i < 9; ++i){
						stage1[i].Sel = FALSE;
					}
				}
			}

			else if (Stage2){

				if (SelCheck(stage2, 3))
					Message = TRUE;
				else{
					Message = FALSE;
					if (InBox(mx, my, Right)){
						Stage3 = TRUE;
						Stage2 = FALSE;
						break;
					}
					else if (InBox(mx, my, stage2[0])){
						Stage1 = TRUE;
						Stage2 = FALSE;
						stage1[0].Sel = FALSE;
						PlaySound("NULL", 0, 0);
						PlaySound("������ �Ҹ�", NULL, SND_ASYNC | SND_LOOP);
						InvalidateRect(hWnd, NULL, FALSE);
						break;
					}
				}

				if (InBoxFULL(mx, my, 3, stage2)){
					if (Message == FALSE){
						for (int i = 0; i < 3; ++i){
							if (InBox(mx, my, stage2[i]))
								stage2[i].Sel = TRUE;

							// ����� ������
							if (stage2[1].Sel && Inven[9].Equip){
								PlaySound("NULL", 0, 0);
								PlaySound("���ӿ���", NULL, SND_ASYNC | SND_LOOP);
								Stage6 = TRUE;
								Main = FALSE;
								Stage1 = FALSE;
								Stage2 = FALSE;
								Stage3 = FALSE;
								Stage4 = FALSE;
								Stage5 = FALSE;
								InvalidateRect(hWnd, NULL, FALSE);
								break;
							}
							// ���÷� ��� ����
							if (stage2[2].Sel && Inven[2].Equip){
								Inven[6].Get = TRUE;
								stage2[2].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BLACK));
								InvalidateRect(hWnd, NULL, FALSE);
								break;
							}
						}
					}
					else{
						break;
					}
				}
				else{
					for (int i = 0; i < 3; ++i){
						stage2[i].Sel = FALSE;
					}
				}

			}
			else if (Stage3){
				if (SelCheck(stage3, 7))
					Message = TRUE;
				else{
					Message = FALSE;

					if (InBox(mx, my, Right)){
						Stage4 = TRUE;
						Stage3 = FALSE;
						InvalidateRect(hWnd, NULL, FALSE);
					}
					else if (InBox(mx, my, Left)){
						Stage2 = TRUE;
						Stage3 = FALSE;
						InvalidateRect(hWnd, NULL, FALSE);
					}
				}

				if (InBoxFULL(mx, my, 7, stage3)){
					if (Message == FALSE){
						for (int i = 0; i < 7; ++i){
							if (InBox(mx, my, stage3[i]))
								stage3[i].Sel = TRUE;
							// ���÷� ��� ����
							if (stage3[0].Sel && Inven[2].Equip){
								Inven[6].Get = TRUE;
								stage3[0].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BLACK));
								InvalidateRect(hWnd, NULL, FALSE);
								break;
							}
							// ���� ȹ��
							if (stage3[1].Sel){
								Inven[2].Get = TRUE;
								InvalidateRect(hWnd, NULL, FALSE);
							}
							// ���� Ȯ��
							if (stage3[2].Sel){
								ENDDING[1] = TRUE;
							}
							if (stage3[3].Sel && Inven[3].Equip){
								stage3[3].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_LIGHT));
								Inven[3].USE = TRUE;
								Inven[3].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_INVLIGHT));
								break;
							}
						}
					}
					else{
						break;
					}
				}
				else{
					for (int i = 0; i < 7; ++i){
						stage3[i].Sel = FALSE;
					}
				}
			}
			else if (Stage4){
				if (SelCheck(stage4, 10)){
					Message = TRUE;
					if (stage4[0].Sel && InOk(mx, my)){
						stage4[0].Sel = FALSE;

						Stage4 = FALSE;
						Gameover = TRUE;
						ENDDING[2] = FALSE;
						InvalidateRect(hWnd, NULL, FALSE);

						PlaySound("NULL", 0, 0);
						PlaySound("��� ������ �Ҹ�", NULL, SND_SYNC);
						// �ʱ�ȭ �����ֱ�
						Message = FALSE;
						// �ȳ�
						Anna.x = 50, Anna.y = 435, Anna.acc = 0.1, wcnt = 0, Anna.gravity = 3;
						Anna.angle = 0, Anna.scale_x = 0, Anna.scale_y = 0, Anna.Jump = FALSE;
						Anna.speed = 0, Anna.vel_x = Anna.speed*Anna.scale_x, Anna.vel_y = Anna.speed * Anna.scale_y;
						// �κ��丮
						ROAP = FALSE;
						for (int i = 0; i < 8; ++i)
							Inven[i].Get = FALSE;
						stage1[1].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_LADDER));	//��ٸ�
						stage1[8].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_CHAIN));	//��罽
						break;
					}
					else if (stage4[0].Sel && InNo(mx, my)){
						stage4[0].Sel = FALSE;
					}
				}
				else{
					Message = FALSE;
					if (InBox(mx, my, Right)){
						Stage5 = TRUE;
						Stage4 = FALSE;
					}
					else if (InBox(mx, my, Left)){
						Stage3 = TRUE;
						Stage4 = FALSE;
					}
				}

				if (InBoxFULL(mx, my, 10, stage4)){
					if (Message == FALSE){
						for (int i = 0; i < 10; ++i){
							if (InBox(mx, my, stage4[i]))
								stage4[i].Sel = TRUE;

							// ������ �����
							if (stage4[0].Sel && Inven[5].Equip){
								stage4[0].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_WATER));
								Inven[5].USE = TRUE;
								break;
							}
							else if (stage4[0].Sel && Inven[5].Equip == FALSE){
								stage4[0].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BUCKET));
								break;
							}
							// ���Ͽ� ��
							if (stage4[1].Sel){
								Inven[3].Get = TRUE;
								break;
							}
							// ��Į
							else if (stage4[8].Sel){
								Inven[4].Get = TRUE;
								break;
							}
						}
					}
					else{
						break;
					}
				}
				else{
					for (int i = 0; i < 10; ++i){
						stage4[i].Sel = FALSE;
					}
				}
			}
			else if (stage5){
				if (SelCheck(stage5, 7))
					Message = TRUE;
				else{
					Message = FALSE;
					if (InBox(mx, my, Left)){
						Stage4 = TRUE;
						Stage5 = FALSE;
					}
					else if (InBox(mx, my, stage5[5])){
						Stage1 = TRUE;
						Stage5 = FALSE;
						stage1[0].Sel = FALSE;
						PlaySound("NULL", 0, 0);
						PlaySound("������ �Ҹ�", NULL, SND_ASYNC | SND_LOOP);
					}
				}

				if (InBoxFULL(mx, my, 7, stage5)){
					if (Message == FALSE){
						for (int i = 0; i < 7; ++i){
							if (InBox(mx, my, stage5[i]))
								stage5[i].Sel = TRUE;

							// ���ڿ� ���Ѹ���
							if (stage5[0].Sel && Inven[5].USE && Inven[5].Equip){
								hBitMap1_1 = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_STAGE1_1_1));	// �� ��(����ON)	
								hBitMap1_2 = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_STAGE1_2_1));	// �� ��(����OFF)	
								stage5[0].Sel = FALSE;
								break;
							}
							// �ɺ� ȹ��
							if (stage5[1].Sel){
								Inven[5].Get = TRUE;
								break;
							}
						}
					}
					else{
						break;
					}
				}
				else{
					for (int i = 0; i < 7; ++i){
						stage5[i].Sel = FALSE;
					}
				}
			}
		}
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_MOUSEMOVE:
		mx = LOWORD(lParam);
		my = HIWORD(lParam);
		if (Inven[n].Get)
			Inven[n].Equip = TRUE;
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_KEYDOWN:
		if (wParam == VK_LEFT){
			if (Stage6){
				if (w < 6)
					w = 10;
				++wcnt;
				if (wcnt % 5 == 0){
					++w;
				}
				if (w > 12)
					w = 7;
				if (Anna.x > 0 && Anna.x * 2 > LEN){
					Anna.scale_x = cos(Anna.angle);
					if (Anna.speed > -3)
						Anna.speed -= Anna.acc;
					Anna.vel_x = Anna.speed*Anna.scale_x;
					Anna.x += Anna.vel_x;
				}
			}
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		}
		else if (wParam == VK_RIGHT){
			if (Stage6){
				++wcnt;
				if (wcnt % 5 == 0){
					++w;
				}
				if (w > 6)
					w = 0;
				if (Anna.x < rect.right - 125){
					Anna.scale_x = cos(Anna.angle);
					if (Anna.speed < 2)
						Anna.speed += Anna.acc;
					Anna.vel_x = Anna.speed*Anna.scale_x;
					Anna.x += Anna.vel_x;
				}
				else{
					for (int i = 0; i < 3; ++i){
						if (ENDDING[i] == TRUE)
							++Ecnt;
					}
					if (Ecnt == 3){
						TrueEnd = TRUE;
						Stage6 = FALSE;
						InvalidateRect(hWnd, NULL, FALSE);
						PlaySound("NULL", 0, 0);
						PlaySound("������", NULL, SND_ASYNC);
						Sleep(10000);
						// �ʱ�ȭ �����ֱ�
						Message = FALSE;
						// �ȳ�
						Anna.x = 50, Anna.y = 435, Anna.acc = 0.1, wcnt = 0, Anna.gravity = 3;
						Anna.angle = 0, Anna.scale_x = 0, Anna.scale_y = 0, Anna.Jump = FALSE;
						Anna.speed = 0, Anna.vel_x = Anna.speed*Anna.scale_x, Anna.vel_y = Anna.speed * Anna.scale_y;
						// �κ��丮
						ROAP = FALSE;
						for (int i = 0; i < 8; ++i)
							Inven[i].Get = FALSE;
						stage1[1].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_LADDER));	//��ٸ�
						stage1[8].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_CHAIN));	//��罽
						Arrow.Left = rect.right - 100;
						PlaySound("���� ���", NULL, SND_ASYNC | SND_LOOP);
					}
					else{
						HappyEnd = TRUE;
						Stage6 = FALSE;
						InvalidateRect(hWnd, NULL, FALSE);
						PlaySound("NULL", 0, 0);
						PlaySound("����", NULL, SND_ASYNC);
						Sleep(10000);
						// �ʱ�ȭ �����ֱ�
						Message = FALSE;
						// �ȳ�
						Anna.x = 50, Anna.y = 435, Anna.acc = 0.1, wcnt = 0, Anna.gravity = 3;
						Anna.angle = 0, Anna.scale_x = 0, Anna.scale_y = 0, Anna.Jump = FALSE;
						Anna.speed = 0, Anna.vel_x = Anna.speed*Anna.scale_x, Anna.vel_y = Anna.speed * Anna.scale_y;
						// �κ��丮
						ROAP = FALSE;
						for (int i = 0; i < 8; ++i)
							Inven[i].Get = FALSE;
						stage1[1].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_LADDER));	//��ٸ�
						stage1[8].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_CHAIN));	//��罽
						Arrow.Left = rect.right - 100;
						PlaySound("���� ���", NULL, SND_ASYNC | SND_LOOP);

					}
				}
				if (95 < Anna.x  && Anna.x < 115 && 515 < Anna.y + 120 && Anna.y < 570){
					SetTimer(hWnd, 4, 1, NULL);
					Arrow.Sel = TRUE;
				}
				if (470 < Anna.x  && Anna.x < 490){
					SetTimer(hWnd, 5, 10, NULL);
					WITCH[0] = TRUE;
				}
			}
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		}
		else if (wParam == VK_UP){
			if (Stage6){
				SetTimer(hWnd, 1, 100, NULL);

				if (GetAsyncKeyState(VK_RIGHT) & 0x8000){
					++wcnt;
					if (wcnt % 5 == 0){
						++w;
					}
					if (w > 6)
						w = 0;
					if (Anna.x < rect.right - 125){
						Anna.scale_x = cos(Anna.angle);
						if (Anna.speed < 1.5)
							Anna.speed += Anna.acc;
						Anna.vel_x = Anna.speed*Anna.scale_x;
						Anna.x += Anna.vel_x;
					}
				}
				InvalidateRect(hWnd, NULL, FALSE);
			}
			break;
		}
		else if (wParam == 'i' || wParam == 'I'){
			Inven[n++].Equip = FALSE;
			InvalidateRect(hWnd, NULL, FALSE);
			if (n > 9)
				n = 0;
			break;
		}
		break;
	case WM_KEYUP:
		switch (wParam){
		case VK_LEFT:
			Anna.speed = 0;
			w = 10;
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case VK_RIGHT:
			w = 3;
			Anna.speed = 0;
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		}
		break;
	case WM_TIMER:
		switch (wParam){
		case 1:
			if (Anna.Jump == FALSE){
				Anna.vel_y -= 20;
				Anna.Jump = TRUE;
			}
			if (Anna.Jump){
				if (Anna.y + Anna.vel_y > 335){
					Anna.y += Anna.vel_y;
					Anna.vel_y -= Anna.gravity;
				}
				else{
					Anna.Jump = FALSE;
					KillTimer(hWnd, 1);
					SetTimer(hWnd, 2, 100, NULL);
				}
			}
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case 2:
			if (Anna.Jump == FALSE){
				Anna.vel_y += 20;
				Anna.Jump = TRUE;
			}
			if (Anna.Jump){
				if (Anna.y + Anna.vel_y < 435){
					Anna.y += Anna.vel_y;
					Anna.vel_y += Anna.gravity;
				}
				else{
					Anna.Jump = FALSE;
					KillTimer(hWnd, 2);
				}
			}
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case 4:
			Arrow.Left -= 50;
			InvalidateRect(hWnd, NULL, FALSE);
			if (Arrow.Left < Anna.x + 40){
				KillTimer(hWnd, 4);
				Arrow.Sel = FALSE;

				Stage6 = FALSE, ENDDING[2] = FALSE;
				Gameover = TRUE;
				InvalidateRect(hWnd, NULL, FALSE);

				PlaySound("NULL", 0, 0);
				PlaySound("��� ������ �Ҹ�", NULL, SND_SYNC);
				// �ʱ�ȭ �����ֱ�
				Message = FALSE;
				// �ȳ�
				Anna.x = 50, Anna.y = 435, Anna.acc = 0.1, wcnt = 0, Anna.gravity = 3;
				Anna.angle = 0, Anna.scale_x = 0, Anna.scale_y = 0, Anna.Jump = FALSE;
				Anna.speed = 0, Anna.vel_x = Anna.speed*Anna.scale_x, Anna.vel_y = Anna.speed * Anna.scale_y;
				// �κ��丮
				ROAP = FALSE;
				for (int i = 0; i < 8; ++i)
					Inven[i].Get = FALSE;
				stage1[1].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_LADDER));	//��ٸ�
				stage1[8].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_CHAIN));	//��罽
				Arrow.Left = rect.right - 100;
				PlaySound("���� ���", NULL, SND_ASYNC | SND_LOOP);
			}
			break;
		case 5:
			++wcnt1;
			if (wcnt1 % 5 == 0){
				++w1;
			}
			if (w1 > 6)
				w1 = 0;
			Witch.scale_x = cos(Witch.angle);
			if (Witch.speed < 5)
				Witch.speed += Witch.acc;
			Witch.vel_x = Witch.speed*Witch.scale_x;
			Witch.x += Witch.vel_x;
			InvalidateRect(hWnd, NULL, FALSE);
			
			if (rect.right / 3 < rect.right - Witch.x || rect.right - Witch.x < rect.right / 3 * 2){
				KillTimer(hWnd, 5);
				SetTimer(hWnd, 5, 7, NULL);
				InvalidateRect(hWnd, NULL, FALSE);
			}
			if (rect.right / 3 > rect.right - Witch.x){
				KillTimer(hWnd, 5);
				WITCH[0] = FALSE;
				WITCH[1] = TRUE;
			}
			// ���࿡�� ������
			if (Anna.x + 60 > rect.right - Witch.x && WITCH[0] == TRUE){
				KillTimer(hWnd, 5);
				WITCH[0] = FALSE;
				WITCH[1] = TRUE;
				Stage6 = FALSE, ENDDING[2] = FALSE;
				Gameover = TRUE;
				InvalidateRect(hWnd, NULL, FALSE);

				PlaySound("NULL", 0, 0);
				PlaySound("��� ������ �Ҹ�", NULL, SND_SYNC);
				// �ʱ�ȭ �����ֱ�
				Message = FALSE;
				// �ȳ�
				Anna.x = 50, Anna.y = 435, Anna.acc = 0.1, wcnt = 0, Anna.gravity = 3;
				Anna.angle = 0, Anna.scale_x = 0, Anna.scale_y = 0, Anna.Jump = FALSE;
				Anna.speed = 0, Anna.vel_x = Anna.speed*Anna.scale_x, Anna.vel_y = Anna.speed * Anna.scale_y;
				// �κ��丮
				ROAP = FALSE;
				for (int i = 0; i < 8; ++i)
					Inven[i].Get = FALSE;
				stage1[1].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_LADDER));	//��ٸ�
				stage1[8].hBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_CHAIN));	//��罽
				Arrow.Left = rect.right - 100;
				PlaySound("���� ���", NULL, SND_ASYNC | SND_LOOP);
				break;
			}
				
			break;
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_NEWGAME:

			break;
		case ID_STAGE1:
			PlaySound("NULL", 0, 0);
			PlaySound("������ �Ҹ�", NULL, SND_ASYNC | SND_LOOP);
			Stage1 = TRUE;
			Main = FALSE;
			Stage2 = FALSE;
			Stage3 = FALSE;
			Stage4 = FALSE;
			Stage5 = FALSE;
			Stage6 = FALSE;
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case ID_STAGE2:
			PlaySound("NULL", 0, 0);
			PlaySound("�ð���ħ �Ҹ�", NULL, SND_ASYNC | SND_LOOP);
			Stage2 = TRUE;
			Main = FALSE;
			Stage1 = FALSE;
			Stage3 = FALSE;
			Stage4 = FALSE;
			Stage5 = FALSE;
			Stage6 = FALSE;
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case ID_STAGE3:
			PlaySound("NULL", 0, 0);
			PlaySound("�ð���ħ �Ҹ�", NULL, SND_ASYNC | SND_LOOP);
			Stage3 = TRUE;
			Main = FALSE;
			Stage1 = FALSE;
			Stage2 = FALSE;
			Stage4 = FALSE;
			Stage5 = FALSE;
			Stage6 = FALSE;
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case ID_STAGE4:
			PlaySound("NULL", 0, 0);
			PlaySound("�ð���ħ �Ҹ�", NULL, SND_ASYNC | SND_LOOP);
			Stage4 = TRUE;
			Main = FALSE;
			Stage1 = FALSE;
			Stage2 = FALSE;
			Stage3 = FALSE;
			Stage5 = FALSE;
			Stage6 = FALSE;
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case ID_STAGE5:
			PlaySound("NULL", 0, 0);
			PlaySound("�ð���ħ �Ҹ�", NULL, SND_ASYNC | SND_LOOP);
			Stage5 = TRUE;
			Main = FALSE;
			Stage1 = FALSE;
			Stage2 = FALSE;
			Stage3 = FALSE;
			Stage4 = FALSE;
			Stage6 = FALSE;
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case ID_STAGE6:
			PlaySound("NULL", 0, 0);
			PlaySound("���ӿ���", NULL, SND_ASYNC | SND_LOOP);
			Anna.x = 50, Anna.y = 435, Anna.acc = 0.1, wcnt = 0, Anna.gravity = 3;
			Anna.angle = 0, Anna.scale_x = 0, Anna.scale_y = 0, Anna.Jump = FALSE;
			Anna.speed = 0, Anna.vel_x = Anna.speed*Anna.scale_x, Anna.vel_y = Anna.speed * Anna.scale_y;
			Stage6 = TRUE;
			Main = FALSE;
			Stage1 = FALSE;
			Stage2 = FALSE;
			Stage3 = FALSE;
			Stage4 = FALSE;
			Stage5 = FALSE;
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case ID_END:
			PostQuitMessage(0);
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