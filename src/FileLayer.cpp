#include "FileLayer.hpp"
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <filesystem>


#include <string_view>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

bool MemoryMapOpen(File* outFile, std::string_view path) {
    // Use the handle to store file descriptor
    int32_t descriptor = open(path.data(), O_RDONLY);
    outFile->handle = descriptor;
    return descriptor > 0;
}

Result<File, FileLayer::EError> FileLayer::OpenFile(std::string_view path, EFileFlags flags) {
    if (!std::filesystem::exists(path)) {
      return EError::FileNotFound;
    }
    File res {
        .flags = flags
    };

    res.size = std::filesystem::file_size(path);

    if (((uint8_t)flags & (uint8_t)EFileFlags::MemoryMapped) != 0) {
        if (MemoryMapOpen(&res, path)) return res;
        return EError::CantOpen;
    }

    FILE* file = fopen(path.data(), "rb");
    if (file == nullptr) {
      return EError::CantOpen;
    }

    return res;
}


Result<uint8_t*, FileLayer::EError> FileLayer::ReadIntoBuffer(uint8_t* buffer, size_t size, const File& file) {
    if (((uint8_t)file.flags & (uint8_t)EFileFlags::MemoryMapped) != 0) {
        uint8_t* mappedAddreess = (uint8_t*)mmap(nullptr, file.size, PROT_READ, MAP_PRIVATE, (int32_t)file.handle, 0);
        return mappedAddreess;
    }
    return EError::NotYetImplemented;    
}

void FileLayer::CloseFile(File* file) {
    assert(file != nullptr && "Cannot close a null file");
    if (((uint8_t)file->flags & (uint8_t)EFileFlags::MemoryMapped) != 0) {
        close(static_cast<int32_t>(file->handle));
        return;
    }
    fclose(reinterpret_cast<FILE*>(file->handle));
}
