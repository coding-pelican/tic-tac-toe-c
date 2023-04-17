/**
 * @file tic_tac_toe.c
 * @author Gyeongtae Kim(DevDasae, @coding-pelican) (codingpelican@gmail.com)
 * @brief Tic-tac-toe game implemented in C for study purpose and alpha-beta pruning algorithm implementation test
 * @version 0.1
 * @date 2023-04-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <conio.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define swap(A, B, TYPE) do { TYPE t = (A); (A) = (B); (B) = t; } while (0)

enum {
    MESSAGE_COUNT_MAX = 4,
    MINIMAX_DEPTH = 8,
    BOARD_SIZE = 9
};

static inline void Assert(int condition, const char* message) {
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
        printf("\rPress enter key to continue . . .");
    } while (getchar() != '\n');
}

static inline void DoSystemCls() {
    printf("\x1B[2J\x1B[H");
}



static int GetInputKey();
static inline const bool IsRunning();
static inline void SetRunning(bool toggle);



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
    TILE_PLAYER_TWO = -1,
    TILE_PLAYER_EMPTY = 0,
    TILE_PLAYER_ONE = 1,
} BoardTile;

// TODO(DevDasae) : Implement State Machine
typedef struct _Scene {
    void(*ProcessInput)();
    void(*Update)();
    void(*Draw)();
} Scene;

typedef struct _Menu_SceneData {
    MenuStateType currentState;
    bool isDraw;
} Menu_SceneData;
Menu_SceneData menuData = { MENU_MAIN, false };
void Menu_ProcessInput();
void Menu_Update();
void Menu_Draw();
static Scene sceneMenu = { Menu_ProcessInput, Menu_Update, Menu_Draw };

static const char* MESSAGE_EMPTY = NULL;
static const char* MESSAGE_SELECT_TILE = "Select tile.";
static const char* MESSAGE_TILE_IS_NOT_EMPTY = "This tile cannot be selected.";

typedef struct _Game_SceneData {
    PlayerType players[2];
    BoardTile board[BOARD_SIZE];
    BoardTile currentPlayer;
    BoardTile currentOpponent;
    int aiDifficulty;
    int turnCount;
    int emptyTileCount;
    int currentPlayerIndex;
    bool isDraw;
    bool isOver;
    bool toggleTileHint;
    bool enqueuesAiMessage;
    // game message queue
    const char* messageQueue[MESSAGE_COUNT_MAX];
    size_t messageHead;
    size_t messageTail;
    size_t messageCount;
} Game_SceneData;
Game_SceneData gameData = {
    PLAYER_NONE, PLAYER_NONE,
    TILE_PLAYER_EMPTY, TILE_PLAYER_EMPTY, TILE_PLAYER_EMPTY, TILE_PLAYER_EMPTY, TILE_PLAYER_EMPTY, TILE_PLAYER_EMPTY,TILE_PLAYER_EMPTY, TILE_PLAYER_EMPTY, TILE_PLAYER_EMPTY,
    TILE_PLAYER_ONE,
    TILE_PLAYER_TWO,
    0,
    0,
    BOARD_SIZE,
    0,
    false,
    false,
    false,
    false,
    NULL,
    0,
    0,
    0
};
void Game_Initialize(PlayerType player2);
void Game_ProcessInput();
void Game_Update();
void Game_Draw();
void Game_Finalize();
static Scene sceneGame = { Game_ProcessInput, Game_Update, Game_Draw };

typedef struct _Exit_SceneData {
    bool isDraw;
} Exit_SceneData;
Exit_SceneData exitData = { false, };
void Exit_ProcessInput();
void Exit_Update();
void Exit_Draw();
static Scene sceneExit = { Exit_ProcessInput, Exit_Update, Exit_Draw };

static Scene* currentScene = &sceneMenu;
static int inputKey = 0;
static bool isRunning = true;



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

inline const bool IsRunning() {
    return isRunning;
}

inline void SetRunning(bool toggle) {
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
            menuData.isDraw = false;
            menuData.currentState = MENU_SELECTION;
            break;
        case 2:
            menuData.isDraw = false;
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
        menuData.isDraw = false;
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
    menuData.isDraw = true;
}

void ShowTurnsPlayer(short x, short y) {
    SetCursorPosition(x, y);
    switch (gameData.currentPlayer) {
    case -1:
        printf("2");
        break;
    case 1:
        printf("1");
        break;
    default:
        printf("Error");
        return;
    }
    printf("P's Turn");
}

// TODO(DevDasae) : implement swappable tile hint feature
static const unsigned char tileHintNumberByTileIndex[] = "1234566789";
static const unsigned char tileHintKeyByTileIndex[] = "qweasdzxc";

inline unsigned char GetTileHintByTile(int index) {
    return tileHintKeyByTileIndex[index];
}

unsigned char GetTileByPlayer(int tileIndex) {
    switch (gameData.board[tileIndex]) {
    case TILE_PLAYER_TWO:
        return 'X';
    case TILE_PLAYER_EMPTY:
        return gameData.toggleTileHint ? GetTileHintByTile(tileIndex) : '_';
    case TILE_PLAYER_ONE:
        return 'O';
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

void ClearMessageQueue() {
    gameData.messageCount = 0;
    gameData.messageHead = 0;
    gameData.messageTail = 0;

    for (int i = 0; i < MESSAGE_COUNT_MAX; ++i) {
        gameData.messageQueue[i] = MESSAGE_EMPTY;
    }
}

void DequeueMessage() {
    gameData.messageCount--;
    gameData.messageQueue[gameData.messageHead] = MESSAGE_EMPTY;
    gameData.messageHead = (gameData.messageHead + 1) % MESSAGE_COUNT_MAX;
}

void EnqueueMessage(const char* pMessage) {
    if (gameData.messageCount >= MESSAGE_COUNT_MAX) {
        DequeueMessage();
    }
    gameData.messageCount++;
    gameData.messageQueue[gameData.messageTail] = pMessage;
    gameData.messageTail = (gameData.messageTail + 1) % MESSAGE_COUNT_MAX;
}

void DrawMessageBox(short x, short y) {
    SetCursorPosition(x, y);
    for (int i = 0; i < gameData.messageCount; ++i) {
        printf("%s\n", gameData.messageQueue[(gameData.messageHead + i) % MESSAGE_COUNT_MAX]);
    }
}

static unsigned char sentenceHuman[] = "The previous player checked $.";
static unsigned char sentenceAI[] = "The computer checked $.";

const char* GetPlayerCheckedMessage(PlayerType type, int checkTile) {
    if (type == PLAYER_HUMAN) {
        sentenceHuman[28] = checkTile < 10 ? (char)GetTileHintByTile(checkTile) : '?';
        return (const char*)sentenceHuman;
    } else {
        sentenceAI[21] = checkTile < 10 ? (char)GetTileHintByTile(checkTile) : '?';
        return (const char*)sentenceAI;
    }
}

static const int winConditions[8][3] = {
    { 0,1,2 },
    { 3,4,5 },
    { 6,7,8 },
    { 0,3,6 },
    { 1,4,7 },
    { 2,5,8 },
    { 0,4,8 },
    { 2,4,6 }
};

int SatisfiesWinCondition(const int* winConditions, BoardTile player) {
    return (gameData.board[winConditions[0]] == player
        && gameData.board[winConditions[1]] == player
        && gameData.board[winConditions[2]] == player);
}

int HasPlayerWonGame(BoardTile player, const int winConditions[8][3]) {
    for (int i = 0; i < 8; ++i) {
        if (SatisfiesWinCondition(winConditions[i], player)) {
            return true;
        }
    }
    return false;
}

inline bool IsBoardFull() {
    return gameData.emptyTileCount < 1 ? true : false;
}

int Evaluate(BoardTile player, BoardTile opponent) {
    if (HasPlayerWonGame(player, winConditions)) {
        return 1;
    } else if (HasPlayerWonGame(opponent, winConditions)) {
        return -1;
    } else if (IsBoardFull()) {
        return 0;
    } else {
        return -2; // game is not over yet
    }
}

int Minimax(BoardTile player, BoardTile opponent, int depth, bool maximizingPlayer) {
    if (depth == 0 || IsBoardFull()) {
        return Evaluate(opponent, player);
    }
    int bestValue = 0;
    if (maximizingPlayer) {
        bestValue = INT_MIN;
        for (int i = 0; i < BOARD_SIZE; ++i) {
            if (gameData.board[i] == TILE_PLAYER_EMPTY) {
                gameData.board[i] = player;
                int value = Minimax(opponent, player, depth - 1, false);
                gameData.board[i] = TILE_PLAYER_EMPTY;
                bestValue = max(bestValue, value);
            }
        }
    } else {
        bestValue = INT_MAX;
        for (int i = 0; i < BOARD_SIZE; ++i) {
            if (gameData.board[i] == TILE_PLAYER_EMPTY) {
                gameData.board[i] = opponent;
                int value = Minimax(player, opponent, depth - 1, true);
                gameData.board[i] = TILE_PLAYER_EMPTY;
                bestValue = min(bestValue, value);
            }
        }
    }
    return bestValue;
}

inline int GetPlayerIndex(BoardTile player) {
    return player == TILE_PLAYER_ONE ? 0 : 1;
}

void Game_Initialize(PlayerType player2) {
    gameData.players[0] = PLAYER_HUMAN;
    gameData.players[1] = player2;

    for (int i = 0; i < BOARD_SIZE; ++i) {
        gameData.board[i] = TILE_PLAYER_EMPTY;
    }

    gameData.currentPlayer = TILE_PLAYER_ONE;
    gameData.currentOpponent = TILE_PLAYER_TWO;
    gameData.currentPlayerIndex = 0;
    gameData.turnCount = 2;
    gameData.emptyTileCount = BOARD_SIZE;
    gameData.isDraw = false;
    gameData.isOver = false;
    gameData.enqueuesAiMessage = false;
    ClearMessageQueue();

    EnqueueMessage(MESSAGE_SELECT_TILE);
}

void Game_ProcessInput() {
    inputKey = GetInputKey();

    switch (inputKey) {
    case 13: // Enter
    case 32: // Space
        if (!gameData.isOver) {
            break;
        }
    case 27: // ESC
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
        gameData.isDraw = false;
    default:
        inputKey = -1;
        break;
    }
}

// TODO(DevDasae) : Add New Game Mode
void Game_Update() {
    // 입력이 없는가, 게임이 렌더링 되어있지 않은가, 게임이 끝났는가
    gameData.currentPlayerIndex = GetPlayerIndex(gameData.currentPlayer);
    if ((gameData.players[gameData.currentPlayerIndex] == PLAYER_HUMAN && inputKey == -1) || !gameData.isDraw || gameData.isOver) { return; };

    gameData.isDraw = false;

    if (gameData.players[gameData.currentPlayerIndex] == PLAYER_HUMAN) { // 현재 플레이어가 인간이라면
        if (gameData.board[inputKey - 1] != TILE_PLAYER_EMPTY) { // 플레이어의 입력 칸이 비어있지 않다면
            EnqueueMessage(MESSAGE_TILE_IS_NOT_EMPTY);
            return;
        }

        gameData.board[inputKey - 1] = gameData.currentPlayer;
        ClearMessageQueue();
    } else { // 현재 플레이어가 AI라면
        if (!gameData.enqueuesAiMessage) {
            gameData.enqueuesAiMessage = !gameData.enqueuesAiMessage;
            EnqueueMessage("The computer is thinking...");
            Delay(200);
            return;
        }
        gameData.enqueuesAiMessage = !gameData.enqueuesAiMessage;
        int aiMove = 0;
        switch (gameData.aiDifficulty) {
        case 1:
            // Easy mode: select a random empty tile
            do {
                aiMove = rand() % BOARD_SIZE;
            } while (gameData.board[aiMove] != TILE_PLAYER_EMPTY);
            break;

        default:
            // Hard mode: use the minimax algorithm
            int bestValue = INT_MIN;
            for (int i = 0; i < BOARD_SIZE; i++) {
                if (gameData.board[i] == TILE_PLAYER_EMPTY) {
                    gameData.board[i] = gameData.currentPlayer;
                    int currentValue = Minimax(gameData.currentPlayer, gameData.currentOpponent, gameData.aiDifficulty, true);
                    gameData.board[i] = TILE_PLAYER_EMPTY;
                    if (currentValue > bestValue) {
                        bestValue = currentValue;
                        aiMove = i;
                    }
                }
            }
            break;
        }
        gameData.board[aiMove] = gameData.currentPlayer;
        inputKey = aiMove + 1;
    }
    EnqueueMessage(GetPlayerCheckedMessage(gameData.players[gameData.currentPlayerIndex], inputKey - 1));

    gameData.emptyTileCount--;

    int turnResult = Evaluate(gameData.currentPlayer, gameData.currentOpponent);
    if (turnResult == 1 || turnResult == -1) {
        gameData.isDraw = false;
        gameData.isOver = true;
        EnqueueMessage("Congratulations! You won!\n");
        return;
    } else if (turnResult == 0) {
        gameData.isDraw = false;
        gameData.isOver = true;
        EnqueueMessage("It's a draw.");
        return;
    } else {
        swap(gameData.currentPlayer, gameData.currentOpponent, BoardTile);
        gameData.turnCount++;
        if (gameData.players[gameData.currentPlayerIndex] == PLAYER_HUMAN) {
            EnqueueMessage(MESSAGE_SELECT_TILE);
        }
    }
}

void Game_Draw() {
    if (gameData.isDraw) { return; }

    DoSystemCls();
    ShowTurnsPlayer(0, 0);
    printf(" : Turn %d", gameData.turnCount / 2);
    DrawGameBoard(0, 3);
    DrawMessageBox(0, 9);

    gameData.isDraw = true;
}

void Game_Finalize() {
    gameData.players[0] = PLAYER_NONE;
    gameData.players[1] = PLAYER_NONE;

    for (int i = 0; i < BOARD_SIZE; ++i) {
        gameData.board[i] = TILE_PLAYER_EMPTY;
    }

    gameData.currentPlayer = TILE_PLAYER_ONE;
    gameData.currentOpponent = TILE_PLAYER_TWO;
    gameData.currentPlayerIndex = 0;
    gameData.turnCount = 0;
    gameData.emptyTileCount = BOARD_SIZE;
    gameData.isDraw = false;
    gameData.isOver = false;
    gameData.enqueuesAiMessage = false;
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
        exitData.isDraw = false;
        currentScene = &sceneMenu;
    case -1:
        break;
    default:
        return SetRunning(false);
    }
}

void Exit_Draw() {
    if (exitData.isDraw) {
        return;
    }
    puts("게임을 종료하시겠습니까? Y/n");
    exitData.isDraw = true;
}



int main(int argc, char const* argv[]) {
    SetCursorVisible(false);
    DoSystemCls();

    while (IsRunning()) {
        currentScene->ProcessInput();
        currentScene->Update();
        currentScene->Draw();
    }

    DoSystemCls();
    DoSystemPause();
    SetCursorVisible(true);
    return 0;
}
