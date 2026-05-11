#pragma once

#include <type_traits>
template<class T, class E>
class Result {
public:
    Result() {
        
    }
    Result(T&& value) {
        T* ptr = reinterpret_cast<T*>(&this->m_value);
        *ptr = value;
        this->m_hasValue = true;
    }

    Result(E&& err) {
        this->m_error = err;
        this->m_hasValue = false;
    }

    operator bool() const {
        return this->m_hasValue;
    }

    const T& Val() const {
        const T* val = reinterpret_cast<const T*>(&this->m_value);
        return *val;
    }
    

private:
    std::aligned_storage<sizeof(T), alignof(T)> m_value{};
    E m_error{};
    bool m_hasValue = false;
};

