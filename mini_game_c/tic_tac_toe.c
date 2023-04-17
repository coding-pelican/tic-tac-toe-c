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

#define SWAP(A, B, TYPE) do { TYPE t = (A); (A) = (B); (B) = t; } while (0)

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
        printf("\rPress any key to continue . . .");
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
    int turnCount;
    int emptyTileCount;
    int currentPlayerIndex;
    bool isDraw;
    bool isOver;
    bool toggleTileHint;
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
    BOARD_SIZE,
    0,
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

// Player = TILE_PLAYER_O, Opponent = TILE_PLAYER_X

// Evaluate the current state of the board and return a score.
// Returns 1 if X wins, -1 if O wins, 0 if the game is a tie, and INT_MIN if the game is not over.
int Evaluate(BoardTile player, BoardTile opponent) {
    // Check if either player has won the game.
    if (HasPlayerWonGame(player, winConditions)) {
        return 1;
    } else if (HasPlayerWonGame(opponent, winConditions)) {
        return -1;
    }

    // If the board is full and neither player has won, the game is a tie.
    if (gameData.emptyTileCount < 1) {
        return 0;
    }

    // If the game is not over, return the worst possible score.
    return INT_MIN;
}

// Returns the best score for the given player, assuming perfect play from both sides.
int AlphaBeta(BoardTile player, BoardTile opponent, int alpha, int beta) {
    // Check if the game is over or not.
    int result = Evaluate(player, opponent);
    if (result != INT_MIN) {
        return result * player;
    }

    // Initialize best score with the worst possible value.
    int bestScore = INT_MIN;

    // Loop through all possible moves on the board.
    for (int index = 0; index < BOARD_SIZE; index++) {
        if (gameData.board[index] != TILE_PLAYER_EMPTY) { continue; }
        // If this move is available, play it and get the score for the next move.
        gameData.board[index] = player;
        int score = -AlphaBeta(opponent, player, -beta, -alpha);
        gameData.board[index] = TILE_PLAYER_EMPTY;

        // Update the best score and alpha value.
        if (score > bestScore) {
            bestScore = score;
        }
        if (score > alpha) {
            alpha = score;
        }

        // If alpha >= beta, we can prune the rest of the moves.
        if (alpha >= beta) {
            return bestScore;
        }
    }

    // If the game ends in a tie, return a score of 0.
    if (bestScore == INT_MIN) {
        return 0;
    }

    return bestScore;
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
    gameData.currentPlayerIndex = 0;
    gameData.turnCount = 2;
    gameData.emptyTileCount = BOARD_SIZE;
    gameData.isDraw = false;
    gameData.isOver = false;
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

void Game_Update() {
    // 입력이 없는가, 게임이 렌더링 되어있지 않은가,게임이 끝났는가
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
        EnqueueMessage("The computer is thinking...");
        int bestScore = INT_MIN;
        int bestIndex = -1;

        for (int index = 0; index < BOARD_SIZE; index++) {
            if (gameData.board[index] != TILE_PLAYER_EMPTY) { continue; }

            gameData.board[index] = gameData.currentPlayer;
            int score = -AlphaBeta(gameData.currentPlayer, gameData.currentOpponent, INT_MIN, INT_MAX);
            gameData.board[index] = TILE_PLAYER_EMPTY;

            if (score > bestScore) {
                bestScore = score;
                bestIndex = index;
            }
        }
        gameData.board[bestIndex] = gameData.currentPlayer;
        inputKey = bestIndex + 1;
    }
    --gameData.emptyTileCount;
    EnqueueMessage(
        GetPlayerCheckedMessage(gameData.players[gameData.currentPlayerIndex], inputKey - 1));

    int result = Evaluate(gameData.currentPlayer, gameData.currentOpponent);
    if (result == 1 || result == -1) {
        gameData.isDraw = false;
        gameData.isOver = true;
        EnqueueMessage("Congratulations! You won!\n");
    } else if (result == 0) {
        gameData.isDraw = false;
        gameData.isOver = true;
        EnqueueMessage("It's a draw.");
    } else {
        gameData.currentPlayer = gameData.currentPlayer == TILE_PLAYER_ONE ? TILE_PLAYER_TWO : TILE_PLAYER_ONE;
        gameData.currentOpponent = gameData.currentOpponent == TILE_PLAYER_TWO ? TILE_PLAYER_ONE : TILE_PLAYER_TWO;
        ++gameData.turnCount;
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
    gameData.currentPlayerIndex = 0;
    gameData.turnCount = 0;
    gameData.emptyTileCount = BOARD_SIZE;
    gameData.isDraw = false;
    gameData.isOver = false;
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
