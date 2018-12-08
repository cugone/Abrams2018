#pragma once
//https://www.youtube.com/watch?v=e2ZQyYr0Oi0
//C++17 - The Best Features - Nicolai Josuttis [ACCU 2018]

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Profiling/StackTrace.hpp"

#include <new>

class Memory {
public:
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
    static void trace(bool do_trace) {
        doTrace = do_trace;
    }
    static void* allocate(std::size_t size) {
        if(active) {
            ++frameCount;
            frameSize += size;
            ++allocCount;
            allocSize += size;
            if(maxSize < allocSize) {
                maxSize = allocSize;
            }
            if(maxCount < allocCount) {
                maxCount = allocCount;
            }
        }
        return std::malloc(size);
    }
    static void deallocate(void* ptr, std::size_t size) {
        if(allocCount) {
            --allocCount;
            allocSize -= size;
        }
        std::free(ptr);
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
    inline static bool doTrace = false;
};

#ifdef TRACK_MEMORY

void* operator new(std::size_t size);
void operator delete(void* ptr, std::size_t size);

#endif