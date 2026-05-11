#pragma once

#include "FileLayer.hpp"

constexpr size_t MAX_ERR_DIAGNOSTIC_LENGTH = 1024;

struct AppState {
    File currentFile;
    // TODO: an error queue
    uint8_t* binaryContentBuffer; // This is either a mmap'd address or given by the memory arena
};

