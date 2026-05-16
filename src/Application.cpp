#include "Application.hpp"

#include "AppState.hpp"
#include "ColorUtils.hpp"
#include "FileLayer.hpp"
#include "LayoutUtils.hpp"
#include "MemoryArena.hpp"
#include "Result.hpp"
#include "TextUtils.hpp"
#include "components/Buttons.hpp"
#include "raylib.h"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <string>
extern "C" {
#include "renderer/clay_renderer_raylib.h"
}
#include <cstdio>
#include <cstdlib>


constexpr Clay_Color PANEL_COLOR = ColorUtils::PANEL_BG();
constexpr Clay_Color BACKGROUND_COLOR = ColorUtils::BORDER();

constexpr size_t SMALL_STR_ARENA_SIZE = 1024 * 10;
MemoryArena smallStringArena;

void Application::Run() {
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        Update(dt);
        Draw();
    }
}


void Application::HandleInput() {
    if (this->m_appState.currentFile.handle == 0) {
        return;
    }

    int64_t lowerLimit = 0;
    int64_t lowerLimitRange = 0;

    if (this->m_appState.selectedByteIdx <= -1) {
        lowerLimit = -1;
    }
    if (this->m_appState.selectedByteRangeEnd <= -1) {
        lowerLimitRange = -1;
    }

    int64_t originalByteIdx = this->m_appState.selectedByteIdx;

    int64_t isMultiSelectFactor = static_cast<int64_t>(IsKeyDown(KEY_LEFT_SHIFT));
    if (isMultiSelectFactor && this->m_appState.selectedByteRangeEnd <= -1) {
        this->m_appState.selectedByteRangeEnd = this->m_appState.selectedByteIdx;
    }

    if (IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT)) {
        this->m_appState.selectedByteIdx -= (1 * (1 - isMultiSelectFactor));
        this->m_appState.selectedByteRangeEnd -= (1 * (isMultiSelectFactor));

    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT)) {
        this->m_appState.selectedByteIdx += (1 * (1 - isMultiSelectFactor));
        this->m_appState.selectedByteRangeEnd += (1 * isMultiSelectFactor);
    }
    if (IsKeyPressed(KEY_UP) || IsKeyPressedRepeat(KEY_UP)) {
        this->m_appState.selectedByteIdx -= this->m_appState.buttonsPerCurrentWidth * (1 - isMultiSelectFactor);
        this->m_appState.selectedByteRangeEnd -= this->m_appState.buttonsPerCurrentWidth * (isMultiSelectFactor);
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressedRepeat(KEY_DOWN)) {
        this->m_appState.selectedByteIdx += this->m_appState.buttonsPerCurrentWidth * (1 - isMultiSelectFactor);
        this->m_appState.selectedByteRangeEnd += this->m_appState.buttonsPerCurrentWidth * (isMultiSelectFactor);
    }


    // Handle deselecting if we stopped pressing shift and kept moving
    if (this->m_appState.selectedByteRangeEnd != -1 && this->m_appState.selectedByteIdx != originalByteIdx && !isMultiSelectFactor) {
        // Switch our selected index for the last byte we actually selected on that direction
        this->m_appState.selectedByteIdx = this->m_appState.selectedByteRangeEnd;
        this->m_appState.selectedByteRangeEnd = -1;
    }
    else {
        this->m_appState.selectedByteRangeEnd = std::clamp<int64_t>(this->m_appState.selectedByteRangeEnd, lowerLimitRange, this->m_appState.currentFile.size - 1);
    }

    this->m_appState.selectedByteIdx = std::clamp<int64_t>(this->m_appState.selectedByteIdx, lowerLimit, this->m_appState.currentFile.size - 1);
    
}

void Application::Init() {
    uint32_t memorySize = Clay_MinMemorySize();
    void* memory = std::malloc(memorySize);
    smallStringArena.Init(SMALL_STR_ARENA_SIZE);

    this->m_arena = Clay_CreateArenaWithCapacityAndMemory(memorySize, memory);
    Clay_Initialize(this->m_arena, { .width = 1280, .height = 800}, {});
    Clay_Raylib_Initialize(1280, 800, "HexViewer", FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);

    fonts[0] = LoadFontEx("assets/fonts/Nova_Square/NovaSquare-Regular.ttf", 72, NULL, 0);
    if (fonts[0].texture.id == 0) fonts[0] = GetFontDefault();

    Font* userDataPtr = &(fonts[0]);
    Clay_SetMeasureTextFunction(Raylib_MeasureText, userDataPtr);
    // Clay_SetDebugModeEnabled(true);
    initialized = true;
}

void Application::Update(float dt) {
    smallStringArena.Clear();
    Clay_SetLayoutDimensions({
        .width = (float)GetScreenWidth(),
        .height = (float)GetScreenHeight(),
    });
    Clay_SetPointerState(
        { .x = (float)GetMouseX(), .y = (float)GetMouseY() },
        IsMouseButtonDown(MOUSE_BUTTON_LEFT)
    );
    Vector2 mouseWheelV = GetMouseWheelMoveV();
    Clay_UpdateScrollContainers(false, { .x = mouseWheelV.x, .y = mouseWheelV.y }, dt);
    this->HandleInput();
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
    Vector2 windowSize = {(float)GetScreenWidth(), (float)GetScreenHeight()};
    Clay_SetLayoutDimensions({.width = windowSize.x, .height = windowSize.y});

    BeginDrawing();

    ClearBackground({ 255, 255, 255, 255 });

    Clay_BeginLayout();

    BuildUI(&this->m_appState);

    Clay_RenderCommandArray cmds = Clay_EndLayout();
    Clay_Raylib_Render(cmds, fonts);

    EndDrawing();
}


Clay_String StrToClayString(const char* data, size_t size) noexcept {
    return Clay_String{ false, static_cast<int32_t>(size), data};
}

Clay_LayoutConfig GrowingLayout(Clay_LayoutDirection dir, uint16_t gap, uint16_t pad) {
    return {
        .sizing = { .width = CLAY_SIZING_GROW(), .height = CLAY_SIZING_GROW() },
        .padding = { .left = pad, .right = pad, .top = pad, .bottom = pad },
        .childGap = gap,
        .layoutDirection = dir,
    };
}

void DrawHeader(const AppState& appState) {
    CLAY({
        .id = CLAY_ID("HeaderBar"),
        .layout = {
            .padding = { .left = 8, .right = 8, .top = 8, .bottom = 8 },
            .sizing = { .height = CLAY_SIZING_FIXED(36), .width = CLAY_SIZING_GROW(0) },
        },
        .backgroundColor = PANEL_COLOR,
        .border = { .color = ColorUtils::BLACK_(), .width = { 0, 0, 1, 0, 0 } },
    }) {
        Clay_String title = appState.currentFile.handle == 0 ? CLAY_STRING("HexViewer") : StrToClayString(appState.currentFile.name, appState.currentFile.nameLength);
        CLAY_TEXT(title,
            CLAY_TEXT_CONFIG({ .fontSize = 24, .textColor = ColorUtils::WHITE_() }));
    }
}

void FileDialogOpen(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData) {
    if (pointerData.state != CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        return;
    }
    std::printf("Open file dialog!\n");
}

bool IsIndexInSelectionRange(size_t idx, int64_t rangeA, int64_t rangeB) {
    if (rangeB <= -1) {
        return idx == rangeA;
    }
    bool couldLowerA = idx <= rangeA && idx >= rangeB;
    bool couldLowerB = idx <= rangeB && idx >= rangeA;
    return couldLowerA || couldLowerB;
}

void DrawButtonForView(size_t idx, const char* text, size_t length, AppState* state) {
    Clay_String str = StrToClayString(text, length);
    Buttons::ButtonArgs button;
    if (IsIndexInSelectionRange(idx, state->selectedByteIdx, state->selectedByteRangeEnd)) {
        // Override colors because we are selected
        button.fgHoverColor = ColorUtils::ACCENT_PURPLE();
        button.fgIdleColor = ColorUtils::ACCENT_PURPLE();
        button.bgHoverColor = ColorUtils::DUSTY_WHITE(200);
        button.bgIdleColor = ColorUtils::DUSTY_WHITE(200);
    }
    else {
        // Just do on hover
        button.fgHoverColor = ColorUtils::ACCENT_PURPLE();
        button.bgHoverColor = ColorUtils::SOFT_BLACK();
    }
    button.active = state->hoverByteIdx == idx;
    button.fontSize = 24;
    button.sizing = {
        .height = CLAY_SIZING_FIXED(48),
        .width = CLAY_SIZING_FIXED(48)
    };
    if (Buttons::RawButton(str, button)) {
        state->hoverByteIdx = idx;
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            // Disable multi select if needed
            if (!IsKeyPressed(KEY_LEFT_SHIFT)) state->selectedByteRangeEnd = -1;
            state->selectedByteIdx = idx;
        }
    }
}

void DrawByte(size_t idx, uint8_t byte, AppState* state) {
    Clay_LayoutConfig layout = {
        .layoutDirection = CLAY_LEFT_TO_RIGHT,
        .childGap = 4,
    };

    const char hexChars[] = "0123456789ABCDEF";
    char* hexRepresentation = smallStringArena.PushManyAndZeroOutNonAligned<char>(3);
    CLAY({
        .id = CLAY_IDI("Byte", idx),
        .layout = layout
    }) {
        // Transform into hex and draw that
        hexRepresentation[0] = hexChars[(byte >> 4) & 0x0F];
        hexRepresentation[1] = hexChars[byte & 0x0F];
        DrawButtonForView(idx, hexRepresentation, 3, state);
    }
}

void DrawBytesLineByLine(AppState* appState) {
    Clay_ElementData panelData = Clay_GetElementData(CLAY_ID("LeftPanel"));
    float availableWidth = panelData.boundingBox.width;
    constexpr float BUTTON_WIDTH = 48;

    // Calculate how many buttons per line
    int32_t buttonsPerWidth = static_cast<int32_t>(availableWidth / BUTTON_WIDTH);
    appState->buttonsPerCurrentWidth = buttonsPerWidth;

    size_t buttonsToWrite = appState->currentFile.size;

    int32_t additionalLinesToWrite = ceil(buttonsToWrite / (float)buttonsPerWidth);

    // TODO: Identify how many buttons there actually are before the clip, we only draw those and keep our memory footprint constant

    CLAY({ .id = CLAY_ID("ByteRows"), .clip = { .vertical = true, .childOffset = Clay_GetScrollOffset() },  .layout = { .layoutDirection = CLAY_TOP_TO_BOTTOM }}) {
        // Then the for loop iterates through the buttons per line
        for (int32_t i = 0; i < additionalLinesToWrite; i ++) {
            CLAY({ .id = CLAY_IDI("PanelSpace", i), .layout = { .sizing = { .width = CLAY_SIZING_FIT() }, .padding = { .top = 0, .left = 4, .bottom = 0, .right = 4 }}}) {
                // Draw the contents of the file
                size_t offset = (buttonsPerWidth * i);
                for (size_t j = offset; j < (offset + buttonsPerWidth)  && j < buttonsToWrite; j++) {
                    uint8_t b = *(appState->binaryContentBuffer + j);
                    DrawByte(j, b, appState);
                }
            }
        }
    }
}


void DrawCharsLineByLine(AppState* appState) {
    Clay_ElementData panelData = Clay_GetElementData(CLAY_ID("LeftPanel"));
    float availableWidth = panelData.boundingBox.width;
    constexpr float BUTTON_WIDTH = 48;

    // Calculate how many buttons per line
    int32_t buttonsPerWidth = static_cast<int32_t>(availableWidth / BUTTON_WIDTH);

    size_t buttonsToWrite = appState->currentFile.size;

    int32_t additionalLinesToWrite = ceil(buttonsToWrite / (float)buttonsPerWidth);

    char* data = (char*)appState->binaryContentBuffer;

    CLAY({ .id = CLAY_ID("InterpretRows"), .layout = { .layoutDirection = CLAY_TOP_TO_BOTTOM }, .clip = { .vertical = true, .childOffset = Clay_GetScrollOffset() }}) {
        // Then the for loop iterates through the buttons per line
        for (int32_t i = 0; i < additionalLinesToWrite; i ++) {
            CLAY({ .id = CLAY_IDI("IPanelSpace", i), .layout = { .sizing = { .width = CLAY_SIZING_FIT() }, .padding = { .top = 0, .left = 4, .bottom = 0, .right = 4 }}}) {
                // Draw the contents of the file
                size_t offset = (buttonsPerWidth * i);
                for (size_t j = offset; j < (offset + buttonsPerWidth)  && j < buttonsToWrite; j++) {
                    DrawButtonForView(j, data + j, 1, appState);
                }
            }
        }
        // TODO: Draw the remaining ones
    }

}


void DrawHexView(AppState* appState) {
  Clay_TextElementConfig panelTitleTextConfig = TextUtils::Default(24);
  float percentageUse = appState->currentFile.handle == 0 ? 1 : 0.5;
  CLAY({
      .id = CLAY_ID("LeftPanel"),
      .layout =
        {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .padding = {.left = 8, .right = 8, .top = 8, .bottom = 8},
            .sizing = {.width = CLAY_SIZING_FIXED(GetScreenWidth() * percentageUse), .height = CLAY_SIZING_GROW()},
        },
      .backgroundColor = PANEL_COLOR,
      .border = {.color = BACKGROUND_COLOR, .width = CLAY_BORDER_OUTSIDE(1)},
  }) {
    CLAY_TEXT(CLAY_STRING("Hex View"), CLAY_TEXT_CONFIG(panelTitleTextConfig));
    if (appState->currentFile.handle == 0) {
        CLAY({ .id = CLAY_ID("PanelSpace"), .layout = { .sizing = LayoutUtils::SizeAutoGrowXY(), .childAlignment = LayoutUtils::ChildAlignCenterAll()}}) {
            // We have no open file, so we need to prompt the user to open one
            Buttons::ButtonArgs openFileBtn {};
            // openFileBtn.fgHoverColor = ColorUtils::SUCCESS();
            openFileBtn.fontSize = 24;
            // openFileBtn.onHover = &::FileDialogOpen;
            Buttons::RawButton(CLAY_STRING("Open a file to start!"), openFileBtn);
        }
    }
    else {
        DrawBytesLineByLine(appState);
    }
  }
}

void DrawInterpretedView(AppState* appState) {
    if (appState->currentFile.handle == 0) {
        // Skip drawing if no file is present, we let the left panel handle the prompt to the user
        return;
    }
    CLAY({
        .id = CLAY_ID("RightPanel"),
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            // .padding = { .left = 8, .right = 8, .top = 8, .bottom = 8 },
            .sizing = { .width = CLAY_SIZING_FIXED(GetScreenWidth() * 0.5f), .height = CLAY_SIZING_GROW()},
        },
        .backgroundColor = PANEL_COLOR,
        .border = { .color = BACKGROUND_COLOR, .width = CLAY_BORDER_OUTSIDE(1) },
    }) {
        CLAY_TEXT(CLAY_STRING("Interpreted View"),
            CLAY_TEXT_CONFIG({ .fontSize = 24, .textColor = ColorUtils::WHITE_() }));

        
        CLAY({ .id = CLAY_ID("InterpretedPanelSpace"), .layout = { .sizing = { .width = CLAY_SIZING_PERCENT(1)}, .padding = { .top = 4, .left = 4, .bottom = 4, .right = 4 }}}) {
            Clay_LayoutConfig layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .childGap = 4,
            };
            // Draw the contents of the file
            DrawCharsLineByLine(appState);
        }

    }
}

void Application::BuildUI(AppState* appState) {
    Clay_TextElementConfig titleConfig = TextUtils::Default(24);
    CLAY({
        .id = CLAY_ID("Root"),
        .layout = GrowingLayout(CLAY_TOP_TO_BOTTOM, 4, 4)
    }) {
        DrawHeader(this->m_appState);
        CLAY({
            .id = CLAY_ID("MainArea"),
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .childGap = 4,
                .sizing = { .width = CLAY_SIZING_GROW(), .height = CLAY_SIZING_GROW() },
            },
        }) {
            DrawHexView(&this->m_appState);

            DrawInterpretedView(&this->m_appState);
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
            constexpr size_t STR_MAX_LENGTH = 32;
            char* offsetStr = smallStringArena.PushManyAndZeroOut<char>(STR_MAX_LENGTH);
            snprintf(offsetStr, STR_MAX_LENGTH, "Offset (Hover position): %lld", appState->hoverByteIdx);
            CLAY_TEXT(StrToClayString(offsetStr, STR_MAX_LENGTH),
                CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = ColorUtils::WHITE_() }));

            char* selectedStr = smallStringArena.PushManyAndZeroOut<char>(STR_MAX_LENGTH);
            // This is a bit awful lol
            int64_t byteDistance = abs(appState->selectedByteIdx - appState->selectedByteRangeEnd) + 1;

            if (appState->selectedByteIdx >= 0 && appState->selectedByteRangeEnd != -1) {
                snprintf(selectedStr, STR_MAX_LENGTH, "Selected bytes: %lld", byteDistance);
                CLAY_TEXT(StrToClayString(selectedStr, STR_MAX_LENGTH),
                    CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = ColorUtils::WHITE_() }));
            }

            // CLAY_TEXT(CLAY_STRING("Size: --"),
            //     CLAY_TEXT_CONFIG({ .fontSize = 13, .textColor = ColorUtils::WHITE_() }));
            // CLAY_TEXT(CLAY_STRING("Type: --"),
            //     CLAY_TEXT_CONFIG({ .fontSize = 13, .textColor = ColorUtils::WHITE_() }));
            // CLAY_TEXT(CLAY_STRING("Selection: none"),
            //     CLAY_TEXT_CONFIG({ .fontSize = 13, .textColor = ColorUtils::WHITE_() }));
        }
    }
}

void Application::Dispose() {
    CloseWindow();
    FileLayer::CloseFile(&this->m_appState.currentFile);
    free(this->m_arena.memory);

}
