#pragma once

#include "Result.hpp"
#include <string_view>


enum class EFileFlags : uint8_t {
    None = 0,
    MemoryMapped = 1
};

struct File {
    uintptr_t handle = 0;
    size_t size = 0;
    EFileFlags flags = EFileFlags::None;
};

namespace FileLayer {
    enum class EError {
        Ok = 0,
        FileNotFound,
        CantOpen,
        NotYetImplemented
    };

    Result<File, EError> OpenFile(std::string_view path, EFileFlags flags = EFileFlags::None);

    Result<uint8_t*, EError> ReadIntoBuffer(uint8_t* buffer, size_t size, const File& file);

    void CloseFile(File* file);
    
};

