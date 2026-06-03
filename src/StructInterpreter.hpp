#pragma once
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>

constexpr size_t MAX_NAME_LENGTH = 32;
constexpr size_t MAX_STRUCT_FIELDS = 16;

template <size_t N>
class FixedCapacityStr {
public:

    FixedCapacityStr() = default;

    FixedCapacityStr(char8_t* data, size_t length) {
        assert(data != nullptr && "Cannot copy a null string into a fixed capacity string!");
        assert(length < N && "Unable to create a string of a length bigger than its fixed capacity!");
        std::memcpy(this->Ptr(), data, length);
        this->m_length = length;
    }

    inline char8_t* Ptr() {
        return &this->m_data[0];
    }

    inline char8_t* CStr() const {
        // Null terminate
        this->m_data[this->m_length] = 0;
        return &this->m_data[0];
    }

    inline bool Empty() const {
        return this->m_length == 0;
    }

    bool operator ==(const FixedCapacityStr& other) {
        return other.m_length == this->m_length && std::strncmp(other.CStr(), this->CStr(), this->m_length) == 0;
    }
    
private:
    char8_t m_data[N] = {0};
    size_t m_length = 0;
};

// Known limitations, arrays of complex types might not be able to be parsed for the moment

// Flashbacks from the C Parser lol
enum class EType : uint8_t {
      I8,
      I16,
      I32,
      I64,
      U8,
      U16,
      U32,
      U64,
      F32,
      F64,
      B8
};

enum class ETypeFlags : uint8_t {
    None = 0,
    DisplayIgnore = 1
};

struct TypeInfo {
    ETypeFlags flags;
    EType type;
    // Only applicable to arrays
    uint16_t elementCount = 0;

};

struct StructDescriptor {
    // How big the struct is in bytes
    size_t size;
    // What its name is
    FixedCapacityStr<MAX_NAME_LENGTH> name;
    // What each field represents
    TypeInfo fields[MAX_STRUCT_FIELDS];
    FixedCapacityStr<16> fieldNames[MAX_STRUCT_FIELDS];
};


