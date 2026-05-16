#pragma once

#include <atomic>
#include <cassert>
#include <cstdint>
#include <algorithm>
#include <cstring>

class MemoryArena;
using ArenaResizeFunction_t = void(*)(MemoryArena*, size_t);

class MemoryArena {
public:
	
	~MemoryArena() {
		if (this->m_memory != nullptr) {
			free(this->m_memory);
			this->m_memory = nullptr;
		}
	}
	
	void Init(size_t allocSize) {
		this->m_capacity = allocSize;
		this->m_memory = static_cast<uint8_t*>(malloc(allocSize));
		this->m_position = 0;
	}

	uint8_t* PushFast(size_t size) {
		assert(this->m_memory != nullptr && "Uninitialized memory, call init() with the arena size to allocate memory first");
		size_t positionAligned = this->AlignToPow2(this->m_position);
		this->m_position = positionAligned + size;
		// ERR_FAIL_COND_V(this->m_position > this->m_capacity, nullptr);
		if (this->m_position > this->m_capacity) {
			// ERR_PRINT("Arena allocation request size exceeded defined arena buffer size!");
			abort();
		}
		uint8_t* res = &this->m_memory[this->m_position];
		return res;
	}

	uint8_t* PushAndZeroOut(size_t size) {
		assert(this->m_memory != nullptr && "Uninitialized memory, call init() with the arena size to allocate memory first");
		size_t positionAligned = this->AlignToPow2(this->m_position);
		this->m_position = positionAligned + size;
		// ERR_FAIL_COND_V(this->m_position > this->m_capacity, nullptr);
		if (this->m_position > this->m_capacity) {
			// ERR_PRINT("Arena allocation request size exceeded defined arena buffer size!");
			abort();
		}
		uint8_t* res = &this->m_memory[this->m_position];
		// TODO: optionally do memset to zero
		std::memset(res, 0, size);
		return res;
	}

	uint8_t* PushAndZeroOutNonAligned(size_t size) {
		assert(this->m_memory != nullptr && "Uninitialized memory, call init() with the arena size to allocate memory first");
		this->m_position = this->m_position + size;
		// ERR_FAIL_COND_V(this->m_position > this->m_capacity, nullptr);
		if (this->m_position > this->m_capacity) {
			// ERR_PRINT("Arena allocation request size exceeded defined arena buffer size!");
			abort();
		}
		uint8_t* res = &this->m_memory[this->m_position];
		// TODO: optionally do memset to zero
		std::memset(res, 0, size);
		return res;
	}
	
	template<class T>
	T* PushFast() {
		return reinterpret_cast<T*>(this->PushFast(sizeof(T)));
	}

	template<class T>
	T* PushAndZeroOut() {
		return reinterpret_cast<T*>(this->PushAndZeroOut(sizeof(T)));
	}

	template<class T>
	T* PushManyFast(size_t count) {
		return reinterpret_cast<T*>(this->PushFast(sizeof(T) * count));
	}

	template<class T>
	T* PushManyAndZeroOut(size_t count) {
		return reinterpret_cast<T*>(this->PushAndZeroOut(sizeof(T) * count));
	}

	template<class T>
	T* PushManyAndZeroOutNonAligned(size_t count) {
		return reinterpret_cast<T*>(this->PushAndZeroOutNonAligned(sizeof(T) * count));
	}

	template<class T>
	void Pop() {
		size_t clampedSize = std::min(sizeof(T), this->m_position.load(std::memory_order_relaxed));
		this->m_position -= clampedSize;
	}

	void Pop(size_t size) {
		size_t clampedSize = std::min(size, this->m_position.load(std::memory_order_relaxed));
		this->m_position -= clampedSize;
	}

	void Clear() {
		this->m_position = 0;
	}
	

private:
	constexpr size_t AlignToPow2 (size_t position) {
		constexpr size_t BYTE_ALIGNMENT = sizeof(uint8_t*);
		return ((position) + ((BYTE_ALIGNMENT) - 1)) & (~((BYTE_ALIGNMENT) - 1));
	}

	std::atomic<size_t> m_capacity = 0;
	std::atomic<size_t> m_position = 0;
	uint8_t* m_memory = nullptr;
	ArenaResizeFunction_t m_resizeFn;

};

