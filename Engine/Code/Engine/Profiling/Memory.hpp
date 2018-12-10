#pragma once
//https://www.youtube.com/watch?v=e2ZQyYr0Oi0
//C++17 - The Best Features - Nicolai Josuttis [ACCU 2018]

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include <new>

class Memory {
public:

    struct block_t {
        std::size_t size = 0;
        void* ptr = nullptr;
    };
    struct status_t {
        std::size_t leaked_objs  = 0;
        std::size_t leaked_bytes = 0;
        operator bool() {
            return leaked_objs || leaked_bytes;
        }
    };
    struct status_frame_t {
        std::size_t leaked_objs = 0;
        std::size_t leaked_bytes = 0;
        operator bool() {
            return leaked_objs || leaked_bytes;
        }
    };

    [[nodiscard]] static void* allocate(std::size_t n) {
        auto b_size = sizeof(Memory::block_t);
        auto size = n + b_size;
        auto b = reinterpret_cast<Memory::block_t*>(std::malloc(size));
        if(!b) {
            return nullptr;
        }
        b->size = n;
        b->ptr = reinterpret_cast<void*>(b + 1);
        if(active) {
            ++frameCount;
            frameSize += b->size;
            ++allocCount;
            allocSize += b->size;
            if(maxSize < allocSize) {
                maxSize = allocSize;
            }
            if(maxCount < allocCount) {
                maxCount = allocCount;
            }
        }
        return b->ptr;
    }

    static void deallocate(void* ptr, std::size_t /*size*/ = 0) noexcept {
        if(ptr) {
            auto b = reinterpret_cast<Memory::block_t*>(ptr) - 1;
            allocSize -= b->size;
            --allocCount;
            std::free(b);
        }
    }

    static void enable(bool e) {
        active = e;
    }

    static bool is_enabled() {
        return active;
    }

    static void resetframecounters() {
        frameSize = 0;
        frameCount = 0;
    }

    static status_t status() {
        return { allocCount, allocSize };
    }

    static status_frame_t frame_status() {
        return { frameCount, frameSize };
    }

    inline static std::size_t maxSize = 0;
    inline static std::size_t maxCount = 0;
    inline static std::size_t allocSize = 0;
    inline static std::size_t allocCount = 0;
    inline static std::size_t frameSize = 0;
    inline static std::size_t frameCount = 0;
protected:
private:
    inline static bool active = false;
};

#ifdef TRACK_MEMORY

void* operator new(std::size_t size);
void* operator new[](std::size_t size);
void operator delete(void* ptr);
void operator delete(void* ptr, std::size_t size);
void operator delete[](void* ptr);
void operator delete[](void* ptr, std::size_t size);

#endif
