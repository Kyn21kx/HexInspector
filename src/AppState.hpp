#pragma once

#include "ColorUtils.hpp"
#include "FileLayer.hpp"

constexpr size_t MAX_ERR_DIAGNOSTIC_LENGTH = 1024;

struct AppState {
    File currentFile;
    // TODO: an error queue
    uint8_t* binaryContentBuffer; // This is either a mmap'd address or given by the memory arena
    int64_t hoverByteIdx = -1;
    int64_t selectedByteIdx = -1;
    int64_t selectedByteRangeEnd = -1;
    int32_t buttonsPerCurrentWidth = 0;
    Clay_Vector2 scrollOffset;
};

