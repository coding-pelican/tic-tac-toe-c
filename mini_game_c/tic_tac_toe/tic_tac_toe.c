// #include <conio.h>
// #include <stdio.h>
// #include <time.h>

// #define SWAP(TYPE, A, B) do { TYPE t = (A); (A) = (B); (B) = t; } while(0);

// /*

// Menu
// Game

// display

// screen

// board
//  O | O | X
// ---+---+---
//  O | X | X
// ---+---+---
//  _ | X | O
// */

// /*
// Tic Tac Toe
//     메인 메뉴
//     player or ai 선택

//     게임 루프
//         사이클 카운트
//         P1 / P2 턴 진행
//         턴 종료
//         게임 종료 체크
//     승자 표시
// */

// static void assert(int condition, const char* message) {
//     if (!condition) {
//         fprintf(stderr, "%s(%s: %d)\n", message, __FILE__, __LINE__);
//         __builtin_trap();
//     }
// }

// void Delay(clock_t waitMS) {
//     clock_t endMS = waitMS + clock();
//     while (endMS > clock());
// }

// enum {
//     SCREEN_WIDTH = 80,
//     SCREEN_HEIGHT = 25,
//     SCREEN_NMEMB_COUNT = SCREEN_WIDTH * SCREEN_HEIGHT
// } ScreenSize;

// typedef enum eMarkedType {
//     MARKED_NONE = 0, // '_'
//     MARKED_PLAYER_1 = 1, // 'O'
//     MARKED_PLAYER_2 = 2 // 'X'
// } MarkedType;

// static char screen[2][SCREEN_NMEMB_COUNT];

// char* previousScreen = screen[0];
// char* currentScreen = screen[1];

// static MarkedType board[9] = { MARKED_NONE };

// static char boardBuffer[] = "\
//  $ | $ | $ n\
// ---+---+---n\
//  $ | $ | $ n\
// ---+---+---n\
//  $ | $ | $ n\
// ";

// int GetInputKey() {
//     if (kbhit()) {
//         int key = getch();
//         if (key == 0xE0 || key == 0) {
//             return getch();
//         } else {
//             return key;
//         }
//     }
//     return 0;
// }

// void PutScreen(short x, short y, char character) {
//     currentScreen[y * SCREEN_WIDTH + x] = character;
// }

// void FillScreen(char character) {
//     for (int ptrAccess = 0; ptrAccess < SCREEN_NMEMB_COUNT; ++ptrAccess) {
//         currentScreen[ptrAccess] = character;
//     }
// }

// void FillScreenAt(int screenIndex, char character) {
//     for (int ptrAccess = 0; ptrAccess < SCREEN_NMEMB_COUNT; ++ptrAccess) {
//         screen[screenIndex][ptrAccess] = character;
//     }
// }

// char GetTileCharByMarkedType(MarkedType markedType) {
//     static const char* tileCharBy = "_OX";
//     return tileCharBy[markedType];
// }

// void DrawBoard(short x, short y) {
//     int ptrScreen = y * SCREEN_WIDTH + x;
//     int ptrTile = 0;
//     int ptrMarkedTile = 0;
//     int curRowLength = 0;

//     while (boardBuffer[ptrTile] != '\0') {
//         ++curRowLength;
//         if (currentScreen[ptrScreen] != boardBuffer[ptrTile]) {
//             switch (boardBuffer[ptrTile]) {
//             case '$':
//                 currentScreen[ptrScreen] = GetTileCharByMarkedType(board[ptrMarkedTile++]);
//                 break;
//             case 'n':
//                 ptrScreen -= curRowLength;
//                 ptrScreen += SCREEN_WIDTH;
//                 curRowLength = 0;
//                 break;
//             default:
//                 currentScreen[ptrScreen] = boardBuffer[ptrTile];
//                 break;
//             }
//         }
//         ++ptrScreen;
//         ++ptrTile;
//     }
// }

// void Draw() {
//     printf("\e[1J\e[H");
//     DrawBoard(0, 0);

//     for (int i = 0; i < SCREEN_NMEMB_COUNT; ++i) {
//         printf("%c", currentScreen[i]);
//     }

//     SWAP(char*, currentScreen, previousScreen);
// }

// int main(void) {
//     system("mode con:cols=80 lines=25");
//     system("title Tic Tac Toe");

//     FillScreenAt(0, ' ');
//     FillScreenAt(1, ' ');

//     puts("Copyright 2023. Gyeongtae Kim. All rights reserved.\n");
//     Delay(2500);
//     puts("Play with...");
//     int selectedMenu = 0;
//     printf("%c (a) another player\n%c (i) A.I.\n", selectedMenu ? ' ' : '>', selectedMenu ? '>' : ' ');

//     Draw();
//     Delay(2500);
//     Draw();
//     Delay(2500);

//     return 0;
// }

// typedef enum eBool {
//     FALSE = 0,
//     TRUE = 1
// } Bool;

// typedef struct _Scene Scene;

// struct _Scene {
//     void(*Initialize)();
//     void(*ProcessInput)();
//     void(*Update)();
//     void(*Draw)();
// };

// typedef struct _SceneMenu {
//     const char** menuStrings;
//     int selectMenu;
// } SceneMenu;

// typedef struct _SceneGame {
//     const char** gameStrings;
// } SceneGame;

// void SceneMenu_Initialize() { }
// void SceneMenu_ProcessInput() { }
// void SceneMenu_Update() { }
// void SceneMenu_Draw() { }
// void SceneMenu_Finalize() { }

// void SceneGame_Initialize() { }
// void SceneGame_ProcessInput() { }
// void SceneGame_Update() { }
// void SceneGame_Draw() { }
// void SceneGame_Finalize() { }

// enum eScene {
//     SCENE_MENU = 0,
//     SCENE_GAME = 1
// };

// int main(int argc, char const* argv[]) {
//     Scene sceneMenu = { SceneMenu_Initialize, SceneMenu_ProcessInput, SceneMenu_Update, SceneMenu_Draw };
//     Scene sceneGame = { SceneGame_Initialize, SceneGame_ProcessInput, SceneGame_Update, SceneGame_Draw };

//     Scene scenes[2];
//     scenes[SCENE_MENU] = sceneMenu;
//     sceneMenu.Initialize();
//     scenes[SCENE_GAME] = sceneGame;
//     sceneGame.Initialize();

//     Bool isRunning = TRUE;

//     int currentScene = SCENE_MENU;

//     system("mode con:cols=80 lines=25");
//     system("title Tic Tac Toe");

//     while (isRunning) {
//         scenes[currentScene].ProcessInput();
//         scenes[currentScene].Update();
//         scenes[currentScene].Draw();
//     }

//     return 0;
// }

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

typedef enum eBool {
    FALSE = 0,
    TRUE = 1
} Bool;

typedef enum eSceneType {
    SCENE_INTRO = 0,
    SCENE_MENU = 1,
    SCENE_GAME = 2
} SceneType;

typedef void* PrivateType;

typedef struct _PrivateGame {
    char board[9];
} PrivateGame;

typedef struct _Scene Scene;
struct _Scene {
    SceneType type;
    Bool isStaying;
    PrivateType private;

    SceneType(*GetSceneType)(Scene* this);
    void (*ProcessInput)(Scene* this);
    void (*Update)(Scene* this);
    void (*Render)(Scene* this);
};

inline SceneType Scene_GetSceneType(Scene* this) {
    return this->type;
}

void Scene_ProcessInput(Scene* this) {
    this->ProcessInput(this);
}

void Scene_Update(Scene* this) {
    this->Update(this);
}

void Scene_Render(Scene* this) {
    this->Render(this);
}

void SceneIntro_ProcessInput(Scene* this) {

}

void SceneIntro_Update(Scene* this) {

}

void SceneIntro_Render(Scene* this) {

}

void SceneIntro_Initialize(Scene* this) {
    this->ProcessInput = SceneIntro_ProcessInput;
    this->Update = SceneIntro_Update;
    this->Render = SceneIntro_Render;
}

void SceneMenu_ProcessInput(Scene* this) {

}

void SceneMenu_Update(Scene* this) {

}

void SceneMenu_Render(Scene* this) {

}

void SceneMenu_Initialize(Scene* this) {
    this->ProcessInput = SceneMenu_ProcessInput;
    this->Update = SceneMenu_Update;
    this->Render = SceneMenu_Render;
}

void SceneGame_ProcessInput(Scene* this) {

}

void SceneGame_Update(Scene* this) {

}

void SceneGame_Render(Scene* this) {

}

void SceneGame_Initialize(Scene* this) {
    this->ProcessInput = SceneGame_ProcessInput;
    this->Update = SceneGame_Update;
    this->Render = SceneGame_Render;
}

Scene* NewScene(SceneType type) {
    Scene* scene = (Scene*)malloc(sizeof(Scene));

    scene->type = type;
    scene->isStaying = FALSE;

    switch (type) {
    case SCENE_INTRO:
        SceneIntro_Initialize(scene);
    case SCENE_MENU:
        SceneMenu_Initialize(scene);
    case SCENE_GAME:
        SceneGame_Initialize(scene);
    default:
        assert(FALSE, "Error: Undefined Scene Type");
    }

    return scene;
}

typedef struct _Game {
    Bool isRunning;
    Scene* scenes[3];
    Scene* curScene;
} Game;


inline void Game_AddScene(Game* game, SceneType type) {
    game->scenes[type] = NewScene(type);
}

inline Scene* Game_GetSceneByType(Game* game, SceneType type) {
    return game->scenes[type];
}

inline void Game_SetSceneByType(Game* game, SceneType type) {
    game->curScene = Game_GetSceneByType(game, type);
    game->curScene->isStaying = TRUE;
}

inline void Game_ChangeSceneByType(Game* game, SceneType type) {
    game->curScene->isStaying = FALSE;
    Game_SetSceneByType(game, type);
}

void Game_Initialize(Game* game) {
    game->isRunning = TRUE;

    Game_AddScene(game, SCENE_INTRO);
    Game_AddScene(game, SCENE_MENU);
    Game_AddScene(game, SCENE_GAME);

    Game_SetSceneByType(game, SCENE_GAME);

    system("mode con:cols=80 lines=25");
    system("title Tic Tac Toe");

    DoSystemCls();
}

const Bool Game_IsRunning(const Game* game) {
    return game->isRunning;
}

void Game_SetRunning(Game* game, const Bool value) {
    game->isRunning = value;
}

void Game_ProcessInput(Game* game) {
    Scene_ProcessInput(game->curScene);
}

void Game_Update(Game* game) {
    Scene_Update(game->curScene);
}

void Game_Render(Game* game) {
    Scene_Render(game->curScene);
}

void Game_Loop(Game* game) {
    while (Game_IsRunning(game)) {
        Game_ProcessInput(game);
        Game_Update(game);
        Game_Render(game);
    }
}

void Game_Finalize(Game* game) {

}

int main(int argc, char const* argv[]) {
    Game game;
    Game_Initialize(&game);
    Game_Loop(&game);
    Game_Finalize(&game);
    return 0;
}
