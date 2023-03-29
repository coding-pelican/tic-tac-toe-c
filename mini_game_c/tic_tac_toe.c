#include <conio.h>
#include <stdio.h>
#include <stdlib.h>

#include <time.h>

#define SWAP(A, B, TYPE) do { TYPE t = (A); (A) = (B); (B) = t; } while (0)

static inline void assert(int condition, const char* message) {
    if (!condition) {
        fprintf(stderr, "%s(%s: %d)\n", message, __FILE__, __LINE__);
        __builtin_trap();
    }
}

static inline void Delay(clock_t waitMS) {
    clock_t endMS = waitMS + clock();
    while (endMS > clock());
}

static void SetCursorVisible(int visible) {
    printf(visible ? "\x1B[?25h" : "\x1B[?25l");
}

static inline void SetCursorPosition(short x, short y) {
    printf("\x1B[%d;%dH", y, x);
}

static inline void DoSystemPause() {
    do {
        printf("\rPress any key to continue . . .");
    } while (getchar() != '\n');
}

static inline void DoSystemCls() {
    printf("\x1B[2J\x1B[H");
}

#define TRUE (1)
#define FALSE (0)

int GetInputKey();

const int IsRunning();
void SetRunning(int toggle);

typedef enum eSceneType {
    SCENE_MENU,
    SCENE_GAME,
    SCENE_EXIT
} SceneType;

typedef enum eMenuStateType {
    MENU_MAIN = 0x1000,
    MENU_SELECTION = 0x2000,
} MenuStateType;

typedef struct _Scene {
    void(*ProcessInput)();
    void(*Update)();
    void(*Draw)();
} Scene;

typedef struct _Menu_SceneData {
    int isDraw;
    MenuStateType currentState;
} Menu_SceneData;
Menu_SceneData menuData = { FALSE, MENU_MAIN };
void Menu_ProcessInput();
void Menu_Update();
void Menu_Draw();
static Scene sceneMenu = { Menu_ProcessInput, Menu_Update, Menu_Draw };

typedef struct _Game_SceneData {
    int turnCount;
} Game_SceneData;
Game_SceneData gameData = { 0, };
void Game_ProcessInput();
void Game_Update();
void Game_Draw();
static Scene sceneGame = { Game_ProcessInput, Game_Update, Game_Draw };

typedef struct _Exit_SceneData {
    int isDraw;
} Exit_SceneData;
Exit_SceneData exitData = { FALSE, };
void Exit_ProcessInput();
void Exit_Update();
void Exit_Draw();
static Scene sceneExit = { Exit_ProcessInput, Exit_Update, Exit_Draw };

static Scene* currentScene = &sceneMenu;
static int inputKey = 0;
static int isRunning = TRUE;



int GetInputKey() {
    if (kbhit()) {
        int key = getch();
        if (key == 0xE0 || key == 0) {
            return getch();
        } else {
            return key;
        }
    }
    return -1;
}

const int IsRunning() {
    return isRunning;
}

void SetRunning(int toggle) {
    isRunning = toggle;
}



void Menu_ProcessInput() {
    inputKey = GetInputKey();
    int inputKeyState = inputKey | menuData.currentState;

    switch (inputKeyState) {
    case ('1' | MENU_MAIN):
        inputKey = 1;
        break;
    case ('2' | MENU_MAIN):
        inputKey = 2;
        break;
    case ('1' | MENU_SELECTION):
        inputKey = 3;
        break;
    case ('2' | MENU_SELECTION):
        inputKey = 4;
        break;
    default:
        break;
    }
}

void Menu_Update() {
    switch (inputKey) {
    case 1:
        menuData.isDraw = FALSE;
        menuData.currentState = MENU_SELECTION;
        return;
    case 2:
        menuData.isDraw = FALSE;
        currentScene = &sceneExit;
        return;
    case 3:
    case 4:
        menuData.isDraw = FALSE;
        currentScene = &sceneGame;
        return;
    default:
        break;
    }

    if (menuData.currentState == MENU_SELECTION && !(inputKey == -1)) {
        menuData.isDraw = FALSE;
        menuData.currentState = MENU_MAIN;
        return;
    }
}

void Menu_Draw() {
    if (menuData.isDraw) {
        return;
    }
    DoSystemCls();
    switch (menuData.currentState) {
    case MENU_MAIN:
        puts("Tic Tac Toe\n");

        puts("1. 새 게임");
        puts("2. 종료");
        break;
    case MENU_SELECTION:
        puts("게임 플레이 방식 선택\n");

        puts("- 1. Play with Another Player");
        puts("- 2. Play with A.I.\n");

        puts("- or Go To Menu");
        break;
    default:
        printf("Error\n");
        break;
    }
    menuData.isDraw = TRUE;
}



void Game_ProcessInput() {
    inputKey = GetInputKey();
}

void Game_Update() {

}

void Game_Draw() {

}



void Exit_ProcessInput() {
    inputKey = GetInputKey();
    switch (inputKey) {
    case 'n':
    case 'N':
        inputKey = 1;
        break;
    default:
        break;
    }
}

void Exit_Update() {
    switch (inputKey) {
    case 1:
        exitData.isDraw = FALSE;
        currentScene = &sceneMenu;
    case -1:
        break;
    default:
        return SetRunning(FALSE);
    }
}

void Exit_Draw() {
    if (exitData.isDraw) {
        return;
    }
    puts("게임을 종료하시겠습니까? Y/n");
    exitData.isDraw = TRUE;
}



int main(int argc, char const* argv[]) {
    SetCursorVisible(FALSE);
    DoSystemCls();

    while (IsRunning()) {
        currentScene->ProcessInput();
        currentScene->Update();
        currentScene->Draw();
    }

    DoSystemCls();
    DoSystemPause();
    SetCursorVisible(TRUE);
    return 0;
}
