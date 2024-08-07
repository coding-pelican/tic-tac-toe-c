/**
 * @file tic_tac_toe.c
 * @author Gyeongtae Kim(dev-dasae) <codingpelican@gmail.com>
 *
 * @brief Tic-tac-toe game implemented in C for study purpose
    and alpha-beta pruning algorithm implementation test
 *
 * @version 0.2
 * @date 2024-04-07
 *
 * @copyright Released under the MIT License. See LICENSE file for details.
 */


// #region Header_Inclusion
#include <conio.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// #endregion // Header_Inclusion

// #region Pre-process_Definitions
#define max(_a, _b) (((_a) > (_b)) ? (_a) : (_b))
#define min(_a, _b) (((_a) < (_b)) ? (_a) : (_b))
#define swap(_T, _a, _b) \
    do {                 \
        _T __t = (_a);   \
        (_a)   = (_b);   \
        (_b)   = __t;    \
    } while (0)
// #endregion // Pre-process_Definitions



enum {
    MESSAGE_COUNT_MAX = 4,
    MINIMAX_DEPTH     = 8,
    BOARD_SIZE        = 9,
    INPUT_MAP_SIZE    = 256,
};

static inline void Assert(int condition, const char* message) {
    if (!condition) {
        (void)fprintf(stderr, "%s(%s: %d)\n", message, __FILE__, __LINE__);
        __builtin_trap();
    }
}

static inline void Delay(clock_t waitMS) {
    clock_t endMS = waitMS + clock();
    while (endMS > clock()) { ; }
}

static void SetCursorVisible(int visible) { printf(visible ? "\x1B[?25h" : "\x1B[?25l"); }

static inline void SetCursorPosition(short posX, short posY) { printf("\x1B[%d;%dH", posY, posX); }

static inline void DoSystemPause() {
    do {
        printf("\rPress enter key to continue . . .");
    } while (getchar() != '\n');
}

static inline void DoSystemCls() { printf("\x1B[2J\x1B[H"); }

static int         GetInputKey();
static inline bool IsRunning();
static inline void SetRunning(bool toggle);

typedef enum eSceneStateType {
    SceneState_None = 0,
    SceneState_Menu,
    SceneState_Game,
    SceneState_Quit
} SceneType;

typedef enum eMenuStateType {
    MenuState_None = 0,
    MenuState_Main,
    MenuState_SelectionPlayMode,
    MenuState_SelectionAILevel,
    MenuState_SelectionPlayerOrder
} MenuStateType;

typedef enum ePlayerType {
    Player_None = 0,
    Player_AI,
    Player_Human
} PlayerType;

typedef enum eBoardTile {
    BoardTile_PlayerTwo   = -1,
    BoardTile_PlayerEmpty = 0,
    BoardTile_PlayerOne   = 1,
} BoardTile;

// TODO(DevDasae): Implement State Machine
typedef struct Scene {
    void (*ProcessInput)();
    void (*Update)();
    void (*Draw)();
} Scene;

typedef struct Menu_SceneData {
    MenuStateType currentState;
    bool          redraws;
} Menu_SceneData;
Menu_SceneData menuData = { MenuState_Main, false };
void           Menu_ProcessInput();
void           Menu_Update();
void           Menu_Draw();
static Scene   sceneMenu = { Menu_ProcessInput, Menu_Update, Menu_Draw };

static const char* MESSAGE_EMPTY                  = NULL;
static const char* MESSAGE_SELECT_TILE            = "Select tile.";
static const char* MESSAGE_BoardTile_IS_NOT_EMPTY = "This tile cannot be selected.";

typedef struct Game_SceneData {
    PlayerType players[2];
    BoardTile  board[BOARD_SIZE];
    BoardTile  currentPlayer;
    BoardTile  currentOpponent;
    int        aiDifficulty;
    int        turnCount;
    int        emptyTileCount;
    int        currentPlayerIndex;
    bool       redraws;
    bool       isOver;
    bool       toggleTileHint;
    bool       enqueuesAiMessage;
    // game message queue
    const char* messageQueue[MESSAGE_COUNT_MAX];
    size_t      messageHead;
    size_t      messageTail;
    size_t      messageCount;
} Game_SceneData;
Game_SceneData gameData = {
    { Player_None, Player_None },
    { BoardTile_PlayerEmpty, BoardTile_PlayerEmpty, BoardTile_PlayerEmpty, BoardTile_PlayerEmpty, BoardTile_PlayerEmpty, BoardTile_PlayerEmpty, BoardTile_PlayerEmpty, BoardTile_PlayerEmpty, BoardTile_PlayerEmpty },
    BoardTile_PlayerOne,
    BoardTile_PlayerTwo,
    1,
    0,
    BOARD_SIZE,
    0,
    false,
    false,
    false,
    false,
    {
        NULL,
        0,
        0,
        0,
    },
    0,
    0,
    0
};
void         Game_Initialize(PlayerType player1, PlayerType player2);
void         Game_ProcessInput();
void         Game_Update();
void         Game_Draw();
void         Game_Finalize();
static Scene sceneGame = { Game_ProcessInput, Game_Update, Game_Draw };

typedef struct Exit_SceneData {
    bool redraws;
} Exit_SceneData;
Exit_SceneData quitData = {
    false,
};
void         Quit_ProcessInput();
void         Quit_Update();
void         Quit_Draw();
static Scene sceneExit = {
    Quit_ProcessInput,
    Quit_Update,
    Quit_Draw
};

static Scene* currentScene = &sceneMenu;
static int    inputKey     = 0;
static bool   isRunning    = true;

int GetInputKey() {
    if (kbhit()) {
        int key = getch();
        if (key == 0xE0 || key == 0) {
            return getch();
        }
        return key;
    }
    return -1;
}

static inline bool IsRunning() { return isRunning; }

static inline void SetRunning(bool toggle) { isRunning = toggle; }

void Menu_ProcessInput() {
    inputKey = GetInputKey();

    switch (inputKey) {
    case '1':
        inputKey = 1;
        break;
    case '2':
        inputKey = 2;
        break;
    case '3':
        inputKey = 3;
        break;
    default:
        break;
    }
}

void Menu_Update() {
    switch (menuData.currentState) {
    case MenuState_Main:
        switch (inputKey) {
        case 1:
            menuData.redraws      = false;
            menuData.currentState = MenuState_SelectionPlayMode;
            break;
        case 2:
            menuData.redraws = false;
            currentScene     = &sceneExit;
            break;
        default:
            break;
        }
        break;
    case MenuState_SelectionPlayMode:
        switch (inputKey) {
        case -1:
            return;
        case 1:
            Game_Initialize(Player_Human, Player_Human);
            currentScene = &sceneGame;
            break;
        case 2:
            Game_Initialize(Player_Human, Player_AI);
            currentScene = &sceneGame;
            break;
        case 3:
            Game_Initialize(Player_AI, Player_AI);
            currentScene = &sceneGame;
        default:
            break;
        }
        menuData.redraws      = false;
        menuData.currentState = MenuState_Main;
        break;
    default:
        break;
    }
}

void Menu_Draw() {
    if (menuData.redraws) { return; }
    DoSystemCls();
    switch (menuData.currentState) {
    case MenuState_Main:
        puts("Tic Tac Toe\n");

        puts("1. New Game");
        puts("2. Quit");
        break;

    case MenuState_SelectionPlayMode:
        puts("Select Game Play Mode\n");

        puts("- 1. Play with Another Player");
        puts("- 2. Play with A.I.");
        puts("- 3. Watch A.I. game play\n");

        puts("- or Go To Menu");
        break;

    case MenuState_SelectionPlayerOrder:
        puts("Select Order of Player\n");

        puts("- 1. Player 1");
        puts("- 2. Player 2\n");
        break;

    case MenuState_SelectionAILevel:
        puts("Select Game A.I. Difficult\n");

        puts("- 1. Easy");
        puts("- 2. Hard");
        break;

    default:
        printf("Error\n");
        break;
    }
    menuData.redraws = true;
}

void ShowTurnsPlayer(short posX, short posY) {
    SetCursorPosition(posX, posY);
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
// static const unsigned char tileHintNumberByTileIndex[] = "1234566789";
static const unsigned char tileHintKeyByTileIndex[] = "qweasdzxc";

static inline unsigned char GetTileHintByTile(int index) { return tileHintKeyByTileIndex[index]; }

unsigned char GetTileByPlayer(int tileIndex) {
    switch (gameData.board[tileIndex]) {
    case BoardTile_PlayerTwo:
        return 'X';
    case BoardTile_PlayerEmpty:
        return gameData.toggleTileHint ? GetTileHintByTile(tileIndex) : '_';
    case BoardTile_PlayerOne:
        return 'O';
    default:
        Assert(false, "Un-handled character in board layout");
        return '!';
    }
}

static const char* boardLayout = "\
0$c0|0$c0|0$c$n\
---+---+---$n\
0$c0|0$c0|0$c$n\
---+---+---$n\
0$c0|0$c0|0$c$n\
";

void DrawGameBoard(short posX, short posY) {
    int index     = 0;
    int tileIndex = 0;
    SetCursorPosition(
        posX,
        posY
    );

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
            default:
                Assert(false, "Invalid character in board layout");
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
    gameData.messageHead  = 0;
    gameData.messageTail  = 0;

    for (int i = 0; i < MESSAGE_COUNT_MAX; ++i) {
        gameData.messageQueue[i] = MESSAGE_EMPTY;
    }
}

void DequeueMessage() {
    gameData.messageQueue[gameData.messageHead] = MESSAGE_EMPTY;

    gameData.messageHead = (gameData.messageHead + 1) % MESSAGE_COUNT_MAX;
    gameData.messageCount--;
}

void EnqueueMessage(const char* pMessage) {
    if (gameData.messageCount >= MESSAGE_COUNT_MAX) {
        DequeueMessage();
    }
    gameData.messageQueue[gameData.messageTail] = pMessage;

    gameData.messageTail = (gameData.messageTail + 1) % MESSAGE_COUNT_MAX;
    gameData.messageCount++;
}

void DrawMessageBox(short posX, short posY) {
    SetCursorPosition(posX, posY);
    for (int i = 0; i < (int)gameData.messageCount; ++i) {
        printf("%s\n", gameData.messageQueue[(gameData.messageHead + i) % MESSAGE_COUNT_MAX]);
    }
}

static unsigned char sentenceHuman[] = "The previous player checked $.";
static unsigned char sentenceAI[]    = "The computer checked $.";

const char* GetPlayerCheckedMessage(PlayerType type, int checkTile) { // NOLINT
    if (type == Player_Human) {
        sentenceHuman[28] = checkTile < 10 ? (char)GetTileHintByTile(checkTile) : '?';
        return (const char*)sentenceHuman;
    }
    sentenceAI[21] = checkTile < 10 ? (char)GetTileHintByTile(checkTile) : '?';
    return (const char*)sentenceAI;
}

// clang-format off
static const int winConditions[8][3] = {
    {0, 1, 2},
    {3, 4, 5},
    {6, 7, 8},
    {0, 3, 6},
    {1, 4, 7},
    {2, 5, 8},
    {0, 4, 8},
    {2, 4, 6}
};
// clang-format on

int SatisfiesWinCondition(const int* winConditions, BoardTile player) {
    return (gameData.board[winConditions[0]] == player && gameData.board[winConditions[1]] == player && gameData.board[winConditions[2]] == player);
}

int HasPlayerWonGame(BoardTile player, const int winConditions[8][3]) {
    for (int i = 0; i < 8; ++i) {
        if (SatisfiesWinCondition(winConditions[i], player)) {
            return true;
        }
    }
    return false;
}

static inline bool IsBoardFull() { return gameData.emptyTileCount < 1 ? true : false; }

int Evaluate(BoardTile player, BoardTile opponent) {
    if (HasPlayerWonGame(player, winConditions)) { return 1; }
    if (HasPlayerWonGame(opponent, winConditions)) { return -1; }
    if (IsBoardFull()) { return 0; }
    return -2; // game is not over yet
}

int MinimaxRecursive(BoardTile player, BoardTile opponent, int depth, bool maximizingPlayer) { // NOLINT
    int score = Evaluate(player, opponent);
    if (score != -2 || depth == 0) { return score; }

    int bestValue = 0;
    if (maximizingPlayer) {
        bestValue = INT_MIN;
        for (int i = 0; i < BOARD_SIZE; ++i) {
            if (gameData.board[i] == BoardTile_PlayerEmpty) {
                gameData.board[i] = player;
                int value         = MinimaxRecursive(player, opponent, depth - 1, false);
                gameData.board[i] = BoardTile_PlayerEmpty;
                bestValue         = max(bestValue, value);
            }
        }
    } else {
        bestValue = INT_MAX;
        for (int i = 0; i < BOARD_SIZE; ++i) {
            if (gameData.board[i] == BoardTile_PlayerEmpty) {
                gameData.board[i] = opponent;
                int value         = MinimaxRecursive(opponent, player, depth - 1, true);
                gameData.board[i] = BoardTile_PlayerEmpty;
                bestValue         = min(bestValue, value);
            }
        }
    }

    return bestValue;
}

static inline int GetPlayerIndex(BoardTile player) { return player == BoardTile_PlayerOne ? 0 : 1; }

void Game_Initialize(PlayerType player1, PlayerType player2) {
    gameData.players[0] = player1;
    gameData.players[1] = player2;

    for (int i = 0; i < BOARD_SIZE; ++i) {
        gameData.board[i] = BoardTile_PlayerEmpty;
    }

    gameData.currentPlayer      = BoardTile_PlayerOne;
    gameData.currentOpponent    = BoardTile_PlayerTwo;
    gameData.currentPlayerIndex = 0;
    gameData.turnCount          = 2;
    gameData.emptyTileCount     = BOARD_SIZE;
    gameData.redraws            = false;
    gameData.isOver             = false;
    gameData.enqueuesAiMessage  = false;
    ClearMessageQueue();

    EnqueueMessage(MESSAGE_SELECT_TILE);
}

enum InputKey {
    KEY_NONE  = -1,
    KEY_0     = '0',
    KEY_1     = '1',
    KEY_2     = '2',
    KEY_3     = '3',
    KEY_4     = '4',
    KEY_5     = '5',
    KEY_6     = '6',
    KEY_7     = '7',
    KEY_8     = '8',
    KEY_9     = '9',
    KEY_Q     = 'q',
    KEY_W     = 'w',
    KEY_E     = 'e',
    KEY_A     = 'a',
    KEY_S     = 's',
    KEY_D     = 'd',
    KEY_Z     = 'z',
    KEY_X     = 'x',
    KEY_C     = 'c',
    KEY_H     = 'h',
    KEY_ENTER = 13,
    KEY_SPACE = 32,
    KEY_ESC   = 27,
};

void Game_ProcessInput() {
    inputKey = GetInputKey();
    switch (inputKey) {
    case KEY_ENTER:
    case KEY_SPACE:
        if (!gameData.isOver) {
            break;
        }
    case KEY_ESC:
    case KEY_0:
        currentScene = &sceneMenu;
        Game_Finalize();
        break;
    case KEY_1:
    case KEY_Q:
        inputKey = 1;
        break;
    case KEY_2:
    case KEY_W:
        inputKey = 2;
        break;
    case KEY_3:
    case KEY_E:
        inputKey = 3;
        break;
    case KEY_4:
    case KEY_A:
        inputKey = 4;
        break;
    case KEY_5:
    case KEY_S:
        inputKey = 5;
        break;
    case KEY_6:
    case KEY_D:
        inputKey = 6;
        break;
    case KEY_7:
    case KEY_Z:
        inputKey = 7;
        break;
    case KEY_8:
    case KEY_X:
        inputKey = 8;
        break;
    case KEY_9:
    case KEY_C:
        inputKey = 9;
        break;
    case KEY_H:
        gameData.toggleTileHint = !gameData.toggleTileHint;
        gameData.redraws        = false;
        break;
    default:
        inputKey = KEY_NONE;
        break;
    }
}

int GetAIMove(int difficulty, BoardTile* board, BoardTile player, BoardTile opponent) {
    int aiMove = 0;

    if (difficulty == 1) {
        do {
            aiMove = rand() % BOARD_SIZE; // NOLINT
        } while (board[aiMove] != BoardTile_PlayerEmpty);
        return aiMove;
    }

    int bestValue = INT_MIN;
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (board[i] == BoardTile_PlayerEmpty) {
            board[i]         = player;
            int currentValue = MinimaxRecursive(player, opponent, difficulty, true);
            board[i]         = BoardTile_PlayerEmpty;
            if (currentValue > bestValue) {
                bestValue = currentValue;
                aiMove    = i;
            }
        }
    }
    return aiMove;
}

// TODO(DevDasae) : Add New Game Mode
void Game_Update() {
    gameData.currentPlayerIndex = GetPlayerIndex(gameData.currentPlayer);
    bool const isHumanTurn      = gameData.players[gameData.currentPlayerIndex] == Player_Human;
    bool       isGameOver       = gameData.isOver;
    bool       isDrawn          = gameData.redraws;

    if ((isHumanTurn && inputKey == -1) || !isDrawn || isGameOver) {
        return;
    }

    gameData.redraws = false;

    if (isHumanTurn) {
        if (gameData.board[inputKey - 1] != BoardTile_PlayerEmpty) {
            EnqueueMessage(MESSAGE_BoardTile_IS_NOT_EMPTY);
            return;
        }

        gameData.board[inputKey - 1] = gameData.currentPlayer;
        ClearMessageQueue();
    } else {
        if (!gameData.enqueuesAiMessage) {
            gameData.enqueuesAiMessage = true;
            EnqueueMessage("The computer is thinking...");
            Delay(200);
            return;
        }
        gameData.enqueuesAiMessage = false;
        int aiMove                 = GetAIMove(gameData.aiDifficulty, gameData.board, gameData.currentPlayer, gameData.currentOpponent);
        gameData.board[aiMove]     = gameData.currentPlayer;
        inputKey                   = aiMove + 1;
    }
    EnqueueMessage(GetPlayerCheckedMessage(gameData.players[gameData.currentPlayerIndex], inputKey - 1));

    gameData.emptyTileCount--;

    int turnResult = Evaluate(gameData.currentPlayer, gameData.currentOpponent);
    if (turnResult == 1 || turnResult == -1) {
        gameData.redraws = false;
        gameData.isOver  = true;
        EnqueueMessage("Congratulations! You won!\n");
        return;
    }
    if (turnResult == 0) {
        gameData.redraws = false;
        gameData.isOver  = true;
        EnqueueMessage("It's a draw.");
        return;
    }
    swap(BoardTile, gameData.currentPlayer, gameData.currentOpponent);
    gameData.turnCount++;
    if (isHumanTurn) {
        EnqueueMessage(MESSAGE_SELECT_TILE);
    }
}



void Game_Draw() {
    if (gameData.redraws) { return; }

    DoSystemCls();
    ShowTurnsPlayer(0, 0);
    printf(" : Turn %d", gameData.turnCount / 2);
    DrawGameBoard(0, 3);
    DrawMessageBox(0, 9);

    gameData.redraws = true;
}

void Game_Finalize() {
    gameData.players[0] = Player_None;
    gameData.players[1] = Player_None;

    for (int i = 0; i < BOARD_SIZE; ++i) {
        gameData.board[i] = BoardTile_PlayerEmpty;
    }

    gameData.currentPlayer      = BoardTile_PlayerOne;
    gameData.currentOpponent    = BoardTile_PlayerTwo;
    gameData.currentPlayerIndex = 0;
    gameData.turnCount          = 0;
    gameData.emptyTileCount     = BOARD_SIZE;
    gameData.redraws            = false;
    gameData.isOver             = false;
    gameData.enqueuesAiMessage  = false;
    ClearMessageQueue();
}

void Quit_ProcessInput() {
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

void Quit_Update() {
    switch (inputKey) {
    case 1:
        quitData.redraws = false;
        currentScene     = &sceneMenu;
    case -1:
        break;
    default:
        return SetRunning(false);
    }
}

void Quit_Draw() {
    if (quitData.redraws) { return; }
    puts("Do you want to quit the game? (Y/n)");
    quitData.redraws = true;
}

int main(int argc, char const* argv[]) {
    (void)argc, (void)argv;

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
