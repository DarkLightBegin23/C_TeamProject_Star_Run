#include <stdio.h>  // 표준 입출력 함수 사용을 위한 헤더 파일
#include <conio.h>  // _kbhit(), _getch() 사용을 위한 헤더 파일
#include <stdlib.h>  // 난수 생성을 위한 헤더 파일
#include <time.h>  // Sleep 함수 사용
#include <locale.h>  // 특수기호 출력을 위한 헤더 파일
#include <stdbool.h>  // 불리언 값 사용을 위한 헤더 파일
#include <Windows.h>  // win32 api 함수 사용
#include "screen.h" // 더블 버퍼링 기술 적용을 위한 헤더 파일

// 게임 진행에 필요한 매크로 상수들
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

// 2, 3, 5, 6 스테이지에서 사용할 맵 배열
wchar_t map[SCREEN_HEIGHT][SCREEN_WIDTH];

// 메뉴 항목 수
#define MENU_ITEMS 3

// 화면 클리어 매크로
#define cls system("cls")

// 스테이지 관련 함수들
int stage1(void);
int stage2(void);
int stage3(void);
int stage4(void);
int stage5(void);
int stage6(void);
int laststage(void);  // 엔딩 스테이지
int index = 1; // 첫 번째 스테이지부터 시작
int hearts = 5;  // 하트 변수 

typedef struct {
    int x, y;
    int active;
} Bullet;

Bullet bossBullets[2];

bool isJumping = false;
bool check_Collision(float playerX, int playerY);
void gameOver();
void drawMap();
void drawPlayer(int x, int y);
void clearPlayer(int x, int y);
int drawHearts(int heart);
void drawBoss();
void fireBullets();
void moveBullets();
void generateMapAndObstacle(int playerX);
void printQuit(int x, int y);
void gotoxy(int x, int y);
void setCursorPosition(int x, int y);
void scrollMap();
void scrollSky();
void printTree(int levels, int trunkHeight, int width);
void getConsoleSize(int* width, int* height);
void printDecorations();
void gotoxy(int x, int y);
void highlightMenu(int pos);
int storyPrint();
void CursorView(char show);
void printTextWithDelay(const char* text, int delayMs);
void transitionToNextStage(int stageNumber);
void End_tree();
void End_print_people();
void Ending_Print();
void handleCollision(int* hearts);
void checkCollisionAndUpdate(int playerX, int playerY, int* heart);
void checkCollision(int playerX, int playerY, int isJumping);
void Player(int prevX, int prevY, int x, int y);

// 메뉴 항목의 x 좌표 배열
int menuX[MENU_ITEMS] = { 10, 35, 60 };

// 메뉴 항목 문자열 배열
char* menuOptions[MENU_ITEMS] = { "새로  하기", "이어  하기", "게임  종료" };

// 현재 선택된 메뉴 위치를 나타내는 변수
int POS = 0;

void drawMap() {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        gotoxy(0, y);
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            wprintf(L"%lc", map[y][x]);
        }
    }
}
void gameOver() {
    gotoxy(0, SCREEN_HEIGHT + 1);
    wprintf(L"게임 오버! 종료하려면 아무 키나 누르세요...\n");
    exit(0);
}

void drawPlayer(int x, int y) {
    gotoxy(x, y - 3);
    printf(" ★ "); // 플레이어 캐릭터
    gotoxy(x, y - 2);
    printf("/|\\");
    gotoxy(x, y - 1);
    printf("/ \\");
}

void clearPlayer(int x, int y) {
    gotoxy(x, y - 3);
    printf("   "); // 빈 공간으로 초기화
    gotoxy(x, y - 2);
    printf("   ");
    gotoxy(x, y - 1);
    printf("   ");
}

void Player(int prevX, int prevY, int playerX, int playerY) {
    // 이전 플레이어 위치를 지움
    // clearPlayer(prevX, prevY);

    // 새로운 플레이어 위치를 그림
    drawPlayer(playerX, playerY);
}


void printQuit(int x, int y)
{
    gotoxy(x, y);
    printf("x : 게임 종료");
}

void scrollMap() {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH - 1; x++) {
            map[y][x] = map[y][x + 1]; // 맵을 왼쪽으로 한 칸씩 이동
        }

        // 땅 위치 및 장애물 처리
        if (y == GROUND_Y) {
            map[y][SCREEN_WIDTH - 1] = L'▣'; // 땅
        }
        else {
            map[y][SCREEN_WIDTH - 1] = L' '; // 그 외는 공백
        }
    }

    // 장애물 추가
    if (rand() % 15 < 1) {
        map[GROUND_Y - 1][SCREEN_WIDTH - 1] = L'▲'; // 장애물 추가
    }
    else {
        map[GROUND_Y - 1][SCREEN_WIDTH - 1] = L' ';
    }
}

void scrollSky() {
    for (int x = 0; x < SCREEN_WIDTH - 1; x++) {
        map[SKY_Y][x] = map[SKY_Y][x + 1];
        map[SKY_Y][SCREEN_WIDTH - 1] = L'▣';
    }

    if (rand() % 30 < 5) {
        map[SKY_Y + 1][SCREEN_WIDTH - 1] = L'▼'; // SKY_Y + 1에만 추가
    }
    else if (rand() % 40 == 0) {
        map[SKY_Y + 2][SCREEN_WIDTH - 1] = L'▼';
        map[SKY_Y + 1][SCREEN_WIDTH - 1] = L'▣';
    }
    else {
        map[SKY_Y + 1][SCREEN_WIDTH - 1] = L' ';
    }
}
int immuneFrames = 0; // 면역 시간 측정 변수

bool check_Collision(float playerX, int playerY) {
    if (immuneFrames > 0) {
        immuneFrames--; // 면역 시간 감소
        return false; // 면역 상태 중에는 충돌하지 않음
    }

    float collisionMargin = 0.01; // X축 충돌 마진 확대
    for (float i = -collisionMargin; i <= collisionMargin; i += 0.001) { // X축 검사 간격 조정
        for (int j = -1; j <= 0; j++) { // Y축 검사 범위 확장
            int xIndex = (int)(playerX + i); // X좌표 인덱스
            if (xIndex < 0 || xIndex >= SCREEN_WIDTH || playerY + j < 0 || playerY + j >= SCREEN_HEIGHT) continue;  // 배열 초과 방지
                if (playerY + j < SCREEN_HEIGHT &&
                    map[playerY + j][xIndex] == L'▲') {
                    immuneFrames = 100; // 면역 시간
                    return true;
            }
        }
    }

    for (int i = 0; i < 2; i++) {
        if (bossBullets[i].active &&
            fabs(bossBullets[i].x - playerX) < 1.0 && // 보스 총알 충돌 마진 확대
            bossBullets[i].y >= playerY - 3 &&        // Y축 마진 확대
            bossBullets[i].y <= playerY + 1) {
            hearts--;
            bossBullets[i].active = 0;
            immuneFrames = 100;
            return true;
        }
    }


    return false; // 충돌 없음
}

void checkCollisionAndUpdate(int playerX, int playerY, int* heart) {

    if (check_Collision(playerX, playerY)) {
        // wprintf(L"\n충돌 발생!\n");
        handleCollision(heart);  // 충돌 시 체력 감소 및 화면 업데이트
    }
    if (playerY + 1 >= SCREEN_HEIGHT || playerY + 2 >= SCREEN_HEIGHT) {
        // 경계 밖으로 나가는 경우 충돌 없음
        return;
    }
}

void handleCollision(int* heart) {
    cls;  // 화면을 지움

    if (*heart > 0) {
        (*heart)--;  // 체력 감소
        drawHearts(*heart);  // 화면에 즉시 반영
    }
    if (*heart <= 0) {
        // 체력이 0 이하일 때 게임 오버 처리
        gameOver();
    }
}

int drawHearts(int hearts) {
    gotoxy(0, 30); // 하단에 체력 표시
    wprintf(L"체력: ");
    for (int i = 0; i < hearts; i++) {
        if (i < hearts) {
            wprintf(L"♥ ");
        }
        else {
            wprintf(L"♡ ");
        }
    }
    return hearts;
}

void reduceHeart() {
    if (hearts > 0) {
        hearts--; // 체력 감소
    }
    if (hearts <= 0) {
        gameOver(); // 체력이 0일 때 게임 오버 처리
    }
}

void generateMapAndObstacle(int playerX) {
    int blockX = SCREEN_WIDTH - 5; // 장애물 위치 (고정적인 간격으로 배치)

    if (rand() % 50 < 1) {
        if (blockX < SCREEN_WIDTH) {
            map[GROUND_Y - 1][blockX] = L'▲'; // 지상 장애물
        }
    }
    else if (rand() % 50 == 0) {
        if (blockX < SCREEN_WIDTH) {
            map[SKY_Y + 1][blockX] = L'▼'; // 공중 장애물
        }
    }
}

void checkCollision(int playerX, int playerY, int playerHealth) {
    // 장애물 충돌 처리
    if (map[playerY+1][playerX] == L'▲' || map[playerY][playerX] == L'▼') {
        playerHealth--;
        if (playerHealth <= 0) {
            gameOver();
        }
    }
}

int stage1(void) {
    time_t stageStartTime = time(NULL); // 스테이지 시작 시간
    time_t stageDuration = 50;         // 스테이지 지속 시간 (초)
    time_t currentTime;

    SetConsoleCP(65001);           // UTF-8 입력 설정
    SetConsoleOutputCP(65001);     // UTF-8 출력 설정
    setlocale(LC_ALL, "");

    float playerY = GROUND_Y;
    int playerX = 10;
    bool isJumping = false;
    int jumpCount = 0;
    int jumpAllowed = 1;           // 최대 점프 횟수
    int playerHealth = 5;          // 플레이어 체력    // 플레이어 초기 위치
    int prevX = playerX, prevY = playerY; // 이전 위치 저장
    int groundY = playerY; // 지면 높이

    int Width, Height;
    getConsoleSize(&Width, &Height);

    int setX = (Width - 15) / 2;
    int setY = (Height - 15) / 2;

    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            if (y == GROUND_Y) {
                map[y][x] = L'▣';  // 땅
            }
            else if (y == SKY_Y) {
                map[y][x] = L'▣';  // 하늘 기준 라인
            }
            else {
                map[y][x] = L' ';  // 빈 공간
            }
        }
    }
    
    while (1) {
        
        currentTime = time(NULL);

        // 남은 시간 확인
        time_t remainingTime = stageDuration - difftime(currentTime, stageStartTime);
        if (remainingTime <= 0) return 2;

        // 이전 플레이어 위치 저장
        prevX = playerX;
        prevY = playerY;

        // 점프 상태 처리
        if (isJumping) {  // 점프 중일 때
            playerY -= 2;  // 플레이어의 Y 좌표를 위로 이동 (상승)
            if (playerY <= GROUND_Y - JUMP_HEIGHT * jumpCount) {
                // 최대 점프 높이에 도달하면 점프 종료
                isJumping = false;
            }
        }
        else if (playerY < GROUND_Y) {  // 플레이어가 땅 위에 있지 않을 때 (낙하 상태)
            playerY += 0.5;  // 중력 효과로 플레이어를 아래로 이동
            if (playerY > GROUND_Y - 0.1) {  // 땅에 닿으면 플레이어 위치를 고정
                playerY = GROUND_Y;
                jumpCount = 0;  // 점프 횟수 초기화
            }
        }

        // 새로운 위치에 플레이어 그리기
        drawPlayer(playerX, playerY);
        
        
        // 키 입력 처리
        if (_kbhit()) {
            char ch = _getch();
            if (ch == ' ') {
                if (jumpCount < jumpAllowed) {
                    isJumping = true;
                    jumpCount++;
                }
            }
            if (ch == 'x' || ch == 'X') return 0;
            if (ch == 'z' || ch == 'Z') return 2;
        }
        checkCollisionAndUpdate(playerX, (int)playerY, &playerHealth);  // 충돌 체크 및 업데이트

        // 맵과 기타 정보 갱신
        scrollMap();
        scrollSky();
        drawMap();
        drawHearts(playerHealth);

		drawPlayer(playerX, playerY);

        Sleep(15);
    }
}

void setCursorPosition(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void getConsoleSize(int* width, int* height) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    *width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    *height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

void map_print(int c) {
    switch (c) {
    case -1:
        printf("■ ");
        break;
    case -2:
        printf("  ");
        break;
    case -3:
        printf("★ ");
        break;
    case -4:
        printf("▲ ");
        break;
    case -5:
        printf("● ");
        break;
    case -6:
        printf("★ ");
        break;
    case -7:
        printf("↑ ");
        break;
    case -8:
        printf("｜");
        break;
    case -9:
        printf("。");
        break;
    case -10:
        printf("▤ ");
        break;
    case -13:
		printf("⊙ ");
		break;
    }
}

int stage2(void) {
    // system("cls");
    int board[25][11] = {
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -2, -2},
        {-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2},
        {-2, -3, -3, -3, -2, -2, -2, -2, -2, -2, -2}
    };
    int xpos = 4, ypos = 21; // 플레이어 시작 위치
    board[ypos][xpos] = -6; // 플레이어 캐릭터
    int heart = 3;

    int Width, Height;
    getConsoleSize(&Width, &Height);

    int setX = (Width - 9 * 2) / 2;
    int setY = (Height - 25) / 2;

    srand(time(NULL)); // 난수 초기화

    int obstype = rand() % 2;//2개의 장애물 종류 중 랜덤 하나
    int Obstacle[23] = { 0 };
    for (int i = 0; i < 23; i++) Obstacle[i] = EMPTY;

    time_t startTime = time(NULL); // 2초마다 자동으로 1칸 올라가는 코드

    while (1) {
        ScreenClear();
        ScreenFlipping();
        setCursorPosition(setX, setY - 2);
        printf(" <- (x: 종료) ->\n");
        printf("                                                   정상에 올라가라!!!\n");

        for (int y = 0; y < 25; y++) {//맵 작성 코드
            setCursorPosition(setX, setY + y);
            for (int x = 0; x < 11; x++) {
                map_print(board[y][x]);
            }
            printf("\n");
        }

        for (int col = 1; col <= 7; col++) {//장애물 작성 코드
            if (Obstacle[col] != EMPTY) {
                board[Obstacle[col]][col] = -2;
                Obstacle[col]++;
                if (Obstacle[col] <= 21) {
                    if (obstype == 1)
                        board[Obstacle[col]][col] = -4;
                    else
                        board[Obstacle[col]][col] = -5;
                }
                else
                    Obstacle[col] = EMPTY;
            }
        }

        if (rand() % 15 < 1) {//장애물 종류 구분
            int i = rand() % 7 + 1;
            if (Obstacle[i] == EMPTY) {
                Obstacle[i] = 1;
                if (obstype == 1)
                    board[1][i] = -4;//고드름, 돌, 나무
                else
                    board[1][i] = -5;//눈덩이
            }
        }

        if (board[ypos][xpos] == -4 || board[ypos][xpos] == -5) {//장애물 히트 판정
            board[24][heart] = -2;
            heart--;
            if (heart == 0) {
                setCursorPosition(setX, setY + 26);
                printf("      GAME OVER         \n");
                return 0;
            }
        }

        if (board[1][xpos] == -6) {
            setCursorPosition(setX, setY + 26);
            printf("   GAME CLEAR            \n");
            return 3;
        }

        board[ypos][xpos] = -2;

        time_t currentTime = time(NULL);//2초마다 자동으로 1칸 올라가는 코드
        if (difftime(currentTime, startTime) >= 2.0) {
            if (ypos > 1) {
                ypos--;
                startTime = currentTime; // 시간 초기화
            }
        }

        int ch = 0;// 조작키
        if (_kbhit()) {
            ch = _getch();
            if (ch == 'x' || ch == 'X') return 0;
            if (ch == 'z' || ch == 'Z') return 3;
            if (ch == 224) {
                int ch2 = _getch();
                switch (ch2) {
                case LEFT:
                    if (xpos > 1) xpos--;
                    break;
                case RIGHT:
                    if (xpos < 7) xpos++;
                    break;
                }
            }
        }
        board[ypos][xpos] = -6;
        board[ypos][10] = -7;
        Sleep(80); // 속도조절
    }
    return 3;
}

int stage3(void) {
    // system("cls");
    int board[13][30] = {
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1},
        {-1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1},
        {-1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1},
        {-1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1},
        {-1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1},
        {-1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1},
        {-1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1},
        {-1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1},
        {-1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1},
        {-1, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -1},
        {-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2},
        {-2, -3, -3, -3, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2}
    };
    int xpos = 1, ypos = 5; // 플레이어 시작 위치
    board[ypos][xpos] = -6; // 플레이어 캐릭터
    int heart = 3;

    int Width, Height;
    getConsoleSize(&Width, &Height);

    int setX = (Width - 30 * 2) / 2;
    int setY = (Height - 13) / 2;

    srand(time(NULL)); // 난수 초기화

    int Obstacle[10] = { 0 };
    for (int i = 1; i < 10; i++) Obstacle[i] = EMPTY;

    time_t startTime = time(NULL); // 2초마다 자동으로 1칸 올라가는 코드

    while (1) {
        ScreenClear();
        ScreenFlipping();
        setCursorPosition(setX, setY - 2);
        printf("\n");
        printf("                               ↑위   ↓아래  덩굴을 타서 동굴을 빠져나가!!! 덩굴을 놓지마!\n");

        for (int y = 0; y < 13; y++) {//맵 작성 코드
            setCursorPosition(setX, setY + y);
            for (int x = 0; x < 30; x++) {
                map_print(board[y][x]);
            }
            printf("\n");
        }

        for (int y = 1; y < 9; y++) { // 각 행에서 장애물 이동 처리
            for (int x = 1; x < 29; x++) { // 왼쪽에서 오른쪽으로 탐색 (반대로 이동)
                if (board[y][x] == -9) {
                    board[y][x - 1] = -9; // 장애물을 왼쪽으로 이동
                    board[y][x] = -2;    // 현재 위치를 공백으로 변경
                }
            }
        }

        if (rand() % 60 < 2) { // 랜덤 확률로 장애물 생성
            int row = rand() % 8 + 1; // 1번~8번 행 중 무작위로 선택
            if (board[row][28] == -2) { // 오른쪽 끝이 비어 있는 경우에만 생성
                board[row][28] = -9;    // 장애물(-9) 생성
            }
        }

        if (board[ypos][xpos] == -9) {//장애물 히트 판정
            board[12][heart] = -2;
            heart--;
            if (heart == 0) {
                setCursorPosition(setX, setY + 26);
                printf("                                        GAME OVER         \n");
                return 0;
            }
        }
        if (board[ypos][28] == -6) {//클리어 조건
            setCursorPosition(setX, setY + 26);
            printf("                                                              GAME CLEAR            \n");
            return 4;
        }

        board[ypos][xpos] = -2;

        time_t currentTime = time(NULL);//2초마다 자동으로 1칸 올라가는 코드
        if (difftime(currentTime, startTime) >= 1) {
            if (xpos > 0) {
                xpos++;
                startTime = currentTime; // 시간 초기화
            }
        }

        int ch = 0;// 조작키
        if (_kbhit()) {
            ch = _getch();
            if (ch == 'x' || ch == 'X') return 0;
            if (ch == 'z' || ch == 'Z') return 4;
            if (ch == 224) {
                int ch2 = _getch();
                switch (ch2) {
                case UP:
                    if (ypos > 1) ypos--;
                    break;
                case DOWN:
                    if (ypos < 9) ypos++;
                    break;
                }
            }
        }
        for (int i = 1; i < 9; i++) {
            board[i][xpos] = -8;
            if (xpos > 1) board[i][xpos - 1] = -2;
        }
        board[ypos][xpos] = -6;
        if (ypos == 9) {
            for (int y = 0; y < 13; y++) {//맵 작성 코드
                setCursorPosition(setX, setY + y);
                for (int x = 0; x < 30; x++) {
                    map_print(board[y][x]);
                }
                printf("\n");
            }
            printf("                                              GAME OVER     로프에서 떨어짐    \n");
            break;
        }
        Sleep(80); // 속도조절
    }
    return 4;
}

int stage4(void) {
    time_t stageStartTime = time(NULL); // 스테이지 시작 시간
    time_t stageDuration = 30; // 스테이지 지속 시간 (30초)
    time_t currentTime = 0; // 매 루프마다 현재 시간 갱신
    time_t remainingTime = stageDuration; // 남은 시간 초기화

    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    setlocale(LC_ALL, "");

    float playerY = GROUND_Y;
    int playerX = 10;
    int isJumping = 0, jumpCount = 0;
    int jumpAllowed = 2;
    int playerHealth = Stage4_MAX_HEARTS;

    int Width, Height;
    getConsoleSize(&Width, &Height);

    int setX = (Width - 15) / 2;
    int setY = (Height - 15) / 2;

    for (int i = 0; i < 2; i++) {
        bossBullets[i].active = 0;
    }

    while (1) {

        setCursorPosition(setX, setY - 2);
        printf("                        보스 총알과 장애물을 피하자!!!\n");

        currentTime = time(NULL);
        remainingTime = stageDuration - difftime(currentTime, stageStartTime);

        if (remainingTime <= 0) {
            index++;
            return 5;
        }

        // 충돌 체크
        checkCollisionAndUpdate(playerX, (int)playerY, &playerHealth);  // 충돌 체크 및 업데이트

		
        // 2. 키 입력 처리
        if (_kbhit()) {
            char ch = _getch();
            if (ch == 'x' || ch == 'X') return 0;
            if (ch == 'z' || ch == 'Z') return 5;
            if (ch == ' ') {
                if (jumpCount < jumpAllowed) {
                    isJumping = true;
                    jumpCount++;
                }
            }
        }

        // 3. 이전 플레이어 위치 지우기
        clearPlayer(playerX, playerY);
        
		// 4. 점프 모션 처리
        if (isJumping) {  // 점프 중일 때
            playerY -= 2;  // 플레이어의 Y 좌표를 위로 이동 (상승)
            if (playerY <= GROUND_Y - JUMP_HEIGHT * jumpCount) {
                // 최대 점프 높이에 도달하면 점프 종료
                isJumping = false;
            }
        }
        else if (playerY < GROUND_Y) {  // 플레이어가 땅 위에 있지 않을 때 (낙하 상태)
            playerY += 0.5;  // 중력 효과로 플레이어를 아래로 이동
            if (playerY > GROUND_Y - 0.1) {  // 땅에 닿으면 플레이어 위치를 고정
                playerY = GROUND_Y;
                jumpCount = 0;  // 점프 횟수 초기화
            }
        }

        // 5. 게임 환경 업데이트 및 화면 그리기
        generateMapAndObstacle(playerX);
        
        drawPlayer(playerX, (int)playerY);
        drawBoss();
        moveBullets();

        if (rand() % 25 == 0) {
            fireBullets();
        }
        scrollSky();
        scrollMap();
        drawMap();
        drawHearts(playerHealth);  // 현재 체력을 화면에 표시
        Sleep(20);
    }
    return 5;
}

void drawBoss() {
    int bossX = SCREEN_WIDTH - 8;
    int bossY = SKY_Y + 2;

    gotoxy(bossX, bossY);
    printf("          _____  ");
    gotoxy(bossX, bossY + 1);
    printf("         /     \\ ");
    gotoxy(bossX, bossY + 2);
    printf("        |  @ #  |");
    gotoxy(bossX, bossY + 3);
    printf("        |   ^   |");
    gotoxy(bossX, bossY + 4);
    printf("-----\\   \\ --- /");
    gotoxy(bossX, bossY + 5);
    printf("----\\ \\ //     \\\\");
    gotoxy(bossX, bossY + 6);
    printf("     \\_\\|      | )");
    gotoxy(bossX, bossY + 7);
    printf("        \\\\_____/ ");
    gotoxy(bossX, bossY + 8);
    printf("          || ||  ");
    gotoxy(bossX, bossY + 9);
    printf("         //   \\\\ ");
}

void fireBullets() {
    for (int i = 0; i < 2; i++) {
        if (!bossBullets[i].active) {
            bossBullets[i].x = SCREEN_WIDTH - 8;
            bossBullets[i].y = SKY_Y + 4 + (i * 2);
            bossBullets[i].active = 1;
        }
    }
}

void moveBullets() {
    for (int i = 0; i < 2; i++) {
        if (bossBullets[i].active) {
            gotoxy(bossBullets[i].x, bossBullets[i].y);
            printf(" ");
            bossBullets[i].x--;
            if (bossBullets[i].x < 0) {
                bossBullets[i].active = 0;
            }
            else {
                gotoxy(bossBullets[i].x, bossBullets[i].y);
                printf("*");
            }
        }
    }
}


int stage5(void) {
    // system("cls");
    int board[25][33] = {
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -2, -2, -2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -1, -2, -2, -2, -1, -1, -1, -2, -2, -1, -1, -1, -2, -2, -2, -1, -1, -2, -2, -2, -1},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -1, -2, -1, -1, -2, -1, -2, -1, -1, -2, -1, -2, -1, -1, -1, -1, -2, -1, -1, -1, -1},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -1, -2, -1, -1, -2, -1, -2, -1, -1, -2, -1, -2, -1, -1, -1, -1, -2, -1, -1, -1, -1},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -1, -2, -2, -2, -1, -1, -2, -1, -1, -2, -1, -1, -2, -2, -1, -1, -1, -2, -2, -1, -1},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -1, -2, -1, -1, -2, -1, -2, -1, -1, -2, -1, -1, -1, -1, -2, -1, -1, -1, -1, -2, -1},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -1, -2, -1, -1, -2, -1, -2, -1, -1, -2, -1, -1, -1, -1, -2, -1, -1, -1, -1, -2, -1},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -1, -2, -2, -2, -1, -1, -1, -2, -2, -1, -1, -2, -2, -2, -1, -1, -2, -2, -2, -1, -1},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -2, -1, -2, -1, -2, -1, -2, -2, -2, -2, -2, -2, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -1, -1, -1, -1, -1, -1, -2, -2, -2, -2, -2, -2, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2},
        {-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2},
        {-2, -3, -3, -3, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2}
    };
    int xpos = 4, ypos = 21; // 플레이어 시작 위치
    board[ypos][xpos] = -6; // 플레이어 캐릭터
    int heart = 3;

    int Width, Height;
    getConsoleSize(&Width, &Height);

    int setX = (Width - 9 * 2) / 2;
    int setY = (Height - 25) / 2;

    srand(time(NULL)); // 난수 초기화

    int obstype = 1;
    int Obstacle[23] = { 0 };
    for (int i = 0; i < 23; i++) Obstacle[i] = EMPTY;

    time_t startTime = time(NULL); // 2초마다 자동으로 1칸 올라가는 코드

    while (1) {
        ScreenClear();
        ScreenFlipping();
        setCursorPosition(setX, setY - 2);
        printf("\n");
        printf("                                         보스의 공격을 피해 정상에 올라가라!!!\n");

        for (int y = 0; y < 25; y++) {//맵 작성 코드
            setCursorPosition(setX, setY + y);
            for (int x = 0; x < 33; x++) {
                map_print(board[y][x]);
            }
            printf("\n");
        }

        for (int col = 1; col <= 7; col++) {//장애물 작성 코드
            if (Obstacle[col] != EMPTY) {
                board[Obstacle[col]][col] = -2;
                Obstacle[col]++;
                if (Obstacle[col] <= 21) {
                    if (obstype == 1)
                        board[Obstacle[col]][col] = -10;
                    else
                        board[Obstacle[col]][col] = -5;
                }
                else
                    Obstacle[col] = EMPTY;
            }
        }

        if (rand() % 10 < 1) {//장애물 종류 구분
            int i = rand() % 7 + 1;
            if (Obstacle[i] == EMPTY) {
                Obstacle[i] = 1;
                if (obstype == 1)
                    board[1][i] = -10;//고드름, 돌, 나무
                else
                    board[1][i] = -5;//눈덩이
            }
        }

        if (board[ypos][xpos] == -10 || board[ypos][xpos] == -5) {//장애물 히트 판정
            board[24][heart] = -2;
            heart--;
            if (heart == 0) {
                setCursorPosition(setX, setY + 26);
                printf("      GAME OVER         \n");
                return 0;
            }
        }

        if (board[1][xpos] == -6) {
            setCursorPosition(setX, setY + 26);
            printf("   GAME CLEAR            \n");
            return 6;
        }

        board[ypos][xpos] = -2;//지나간 자리 채우기

        time_t currentTime = time(NULL);//2초마다 자동으로 1칸 올라가는 코드
        if (difftime(currentTime, startTime) >= 2.0) {
            if (ypos > 1) {
                ypos--;
                startTime = currentTime; // 시간 초기화
            }
        }

        int ch = 0;// 조작키
        if (_kbhit()) {
            ch = _getch();
            if (ch == 'x' || ch == 'X') return 0;
            if (ch == 'z' || ch == 'Z') return 6;
            if (ch == 224) {
                int ch2 = _getch();
                switch (ch2) {
                case LEFT:
                    if (xpos > 1) xpos--;
                    break;
                case RIGHT:
                    if (xpos < 7) xpos++;
                    break;
                }
            }
        }
        board[ypos][xpos] = -6;
        board[ypos][10] = -7;
        Sleep(60); // 속도조절
    }
    return 6;
}

int stage6(void) {
    // system("cls");
    int board[21][37] = {
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -2, -2, -2, -2, -2, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -2, -1, -1, -1, -1, -1, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -12, -1, -1},
        {-1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2},
        {-1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2},
        {-1, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -1, -2, -2, -2, -2, -2, -2, -2},
        {-2, -11, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2},//
        {-2, -3, -3, -3, -2, -2, -2, -2, -2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -2, -2, -2, -2, -2, -2, -2},
        {-2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -1, -1, -1, -2, -2, -1, -1, -1, -2, -2, -2, -1, -1, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2},
        {-2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -2, -1, -1, -2, -1, -2, -1, -1, -2, -1, -2, -1, -1, -1, -1, -2, -1, -1, -1, -1, -2, -2, -2, -2, -2, -2, -2},
        {-2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -2, -1, -1, -2, -1, -2, -1, -1, -2, -1, -2, -1, -1, -1, -1, -2, -1, -1, -1, -1, -2, -2, -2, -2, -2, -2, -2},
        {-2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -1, -1, -2, -1, -1, -2, -1, -1, -2, -2, -1, -1, -1, -2, -2, -1, -1, -2, -2, -2, -2, -2, -2, -2},
        {-2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -2, -1, -1, -2, -1, -2, -1, -1, -2, -1, -1, -1, -1, -2, -1, -1, -1, -1, -2, -1, -2, -2, -2, -2, -2, -2, -2},
        {-2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -2, -1, -1, -2, -1, -2, -1, -1, -2, -1, -1, -1, -1, -2, -1, -1, -1, -1, -2, -1, -2, -2, -2, -2, -2, -2, -2},
        {-2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -1, -1, -1, -2, -2, -1, -1, -2, -2, -2, -1, -1, -2, -2, -2, -1, -1, -2, -2, -2, -2, -2, -2, -2},
        {-2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -2, -2, -2, -2, -2, -2, -2}
    };
    int xpos = 1, ypos = 5; // 플레이어 시작 위치
    board[ypos][xpos] = -6; // 플레이어 캐릭터
    int heart = 3;

    int Width, Height;
    getConsoleSize(&Width, &Height);

    int setX = (Width - 30 * 2) / 2;
    int setY = (Height - 13) / 2;

    srand(time(NULL)); // 난수 초기화

    int Obstacle[10] = { 0 };
    for (int i = 1; i < 10; i++) Obstacle[i] = EMPTY;

    time_t startTime = time(NULL); // 2초마다 자동으로 1칸 올라가는 코드

    while (1) {
        ScreenClear();
        ScreenFlipping();
        setCursorPosition(setX, setY - 2);
        printf("\n");
        printf("                               ↑위   ↓아래  보스가 쏘는 샷건을 피해서 탈출해!!!\n");

        for (int y = 0; y < 21; y++) {//맵 작성 코드
            setCursorPosition(setX, setY + y);
            for (int x = 0; x < 37; x++) {
                map_print(board[y][x]);
            }
            printf("\n");
        }

        for (int y = 1; y < 9; y++) { // 각 행에서 장애물 이동 처리
            for (int x = 1; x < 29; x++) { // 왼쪽에서 오른쪽으로 탐색 (반대로 이동)
                if (board[y][x] == -13) {
                    board[y][x - 1] = -13; // 장애물을 왼쪽으로 이동
                    board[y][x] = -2;    // 현재 위치를 공백으로 변경
                }
            }
        }

        if (rand() % 20 < 1) { // 랜덤 확률로 장애물 생성
            int row = rand() % 8 + 1; // 1번~8번 행 중 무작위로 선택
            if (board[row][28] == -2) { // 오른쪽 끝이 비어 있는 경우에만 생성
                board[row][28] = -13;    // 장애물(-13) 생성
            }
        }

        if (board[ypos][xpos] == -13) {//장애물 히트 판정
            board[12][heart] = -2;
            heart--;
            if (heart == 0) {
                setCursorPosition(setX, setY + 26);
                printf("                                        GAME OVER         \n");
                break;
            }
        }
        if (board[ypos][28] == -6) {//클리어 조건
            setCursorPosition(setX, setY + 26);
            printf("                                                              GAME CLEAR            \n");
            return 7;
        }

        board[ypos][xpos] = -2;

        time_t currentTime = time(NULL);//2초마다 자동으로 1칸 올라가는 코드
        if (difftime(currentTime, startTime) >= 1) {
            if (xpos > 0) {
                xpos++;
                startTime = currentTime; // 시간 초기화
            }
        }

        int ch = 0;// 조작키
        if (_kbhit()) {
            ch = _getch();
            if (ch == 'x' || ch == 'X') return 0;
            if (ch == 'z' || ch == 'Z') return 7;
            if (ch == 224) {
                int ch2 = _getch();
                switch (ch2) {
                case UP:
                    if (ypos > 1) ypos--;
                    break;
                case DOWN:
                    if (ypos < 9) ypos++;
                    break;
                }
            }
        }
        for (int i = 1; i < 9; i++) {
            board[i][xpos] = -8;
            if (xpos > 1) board[i][xpos - 1] = -2;
        }
        board[ypos][xpos] = -6;
        if (ypos == 9) {
            for (int y = 0; y < 13; y++) {//맵 작성 코드
                setCursorPosition(setX, setY + y);
                for (int x = 0; x < 30; x++) {
                    map_print(board[y][x]);
                }
                printf("\n");
            }
            printf("                                              GAME OVER     로프에서 떨어짐    \n");
            return 0;
        }
        Sleep(80); // 속도조절
    }
    return 7;
}

int laststage(void) {
    int board[11][30] = {
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1},
        {-1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1},
        {-1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1},
        {-1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1},
        {-1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1},
        {-1, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1},
        {-1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -10, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1},
        {-1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -10, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1},
        {-1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -10, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
    };
    int xpos = 1, ypos = 5; // 플레이어 시작 위치
    board[ypos][xpos] = -6; // 플레이어 캐릭터

    int Width, Height;
    getConsoleSize(&Width, &Height);

    int setX = (Width - 30 * 2) / 2;
    int setY = (Height - 11) / 2;
    int count = 0;

    time_t startTime = time(NULL); // 2초마다 자동으로 1칸 올라가는 코드

    while (1) {
        ScreenClear();
        ScreenFlipping();
        setCursorPosition(setX, setY - 2);
        printf("\n");
        printf("                                                      ▲ 산정상 ▲\n");

        for (int y = 0; y < 11; y++) {//맵 작성 코드
            setCursorPosition(setX, setY + y);
            for (int x = 0; x < 30; x++) {
                map_print(board[y][x]);
            }
            printf("\n");
        }

        if (board[9][15] == -6) {//클리어 조건
            setCursorPosition(setX, setY + 26);
            return 8;
        }

        board[ypos][xpos] = -2;

        time_t currentTime = time(NULL);//2초마다 자동으로 1칸 올라가는 코드
        if (difftime(currentTime, startTime) >= 0.5) {
            if (xpos == 15) {
                ypos++;
                startTime = currentTime;
            }
            else {
                if (xpos > 0) {
                    xpos++;
                    startTime = currentTime; // 시간 초기화
                }
            }
        }

        int ch = 0;// 조작키
        if (_kbhit()) {
            ch = _getch();
            if (ch == 'x' || ch == 'X') return 0;
            if (ch == 'z' || ch == 'Z') return 8;
        }
        board[ypos][xpos] = -6;
        Sleep(200); // 속도조절
    }
    return 8;
}

void End_tree() {
    int n;
    printf("      ★      \n");
    for (int i = 0; i < 2; i++) { // 나뭇잎의 단을 반복 (총 2단)
        n = 2 + 4 * i; // 각 단의 첫 번째 행에 출력될 '*'의 개수 계산
        for (int j = 0; j < 4; j++) { // 각 단의 행을 반복 (단당 4행)
            for (int k = 0; k < 15; k++) { // 한 행의 열을 반복 (열 개수는 15)
                if (k < (15 - (n + 2 * j)) / 2 || k >= (15 + (n + 2 * j)) / 2)
                    printf(" "); // '*' 외의 영역에 공백 출력
                else
                    printf("*"); // '*'을 출력
            }
            printf("\n"); // 한 행이 끝나면 줄바꿈
        }
    }

    // 나무 줄기 출력
    printf("     IIII      \n"); // 첫 번째 줄기 출력
    printf("     IIII      \n"); // 두 번째 줄기 출력
}

void End_print_people() {
    printf(" * \\O/ \\O/ \\0/ * \\0/ * \\O/ * \\O/ *  \n");

    printf("    |   |   |  *  |     | *   | \n");
    printf("   / \\ / \\ / \\ * / \\   / \\ * / \\ ");
    printf("                   ====!,====            \n");
    printf("      *       *         *    *      *   ");
    printf("            ====-,!===            \n");
    printf("    *         ___   ___   ___   ___   ________  *");
    printf("   ===!--,*==            \n");
    printf("         *   /   \\ /   \\ /   \\ /   \\ /       \\  ");
    printf("    ===,,,,===            \n");
    printf("            /   * \\  *  \\   * \\     \\     *   \\ * ");
    printf("  ==*    *==            \n");
    printf("   *       /_______\\_______\\_________\\_________\\");
    printf("    ==~   -*==            \n");
    printf("          *|  .-.  ||  .-.  ||  .-.  ||  .-.  | ");
    printf("    ==;.. ;===            \n");
    printf("           |  | |  ||  | |  ||* | |  ||  | |  | ");
    printf("    ==*,;*====            \n");
    printf("    *      |__|_|__||__|_|__||__|_|__||__|_|__|\n");
}
// 콘솔 텍스트 색상 설정 함수
void SetColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// 콘솔 커서 가시성 설정 함수
void CursorView(char show) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO ConsoleCursor;
    ConsoleCursor.bVisible = show;  // 0: 숨김, 1: 표시
    ConsoleCursor.dwSize = 1;       // 커서 크기
    SetConsoleCursorInfo(hConsole, &ConsoleCursor);
}

// 콘솔에서 특정 위치로 커서 이동
void gotoxy(int x, int y) {

    COORD Pos = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}

// 초기 화면 출력 함수
void screenPrt() {
    // 콘솔 창 크기 및 제목 설정
    system("title Star Run");

    // 상단 테두리 출력
    puts("■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■");

    // 화면 내용 출력
    for (int i = 0; i < 7; i++) {
        if (i == 1) {
            // 게임 제목 출력
            printf("■                                ★★★  Star Run  ★★★                                     ■\n");
        }
        else {
            // 빈 줄 출력
            printf("■                                                                                       ■\n");
        }
    }

    // 하단 테두리 출력
    puts("■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■");

    // 하단 테두리 출력
    puts("■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■");

    printf("                                          ★                                                       \n");
    printf("                                          **                                                                \n");
    printf("                                         ****                                                               \n");
    printf("               \\0/                      ******                                                                 \n");
    printf("                |                      ********                                                                  \n");
    printf("               / \\                    **********                                                                   \n");
    printf("                                     ************                                                                     \n");
    printf("                                    **************                \\0/           \n");
    printf("                                   ****************                |                                                    \n");
    printf("                                  ******************              / \\                                             \n");
    printf("                  \\0/            ********************                                                            \n");
    printf("                   |            **********************                                                            \n");
    printf("                  / \\          ************************                                                       \n");
    printf("                                  ┌ ┐    IIII  ┌ ┐                                                           \n");
    printf("                                  ┕ ┙    IIII  ┕ ┙                                                          \n");
}


// 메뉴 선택을 처리하는 함수
void CursorMove() {

    highlightMenu(POS);

    while (1) {
        if (_kbhit()) {  // 키보드 입력이 있는지 확인
            char key = _getch();  // 입력된 키 가져오기

            if (key == '\r') {  // 엔터키 확인 ('\r' = 엔터)
                if (POS == 0) {
                    break;
                }
            }
            else if (key == 0 || key == -32) {  // 방향키 처리
                // 방향키는 2바이트로 이루어져 있으며, 첫 바이트는 0 또는 -32
                key = _getch();  // 실제 방향키 코드 가져오기
                if (key == 77) {  // 오른쪽 방향키
                    POS = (POS == MENU_ITEMS - 1) ? 0 : POS + 1;
                }
                else if (key == 75) {  // 왼쪽 방향키
                    POS = (POS == 0) ? MENU_ITEMS - 1 : POS - 1;
                }
            }

            // 선택된 메뉴 강조 표시
            highlightMenu(POS);
        }

        // 입력 간 간격 (딜레이) 설정
        Sleep(150);
    }

    ScreenClear();
    ScreenFlipping();

    // 화면 초기화
    if (POS == 0) {
        // 새로하기 메뉴 선택 시
        storyPrint();
    }
    else if (POS == 1) {
        // 이어하기 메뉴 선택 시
        printf("\n");
        puts("구현 준비중");
        exit(0);
    }
    else if (POS == 2) {
        // 게임 종료 메뉴 선택 시

        printf("\n");
        puts("아무 키나 눌러주시면 게임이 종료됩니다.");
        exit(0);
    }
}




// 선택한 메뉴 강조 표시
void highlightMenu(int pos) {
    for (int i = 0; i < MENU_ITEMS; i++) {
        gotoxy(menuX[i], 4);  // 메뉴 항목 x 좌표로 이동
        if (i == pos) {
            SetColor(11);  // 현재 선택된 메뉴는 밝은 파란색으로 표시
        }
        else {
            SetColor(15);  // 선택되지 않은 메뉴는 기본 흰색으로 표시
        }
        printf("%s", menuOptions[i]);  // 메뉴 항목 출력
    }
}
int storyPrint()
{
    /*
    cls;
    return 1;
    
    }
    */
    cls;

    int boxWidth = 100; // 줄 박스의 길이

    printTree(3, 2, 15);    // 나무 출력
    printDecorations(); // 장식 출력

    printf("\n");

    // 상단 박스 출력
    for (int i = 0; i < boxWidth; i++) {
        printf("=");
    }
    printf("\n");

    char* story = "2024/12/25 스타마을\n"
        "크리스마스에 마을 중간에 있는 트리에 아이들이 나와있습니다.\n"
        "한아이가 말했습니다. \"저기에 별이 없어! 이러면 산타가 오지 않을거야ㅠㅠ \" \n"
        "우리는 트리 위에 별을 달기 위해 모험을 시작하게 되었습니다.\n"
        "게임 로딩중...\n";


    printTextWithDelay(story, 20);

    // 하단 박스 출력
    for (int i = 0; i < boxWidth; i++) {
        printf("=");
    }
    printf("\n");
}
// 나무를 출력하는 함수
void printTree(int levels, int trunkHeight, int width) {
    int baseWidth, levelWidth;

    // 나뭇잎 출력
    for (int i = 0; i < levels; i++) {
        baseWidth = 2 + 4 * i; // 각 단의 초기 너비
        for (int j = 0; j < 3; j++) { // 각 단의 높이는 4줄
            levelWidth = baseWidth + 2 * j; // 현재 줄의 별 개수
            for (int k = 0; k < width; k++) {
                // 나무 중심을 기준으로 별 위치 출력
                if (k < (width - levelWidth) / 2 || k >= (width + levelWidth) / 2) {
                    printf(" "); // 공백
                }
                else {
                    printf("*"); // 별
                }
            }
            printf("\n");
        }
    }

    // 나무 줄기 출력
    for (int i = 0; i < trunkHeight; i++) {
        for (int j = 0; j < width; j++) {
            if (j < width / 2 - 2 || j > width / 2 + 1) {
                printf(" "); // 공백
            }
            else {
                printf("I"); // 나무 줄기
            }
        }
        printf("\n");
    }
}

// 장식 출력
void printDecorations() {
    printf("\n"); // 줄바꿈

    char* decorations[] = {
        " *O  *  O      *  O     O        *       *  ",
        " /|\\ * /|\\   *   /|\\   /|\\       *       * ",
        " / \\   / \\  *    / \\   / \\ *         *    ",
        "   O     O         O     O       *         ",
        "  /|\\   /|\\   *   /|\\   /|\\   *            ",
        "  / \\   / \\       / \\ * / \\      *     *   ",
        "    *         ___   ___   ___   ___   ___     *",
        "         *   /   \\ /   \\ /   \\ /   \\ /   \\  ",
        "            /     \\     \\   * \\     \\     \\ *",
        "   *       /_______\\_______\\_______\\_______\\ ",
        "          *|  .-.  ||  .-.  ||  .-.  ||  .-.  |",
        "           |  | |  ||  | |  ||* | |  ||  | |  |",
        "           |__|_|__||__|_|__||__|_|__||__|_|__|",
    };

    // 각 줄을 출력
    for (int i = 0; i < sizeof(decorations) / sizeof(decorations[0]); i++) {
        printf(decorations[i]); // 지연 출력
        printf("\n");
    }
}

// 텍스트를 지연 출력하는 함수
void printTextWithDelay(const char* text, int delayMs) {
    for (int i = 0; text[i] != '\0'; i++) {
        printf("%c", text[i]); // 한 글자 출력
        Sleep(delayMs);        // 지연
    }
}

// 전환 연출 함수

void transitionToNextStage(int stageNumber) {
    if (stageNumber == 7) {
        system("cls");
        printf("\n\n\n");
        printf("========== ENDING ==========\n");
        Sleep(2000);
        system("cls");
    }
    else {
        system("cls");
        printf("\n\n\n");
        printf("========== 스테이지 %d 시작 ==========\n", stageNumber);
        Sleep(2000);
        system("cls");
    }
}

void Ending_Print()
{

    int boxWidth = 100; // 줄 박스의 길이

    printf("\n");

    // 상단 박스 출력
    for (int i = 0; i < boxWidth; i++) {
        printf("=");
    }
    printf("\n");

    char* End_story = "이렇게 나무 위에 별이 맞춰지면서 이야기를 끝마치게 됩니다.\n"
        "우리를 위협했던 악당도 크리스마스 트리를 보고 감동받았네요!\n"
        "이렇게 크리스마스는 저물어갑니다. . . \n"
        "메리 크리스마스!!\n";

    printTextWithDelay(End_story, 20);

    // 하단 박스 출력
    for (int i = 0; i < boxWidth; i++) {
        printf("=");
    }
    printf("\n");


}

int main(void) {

    // 콘솔 초기 설정
    CursorView(0);
    screenPrt();
    system("COLOR 0F");
    CursorMove();

    time_t stageStartTime = 0;
    time_t stageTime = time(NULL); // 시작 시간 기록
    time_t stageDuration = 60; // 스테이지 지속 시간 (초)
    time_t currentTime = time(NULL);
    time_t remainingTime = stageDuration - (currentTime - stageTime); // 남은 시간 계산

    while (1) { // 게임 루프

        if (index == 0) { // 게임 종료
            system("cls");
            printf("게임 오버\n");
            break;
        }
        // 스테이지 전환 연출
                // 현재 스테이지 실행
        if (index == 1) {
            transitionToNextStage(index);
            index = stage1();
        }
        else if (index == 2) {
            transitionToNextStage(index);
            index = stage2();
        }
        else if (index == 3) {
            transitionToNextStage(index);
            index = stage3();
        }
        else if (index == 4) {
            transitionToNextStage(index);
            index = stage4();
        }
        else if (index == 5) {
            transitionToNextStage(index);
            index = stage5();
        }
        else if (index == 6) {
            transitionToNextStage(index);
            index = stage6();
        }
        else if (index == 7) {
            transitionToNextStage(index);
            index = laststage();
        }
        else if (index == 8) {
            system("cls");
            End_tree();
            End_print_people();
            Ending_Print();

            break;
        }
        else {
            printf("디버그: 알 수 없는 index 값: %d\n", index);
            break;
        }
        // 스테이지 지속 시간 체크
        if (remainingTime <= 0) {
            index++; // 다음 스테이지로 전환
            stageStartTime = currentTime; // 새로운 스테이지 시작 시간 기록
        }

        if (index < 1 || index > 8) {
            printf("게임 강제 종료\n");
            break;
        }

        Sleep(1000); // 루프를 1초 대기 (1초 단위로 업데이트)
    }

    return 0; // 프로그램 정상 종료
}