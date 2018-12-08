#pragma once
//https://www.youtube.com/watch?v=e2ZQyYr0Oi0
//C++17 - The Best Features - Nicolai Josuttis [ACCU 2018]

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include <new>

class Memory {
public:
    static void enable(bool e);
    static bool is_enabled();
    static void resetframecounters();
    static void* allocate(std::size_t size);
    static void deallocate(void* ptr);
    static void deallocate(void* ptr, std::size_t size);
    inline static std::size_t maxSize = 0;
    inline static std::size_t maxCount = 0;
    inline static std::size_t allocSize = 0;
    inline static std::size_t allocCount = 0;
    inline static std::size_t freeSize = 0;
    inline static std::size_t freeCount = 0;
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