#include "Application.hpp"

#include "AppState.hpp"
#include "ColorUtils.hpp"
#include "FileLayer.hpp"
#include "LayoutUtils.hpp"
#include "Result.hpp"
#include "TextUtils.hpp"
#include "components/Buttons.hpp"
#include "raylib.h"
#include <cassert>
extern "C" {
#include "renderer/clay_renderer_raylib.h"
}
#include <cstdio>
#include <cstdlib>


constexpr Clay_Color PANEL_COLOR = ColorUtils::PANEL_BG();
constexpr Clay_Color BACKGROUND_COLOR = ColorUtils::BORDER();

void Application::Run() {
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        Update(dt);
        Draw();
    }
}

void Application::Init() {
    uint32_t memorySize = Clay_MinMemorySize();
    void* memory = std::malloc(memorySize);

    this->m_arena = Clay_CreateArenaWithCapacityAndMemory(memorySize, memory);
    Clay_Initialize(this->m_arena, { .width = 1280, .height = 800}, {});
    Clay_Raylib_Initialize(1280, 800, "HexViewer", FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);

    fonts[0] = LoadFontEx("assets/fonts/Nova_Square/NovaSquare-Regular.ttf", 72, NULL, 0);
    if (fonts[0].texture.id == 0) fonts[0] = GetFontDefault();

    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);
    initialized = true;
}

void Application::Update(float dt) {
    Clay_SetLayoutDimensions((Clay_Dimensions){
        .width = (float)GetScreenWidth(),
        .height = (float)GetScreenHeight(),
    });
    Clay_SetPointerState(
        (Clay_Vector2){ .x = (float)GetMouseX(), .y = (float)GetMouseY() },
        IsMouseButtonDown(MOUSE_BUTTON_LEFT)
    );
    Clay_UpdateScrollContainers(true, (Clay_Vector2){ .x = 0, .y = 0 }, dt);
    if (IsFileDropped()) {
        FilePathList pathList = LoadDroppedFiles();

        assert(pathList.count <= 1 && "Hex viewer currently only handles one file at a time!");

        Result<uint8_t*, FileLayer::EError> readResult;
        Result<File, FileLayer::EError> openRes;

        const char* pathToLoad = pathList.paths[0];
        openRes = FileLayer::OpenFile(pathToLoad, EFileFlags::MemoryMapped);
        if (!openRes) {
            // Push error
            goto unload_files;
        }

        this->m_appState.currentFile = openRes.Val();

        // This format of the call is specifically for mmapped files
        readResult = FileLayer::ReadIntoBuffer(nullptr, 0, this->m_appState.currentFile);

        if (!readResult) {
            goto unload_files;
        }

        this->m_appState.binaryContentBuffer = readResult.Val();

        unload_files: {
            UnloadDroppedFiles(pathList);
        }
    }
}

void Application::Draw() {
    BeginDrawing();
    ClearBackground((Color){ 255, 255, 255, 255 });

    Clay_BeginLayout();
    BuildUI();
    Clay_RenderCommandArray cmds = Clay_EndLayout();
    Clay_Raylib_Render(cmds, fonts);

    EndDrawing();
}

Clay_LayoutConfig GrowingLayout(Clay_LayoutDirection dir, uint16_t gap, uint16_t pad) {
    return (Clay_LayoutConfig){
        .sizing = { .width = CLAY_SIZING_GROW(), .height = CLAY_SIZING_GROW() },
        .padding = { .left = pad, .right = pad, .top = pad, .bottom = pad },
        .childGap = gap,
        .layoutDirection = dir,
    };
}

void DrawHeader() {
    CLAY({
        .id = CLAY_ID("HeaderBar"),
        .layout = {
            .padding = { .left = 8, .right = 8, .top = 8, .bottom = 8 },
            .sizing = { .height = CLAY_SIZING_FIXED(36), .width = CLAY_SIZING_GROW(0) },
        },
        .backgroundColor = PANEL_COLOR,
        .border = { .color = ColorUtils::BLACK_(), .width = { 0, 0, 1, 0, 0 } },
    }) {
        CLAY_TEXT(CLAY_STRING("HexViewer"),
            CLAY_TEXT_CONFIG({ .fontSize = 16, .textColor = ColorUtils::WHITE_() }));
    }
}

void FileDialogOpen(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData) {
    if (pointerData.state != CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        return;
    }
    std::printf("Open file dialog!\n");
}

void DrawHexView(const AppState& appState) {
  Clay_TextElementConfig panelTitleTextConfig = TextUtils::Default(14);
  CLAY({
      .id = CLAY_ID("LeftPanel"),
      .layout =
          {
              .padding = {.left = 8, .right = 8, .top = 8, .bottom = 8},
              .sizing = {.width = CLAY_SIZING_GROW(),
                         .height = CLAY_SIZING_GROW()},
          },
      .backgroundColor = PANEL_COLOR,
      .border = {.color = BACKGROUND_COLOR, .width = CLAY_BORDER_OUTSIDE(1)},
  }) {
    CLAY_TEXT(CLAY_STRING("Hex View"), &panelTitleTextConfig);
    CLAY({ .id = CLAY_ID("PanelSpace"), .layout = { .sizing = LayoutUtils::SizeAutoGrowXY(), .childAlignment = LayoutUtils::ChildAlignCenterAll()}}) {
        // We have no open file, so we need to prompt the user to open one
        if (appState.currentFile.handle == 0) {
            Buttons::ButtonArgs openFileBtn {};
            // openFileBtn.fgHoverColor = ColorUtils::SUCCESS();
            openFileBtn.fontSize = 24;
            openFileBtn.onHover = &::FileDialogOpen;
            Buttons::RawButton(CLAY_STRING("Open a file to start!"), openFileBtn);
            return;
        }

        // Draw the contents of the file
        
    }
  }
}

void DrawInterpretedView(const AppState& appState) {
    if (appState.currentFile.handle == 0) {
        // Skip drawing if no file is present, we let the left panel handle the prompt to the user
        return;
    }
    CLAY({
        .id = CLAY_ID("RightPanel"),
        .layout = {
            .padding = { .left = 8, .right = 8, .top = 8, .bottom = 8 },
            .sizing = { .width = CLAY_SIZING_GROW(), .height = CLAY_SIZING_GROW() },
        },
        .backgroundColor = PANEL_COLOR,
        .border = { .color = BACKGROUND_COLOR, .width = CLAY_BORDER_OUTSIDE(1) },
    }) {
        CLAY_TEXT(CLAY_STRING("Interpreted View"),
            CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = ColorUtils::WHITE_() }));
    }
}

void Application::BuildUI() {
    constexpr Clay_TextElementConfig titleConfig = TextUtils::Default(24);
    CLAY({
        .id = CLAY_ID("Root"),
        .layout = GrowingLayout(CLAY_TOP_TO_BOTTOM, 4, 4)
    }) {
        DrawHeader();
        CLAY({
            .id = CLAY_ID("MainArea"),
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .childGap = 4,
                .sizing = { .width = CLAY_SIZING_GROW(), .height = CLAY_SIZING_GROW() },
            },
        }) {
            DrawHexView(this->m_appState);

            DrawInterpretedView(this->m_appState);
        }

        CLAY({
            .id = CLAY_ID("MetadataPanel"),
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .padding = { .left = 8, .right = 8, .top = 8, .bottom = 8 },
                .childGap = 16,
                .sizing = { .height = CLAY_SIZING_FIXED(48), .width = CLAY_SIZING_GROW(0) },
            },
            .backgroundColor = PANEL_COLOR,
            .border = { .color = BACKGROUND_COLOR, .width = CLAY_BORDER_OUTSIDE(1) },
        }) {
            CLAY_TEXT(CLAY_STRING("Offset: --"),
                CLAY_TEXT_CONFIG({ .fontSize = 13, .textColor = ColorUtils::WHITE_() }));
            CLAY_TEXT(CLAY_STRING("Size: --"),
                CLAY_TEXT_CONFIG({ .fontSize = 13, .textColor = ColorUtils::WHITE_() }));
            CLAY_TEXT(CLAY_STRING("Type: --"),
                CLAY_TEXT_CONFIG({ .fontSize = 13, .textColor = ColorUtils::WHITE_() }));
            CLAY_TEXT(CLAY_STRING("Selection: none"),
                CLAY_TEXT_CONFIG({ .fontSize = 13, .textColor = ColorUtils::WHITE_() }));
        }
    }
}

void Application::Dispose() {
    CloseWindow();
    FileLayer::CloseFile(&this->m_appState.currentFile);
    free(this->m_arena.memory);

}
