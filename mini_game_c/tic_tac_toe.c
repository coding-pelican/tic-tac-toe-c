#include <conio.h>
#include <stdio.h>
#include <time.h>

#define SWAP(TYPE, A, B) do { TYPE t = (A); (A) = (B); (B) = t; } while(0);

/*
display

screen

board
 O | O | X
---+---+---
 O | X | X
---+---+---
 _ | X | O
*/

/*
Tic Tac Toe
    메인 메뉴
    player or ai 선택

    게임 루프
        사이클 카운트
        P1 / P2 턴 진행
        턴 종료
        게임 종료 체크
    승자 표시
*/

static void assert(int condition, const char* message) {
    if (!condition) {
        fprintf(stderr, "%s(%s: %d)\n", message, __FILE__, __LINE__);
        __builtin_trap();
    }
}

void Delay(clock_t waitMS) {
    clock_t endMS = waitMS + clock();
    while (endMS > clock());
}

enum {
    SCREEN_WIDTH = 80,
    SCREEN_HEIGHT = 25,
    SCREEN_NMEMB_COUNT = SCREEN_WIDTH * SCREEN_HEIGHT
} ScreenSize;

typedef enum eMarkedType {
    MARKED_NONE = 0, // '_'
    MARKED_PLAYER_1 = 1, // 'O'
    MARKED_PLAYER_2 = 2 // 'X'
} MarkedType;


static char screen[2][SCREEN_NMEMB_COUNT];

char* previousScreen = screen[0];
char* currentScreen = screen[1];

static MarkedType board[9] = { MARKED_NONE };

static char boardBuffer[] = "\
 $ | $ | $ n\
---+---+---n\
 $ | $ | $ n\
---+---+---n\
 $ | $ | $ n\
";

int GetInputKey() {
    if (kbhit()) {
        int key = getch();
        if (key == 0xE0 || key == 0) {
            return getch();
        } else {
            return key;
        }
    }
    return 0;
}

void PutScreen(short x, short y, char character) {
    currentScreen[y * SCREEN_WIDTH + x] = character;
}

void FillScreen(char character) {
    for (int ptrAccess = 0; ptrAccess < SCREEN_NMEMB_COUNT; ++ptrAccess) {
        currentScreen[ptrAccess] = character;
    }
}

void FillScreenAt(int screenIndex, char character) {
    for (int ptrAccess = 0; ptrAccess < SCREEN_NMEMB_COUNT; ++ptrAccess) {
        screen[screenIndex][ptrAccess] = character;
    }
}

char GetTileCharByMarkedType(MarkedType markedType) {
    static const char* tileCharBy = "_OX";
    return tileCharBy[markedType];
}

void DrawBoard(short x, short y) {
    int ptrScreen = y * SCREEN_WIDTH + x;
    int ptrTile = 0;
    int ptrMarkedTile = 0;
    int curRowLength = 0;

    while (boardBuffer[ptrTile] != '\0') {
        ++curRowLength;
        if (currentScreen[ptrScreen] != boardBuffer[ptrTile]) {
            switch (boardBuffer[ptrTile]) {
            case '$':
                currentScreen[ptrScreen] = GetTileCharByMarkedType(board[ptrMarkedTile++]);
                break;
            case 'n':
                ptrScreen -= curRowLength;
                ptrScreen += SCREEN_WIDTH;
                curRowLength = 0;
                break;
            default:
                currentScreen[ptrScreen] = boardBuffer[ptrTile];
                break;
            }
        }
        ++ptrScreen;
        ++ptrTile;
    }
}

void Draw() {
    printf("\e[1J\e[H");
    DrawBoard(0, 0);

    for (int i = 0; i < SCREEN_NMEMB_COUNT; ++i) {
        printf("%c", currentScreen[i]);
    }

    SWAP(char*, currentScreen, previousScreen);
}

int main(void) {
    system("mode con:cols=80 lines=25");
    system("title Tic Tac Toe");

    FillScreenAt(0, ' ');
    FillScreenAt(1, ' ');

    puts("Copyright 2023. Gyeongtae Kim. All rights reserved.\n");
    Delay(2500);
    puts("Play with...");
    int selectedMenu = 0;
    printf("%c (a) another player\n%c (i) A.I.\n", selectedMenu ? ' ' : '>', selectedMenu ? '>' : ' ');

    Draw();
    Delay(2500);
    Draw();
    Delay(2500);

    return 0;
}
