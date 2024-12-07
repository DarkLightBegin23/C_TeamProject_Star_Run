#include "screen.h"

// 전역 변수 초기화
static int g_nScreenIndex;
static HANDLE g_hScreen[2];

HDC hdc = NULL, hBackBufferDC = NULL;   // 화면 DC와 백 버퍼 DC
HBITMAP hBackBufferBitmap = NULL;       // 백 버퍼 비트맵
RECT screenRect;                        // 화면 영역



void ScreenInit() {
    CONSOLE_CURSOR_INFO cci;

    // 화면 버퍼 2개를 생성
    g_hScreen[0] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,
        0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    g_hScreen[1] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,
        0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

    // 커서 숨기기
    cci.dwSize = 1;
    cci.bVisible = FALSE;
    SetConsoleCursorInfo(g_hScreen[0], &cci);
    SetConsoleCursorInfo(g_hScreen[1], &cci);
}

void ScreenFlipping() {
    SetConsoleActiveScreenBuffer(g_hScreen[g_nScreenIndex]);
    g_nScreenIndex = !g_nScreenIndex;
}

void ScreenClear() {
    COORD Coor = { 0, 0 };
    DWORD dw;
    FillConsoleOutputCharacter(g_hScreen[g_nScreenIndex], ' ', 80 * 25, Coor, &dw);
}

void ScreenRelease() {
    CloseHandle(g_hScreen[0]);
    CloseHandle(g_hScreen[1]);
}

void ScreenPrint(int x, int y, char* string) {
    DWORD dw;
    COORD CursorPosition = { x, y };
    SetConsoleCursorPosition(g_hScreen[g_nScreenIndex], CursorPosition);
    WriteFile(g_hScreen[g_nScreenIndex], string, strlen(string), &dw, NULL);
}

void InitBackBuffer(HWND hWnd) {
    hdc = GetDC(hWnd);
    GetClientRect(hWnd, &screenRect);

    // 백 버퍼용 DC 생성
    hBackBufferDC = CreateCompatibleDC(hdc);

    // 화면 크기에 맞는 비트맵 생성
    hBackBufferBitmap = CreateCompatibleBitmap(hdc, screenRect.right, screenRect.bottom);
    SelectObject(hBackBufferDC, hBackBufferBitmap);

    ReleaseDC(hWnd, hdc);
}

void DrawBackBuffer(HWND hWnd) {
    hdc = GetDC(hWnd);

    // 백 버퍼의 내용을 화면으로 복사
    BitBlt(hdc, 0, 0, screenRect.right, screenRect.bottom, hBackBufferDC, 0, 0, SRCCOPY);

    ReleaseDC(hWnd, hdc);
}

void CleanupBackBuffer() {
    DeleteObject(hBackBufferBitmap);
    DeleteDC(hBackBufferDC);
}
