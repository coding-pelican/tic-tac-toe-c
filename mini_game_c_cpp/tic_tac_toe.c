#include <conio.h>
#include <stdio.h>
#include <stdlib.h>

#include <time.h>

#define SWAP(A, B, TYPE) do { TYPE t = (A); (A) = (B); (B) = t; } while (0)

#define TRUE (1)
#define FALSE (0)

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



int GetInputKey();

const int IsRunning();

void SetRunning(int toggle);



typedef enum eSceneType {
    SCENE_NONE = 0,
    SCENE_MENU,
    SCENE_GAME,
    SCENE_EXIT
} SceneType;

typedef enum eMenuStateType {
    MENU_MAIN = 0x1000,
    MENU_SELECTION = 0x2000,
} MenuStateType;

typedef enum ePlayerType {
    PLAYER_NONE = 0,
    PLAYER_AI,
    PLAYER_HUMAN
} PlayerType;

typedef enum eBoardTile {
    TILE_PLAYER_EMPTY,
    TILE_PLAYER_ONE,
    TILE_PLAYER_TWO
} BoardTile;

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
    int isDraw;
    int isEnd;
    int toggleTileHint;
    int turnCount;
    BoardTile currentPlayer;
    PlayerType players[2];
    BoardTile board[9];
} Game_SceneData;
Game_SceneData gameData = {
    FALSE,
    FALSE,
    FALSE,
    0,
    TILE_PLAYER_ONE,
    PLAYER_NONE, PLAYER_NONE,
    TILE_PLAYER_EMPTY, TILE_PLAYER_EMPTY, TILE_PLAYER_EMPTY, TILE_PLAYER_EMPTY, TILE_PLAYER_EMPTY, TILE_PLAYER_EMPTY,TILE_PLAYER_EMPTY, TILE_PLAYER_EMPTY, TILE_PLAYER_EMPTY
};
void Game_Initialize(PlayerType player2);
void Game_ProcessInput();
void Game_Update();
void Game_Draw();
void Game_Finalize();
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

    switch (inputKey) {
    case ('1'):
        inputKey = 1;
        break;
    case ('2'):
        inputKey = 2;
        break;
    default:
        break;
    }
}

void Menu_Update() {
    switch (menuData.currentState) {
    case MENU_MAIN:
        switch (inputKey) {
        case 1:
            menuData.isDraw = FALSE;
            menuData.currentState = MENU_SELECTION;
            break;
        case 2:
            menuData.isDraw = FALSE;
            currentScene = &sceneExit;
            break;
        default:
            break;
        }
        break;
    case MENU_SELECTION:
        switch (inputKey) {
        case -1:
            return;
        case 1:
            Game_Initialize(PLAYER_HUMAN);
            currentScene = &sceneGame;
            break;
        case 2:
            Game_Initialize(PLAYER_AI);
            currentScene = &sceneGame;
            break;
        default:
            break;
        }
        menuData.isDraw = FALSE;
        menuData.currentState = MENU_MAIN;
        break;
    default:
        break;
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



static const char* MESSAGE_EMPTY = NULL;
static const char* MESSAGE_SELECT_TILE = "Select tile.";
static const char* MESSAGE_TILE_IS_NOT_EMPTY = "This tile cannot be selected.";
static const char* GameData_MessageQueue[3] = { NULL, NULL, NULL };
static size_t GameData_MessageHead = 0;
static size_t GameData_MessageTail = 0;
static size_t GameData_MessageCount = 0;

void ClearMessageQueue() {
    GameData_MessageCount = 0;
    GameData_MessageHead = 0;
    GameData_MessageTail = 0;

    for (int i = 0; i < 3; ++i) {
        GameData_MessageQueue[i] = MESSAGE_EMPTY;
    }
}

void DequeueMessage() {
    --GameData_MessageCount;
    GameData_MessageQueue[GameData_MessageHead] = MESSAGE_EMPTY;
    GameData_MessageHead = (GameData_MessageHead + 1) % 3;
}

void EnqueueMessage(const char* pMessage) {
    if (GameData_MessageCount >= 3) {
        DequeueMessage();
    }
    ++GameData_MessageCount;
    GameData_MessageQueue[GameData_MessageTail] = pMessage;
    GameData_MessageTail = (GameData_MessageTail + 1) % 3;
}

void ShowTurnsPlayer(short x, short y) {
    SetCursorPosition(x, y);
    switch (gameData.currentPlayer) {
    case 1:
        printf("1");
        break;
    case 2:
        printf("2");
        break;
    default:
        printf("Error");
        return;
    }
    printf("P's Turn");
}

static const char* tileHintNumberByTileIndex = "1234566789";
// TODO(DevDasae) : implement swappable tile hint feature
static const char* tileHintKeyByTileIndex = "qweasdzxc";

char GetTileByPlayer(int tileIndex) {
    switch (gameData.board[tileIndex]) {
    case TILE_PLAYER_EMPTY:
        return gameData.toggleTileHint ? tileHintKeyByTileIndex[tileIndex] : '_';
    case TILE_PLAYER_ONE:
        return 'O';
    case TILE_PLAYER_TWO:
        return 'X';
    default:
        printf("Error");
        break;
    }
    return '!';

}

static const char* boardLayout = "\
0$c0|0$c0|0$c$n\
---+---+---$n\
0$c0|0$c0|0$c$n\
---+---+---$n\
0$c0|0$c0|0$c$n\
";

void DrawGameBoard(short x, short y) {
    int index = 0;
    int tileIndex = 0;
    SetCursorPosition(x, y);

    while (boardLayout[index] != '\0') {
        switch (boardLayout[index]) {
        case '0':
            printf(" ");
            break;
        case '$':
            switch (boardLayout[++index]) {
            case 'c':
                printf("%c", GetTileByPlayer(tileIndex++));
                break;
            case 'n':
                printf("\n");
                break;
            }
            break;
        default:
            printf("%c", boardLayout[index]);
            break;
        }
        ++index;
    }
}

void DrawMessageBox(short x, short y) {
    SetCursorPosition(x, y);
    for (int i = 0; i < GameData_MessageCount; ++i) {
        printf("%s\n", GameData_MessageQueue[(GameData_MessageHead + i) % 3]);
    }
}


// TODO(DevDasae) : Implement Checking Game End Condition, Workable Game AI
void Game_Initialize(PlayerType player2) {
    gameData.isEnd = FALSE;
    gameData.turnCount = 2;
    gameData.currentPlayer = TILE_PLAYER_ONE;
    gameData.players[0] = PLAYER_HUMAN;
    gameData.players[1] = player2;

    for (int i = 0; i < 9; ++i) {
        gameData.board[i] = TILE_PLAYER_EMPTY;
    }

    ClearMessageQueue();
    EnqueueMessage(MESSAGE_SELECT_TILE);
}

void Game_ProcessInput() {
    inputKey = GetInputKey();

    switch (inputKey) {
    case '0':
        currentScene = &sceneMenu;
        Game_Finalize();
        break;

    case '1':
    case 'q':
    case 'Q':
        inputKey = 1;
        break;

    case '2':
    case 'w':
    case 'W':
        inputKey = 2;
        break;

    case '3':
    case 'e':
    case 'E':
        inputKey = 3;
        break;

    case '4':
    case 'a':
    case 'A':
        inputKey = 4;
        break;

    case '5':
    case 's':
    case 'S':
        inputKey = 5;
        break;

    case '6':
    case 'd':
    case 'D':
        inputKey = 6;
        break;

    case '7':
    case 'z':
    case 'Z':
        inputKey = 7;
        break;

    case '8':
    case 'x':
    case 'X':
        inputKey = 8;
        break;

    case '9':
    case 'c':
    case 'C':
        inputKey = 9;
        break;

    case 'h':
    case 'H':
        gameData.toggleTileHint = !gameData.toggleTileHint;
        gameData.isDraw = FALSE;
    default:
        inputKey = -1;
        break;
    }
}

void Game_Update() {
    if (inputKey == -1 || !gameData.isDraw) { return; }

    gameData.isDraw = FALSE;

    if (gameData.board[inputKey - 1] != TILE_PLAYER_EMPTY) {
        EnqueueMessage(MESSAGE_TILE_IS_NOT_EMPTY);
        return;
    }
    gameData.board[inputKey - 1] = gameData.currentPlayer;
    gameData.currentPlayer = gameData.currentPlayer == TILE_PLAYER_ONE ? TILE_PLAYER_TWO : TILE_PLAYER_ONE;
    ++gameData.turnCount;
    ClearMessageQueue();
    EnqueueMessage(MESSAGE_SELECT_TILE);
}

void Game_Draw() {
    if (gameData.isDraw) { return; }

    DoSystemCls();
    ShowTurnsPlayer(0, 0);
    printf(" : Turn %d", gameData.turnCount / 2);
    DrawGameBoard(0, 3);
    DrawMessageBox(0, 9);

    gameData.isDraw = TRUE;
}

void Game_Finalize() {
    gameData.isDraw = FALSE;
    gameData.isEnd = FALSE;
    gameData.turnCount = 0;
    gameData.currentPlayer = TILE_PLAYER_ONE;
    gameData.players[0] = PLAYER_NONE;
    gameData.players[1] = PLAYER_NONE;

    for (int i = 0; i < 9; ++i) {
        gameData.board[i] = TILE_PLAYER_EMPTY;
    }

    ClearMessageQueue();
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
