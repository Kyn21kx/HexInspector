#pragma once

#include "AppState.hpp"
#include "raylib.h"
#include "clay.h"

struct AppState;

class Application {
public:

    void Run();
    void Init();
    void Update(float dt);
    void Draw();
    void Dispose();

    void BuildUI(AppState* appState);

private:

    void HandleInput();
    
    Font fonts[10];
    bool initialized = false;
    AppState m_appState;
    Clay_Arena m_arena;
};
