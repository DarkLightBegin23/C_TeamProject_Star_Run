#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>

#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80
#define EMPTY 100
#define SCREEN_WIDTH 70
#define SCREEN_HEIGHT 30
#define GROUND_Y 16
#define JUMP_HEIGHT_MAX 8
#define SKY_Y 6
#define JUMP_HEIGHT 3
#define Stage4_MAX_HEARTS 5
#define Stage1_MAX_HEARTS 5
#define JUMP_SPEED 0.5
#define GRAVITY 0.5 // 중력


wchar_t map[SCREEN_HEIGHT][SCREEN_WIDTH];

// 메뉴 항목 수
#define MENU_ITEMS 3

// 화면 클리어 매크로
#define cls system("cls")

// 함수 선언
void ScreenInit();
void ScreenFlipping();
void ScreenClear();
void ScreenRelease();
void ScreenPrint(int x, int y, char* string);

void InitBackBuffer(HWND hWnd);
void DrawBackBuffer(HWND hWnd);
void CleanupBackBuffer();

// 외부에서 접근할 수 있는 전역 변수 선언
extern HDC hdc, hBackBufferDC;
extern HBITMAP hBackBufferBitmap;
extern RECT screenRect;
