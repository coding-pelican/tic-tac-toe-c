#include <stdbool.h>
#include <stdlib.h>


void SetCursorVisible(int visible);
void SetCursorPosition(int posX, int posY);
void DoSystemPause();
void DoSystemCls();


typedef enum eSceneType {
    eSceneType_None = -1,
    eSceneType_Main = 0,
    eSceneType_Game,
    eSceneType_Quit,
    kSceneType_Count
} SceneType;

typedef struct Scene Scene;
struct Scene {
    SceneType const type;
    void (*const ProcessInput)();
    void (*const Update)();
    void (*const Draw)();
};

void   SceneMain_ProcessInput();
void   SceneMain_Update();
void   SceneMain_Draw();
Scene* SceneMain() {
    static Scene scene = {
        .type         = eSceneType_Main,
        .ProcessInput = &SceneMain_ProcessInput,
        .Update       = &SceneMain_Update,
        .Draw         = &SceneMain_Draw,
    };
    return &scene;
}

void   SceneGame_ProcessInput();
void   SceneGame_Update();
void   SceneGame_Draw();
Scene* SceneGame() {
    static Scene scene = {
        .type         = eSceneType_Game,
        .ProcessInput = &SceneGame_ProcessInput,
        .Update       = &SceneGame_Update,
        .Draw         = &SceneGame_Draw,
    };
    return &scene;
}

void   SceneQuit_ProcessInput();
void   SceneQuit_Update();
void   SceneQuit_Draw();
Scene* SceneQuit() {
    static Scene scene = {
        .type         = eSceneType_Quit,
        .ProcessInput = &SceneQuit_ProcessInput,
        .Update       = &SceneQuit_Update,
        .Draw         = &SceneQuit_Draw,
    };
    return &scene;
}

static Scene* const* Scenes() {
    static Scene* scenes[kSceneType_Count] = { 0 };
    static bool   isInitialized            = false;
    if (!isInitialized) {
        scenes[eSceneType_Main] = SceneMain();
        scenes[eSceneType_Game] = SceneGame();
        scenes[eSceneType_Quit] = SceneQuit();
        isInitialized           = true;
    }
    return scenes;
}


struct App {
    void (*const Initial)();
    void (*const Loop)();
    void (*const Release)();

    Scene* currentScene;
    bool   isRunning;
};

void App_Initial();
void App_Loop();
void App_Release();
void App_ChangeScene(SceneType type);
// clang-format off
static struct App App = {
    .Initial = App_Initial,
    .Loop = App_Loop,
    .Release = App_Release,
    .currentScene = 0,
    .isRunning = false
};
// clang-format on

void App_Initial() {
    SetCursorVisible(false);
    DoSystemCls();

    App.currentScene = Scenes()[eSceneType_Main];
    App.isRunning    = true;
}

void App_Loop() {
    while (App.isRunning) {
        App.currentScene->ProcessInput();
        App.currentScene->Update();
        App.currentScene->Draw();
    }
}

void App_Release() {
    App.currentScene = NULL;

    DoSystemCls();
    DoSystemPause();
    SetCursorVisible(true);
}

void App_ChangeScene(SceneType type) { App.currentScene = Scenes()[type]; }



int main(int argc, char const* argv[]) {
    (void)argc, (void)argv;

    App.Initial();
    App.Loop();
    App.Release();

    return 0;
}
