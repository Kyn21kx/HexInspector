#pragma once

#include "AppState.hpp"
#include "raylib.h"
#include "clay.h"

class Application {
public:

    void Run();
    void Init();
    void Update(float dt);
    void Draw();
    void Dispose();

    void BuildUI();

private:
    Font fonts[10];
    bool initialized = false;
    AppState m_appState;
    Clay_Arena m_arena;
};
