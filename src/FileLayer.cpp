#include "FileLayer.hpp"
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>


#include <string_view>
#ifdef _WIN32
#pragma warning(disable : 4996)
#include <Windows.h>
#include <memoryapi.h>
#include <fileapi.h>
#include <handleapi.h>
#else
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#include <stdio.h>

bool MemoryMapOpen(File* outFile, std::string_view path) {
    #ifdef _WIN32
    HANDLE fileHandle;

    fileHandle = CreateFile(path.data(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, 0);

    outFile->handle = reinterpret_cast<uintptr_t>(fileHandle); 
    return fileHandle != INVALID_HANDLE_VALUE;
    #else
    // Use the handle to store file descriptor
    int32_t descriptor = open(path.data(), O_RDONLY);
    outFile->handle = descriptor;
    return descriptor > 0;
    #endif
}

Result<File, FileLayer::EError> FileLayer::OpenFile(std::string_view path, EFileFlags flags) {
    if (!std::filesystem::exists(path)) {
      return EError::FileNotFound;
    }
    File res {
        .flags = flags
    };

    res.size = std::filesystem::file_size(path);
    std::string fn = std::filesystem::path(path).filename().string();
    std::memcpy(res.name, fn.data(), fn.size());
    res.nameLength = fn.size();

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


Result<uint8_t*, FileLayer::EError> FileLayer::ReadIntoBuffer(uint8_t* buffer, size_t size, File& file) {
    if (((uint8_t)file.flags & (uint8_t)EFileFlags::MemoryMapped) != 0) {
    #ifdef _WIN32
        HANDLE mappedHandle = CreateFileMapping((HANDLE)file.handle, NULL, PAGE_READONLY, 0, 0, nullptr);
        if (mappedHandle == INVALID_HANDLE_VALUE) {
            return EError::FailedToMapFile;
        }
        file.mapHandle = reinterpret_cast<uintptr_t>(mappedHandle);
        uint8_t* mappedAddress = (uint8_t*)MapViewOfFile(mappedHandle, FILE_MAP_READ, 0, 0, 0);
        if (mappedAddress == nullptr) {
            CloseHandle(mappedHandle);
            CloseHandle((HANDLE)file.handle);
            return EError::FailedToMapFile;
        }
        return mappedAddress;
    #else
        uint8_t* mappedAddress = (uint8_t*)mmap(nullptr, file.size, PROT_READ, MAP_PRIVATE, (int32_t)file.handle, 0);
        return mappedAddress;
    #endif
    }
    return EError::NotYetImplemented;    
}

void FileLayer::CloseFile(File* file) {
    assert(file != nullptr && "Cannot close a null file");
    if (((uint8_t)file->flags & (uint8_t)EFileFlags::MemoryMapped) != 0) {
        #ifdef _WIN32
        CloseHandle((HANDLE)file->mapHandle);
        CloseHandle((HANDLE)file->handle);
        #else
        close(static_cast<int32_t>(file->handle));
        #endif
        return;
    }
    fclose(reinterpret_cast<FILE*>(file->handle));
}
