#pragma once
//https://www.youtube.com/watch?v=e2ZQyYr0Oi0
//C++17 - The Best Features - Nicolai Josuttis [ACCU 2018]

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include <new>
#include <sstream>

class Memory {
public:

    struct status_t {
        std::size_t leaked_objs  = 0;
        std::size_t leaked_bytes = 0;
        operator bool() {
            return leaked_objs || leaked_bytes;
        }
        operator std::string() {
            std::ostringstream ss;
            ss << "Leaked objects: " << leaked_objs << " for " << leaked_bytes << " bytes.\n";
            return ss.str();
        }
        operator const char*() {
            std::ostringstream ss;
            ss << "Leaked objects: " << leaked_objs << " for " << leaked_bytes << " bytes.\n";
            return ss.str().c_str();
        }
        friend std::ostream& operator<<(std::ostream& os, const status_t s) {
            os << "Leaked objects: " << s.leaked_objs << " for " << s.leaked_bytes << " bytes.\n";
            return os;
        }
    };
    struct status_frame_t {
        std::size_t frame_id = 0;
        std::size_t leaked_objs = 0;
        std::size_t leaked_bytes = 0;
        operator bool() {
            return leaked_objs || leaked_bytes;
        }
        operator std::string() {
            std::ostringstream ss;
            ss << "Frame " << frame_id << ": Leaked objects: " << leaked_objs << " for " << leaked_bytes << " bytes.\n";
            return ss.str();
        }
        operator const char*() {
            std::ostringstream ss;
            ss << "Frame " << frame_id << ": Leaked objects: " << leaked_objs << " for " << leaked_bytes << " bytes.\n";
            return ss.str().c_str();
        }
    };

    [[nodiscard]] static void* allocate(std::size_t n) {
        if(active) {
            ++frameCount;
            frameSize += n;
            ++allocCount;
            allocSize += n;
            if(maxSize < allocSize) {
                maxSize = allocSize;
            }
            if(maxCount < allocCount) {
                maxCount = allocCount;
            }
        }
        return std::malloc(n);
    }

    static void deallocate(void* ptr, std::size_t size) noexcept {
        if(active) {
            ++framefreeCount;
            framefreeSize += size;
            ++freeCount;
            freeSize += size;
        }
        std::free(ptr);
    }

    static void enable(bool e) {
        active = e;
    }

    static bool is_enabled() {
        return active;
    }

    static void tick() {
        if(auto f = Memory::frame_status()) {
            DebuggerPrintf(f);
        }
        ++frameCounter;
        resetframecounters();
    }

    static void resetframecounters() {
        frameSize = 0;
        frameCount = 0;
        framefreeCount = 0;
        framefreeSize = 0;
    }

    static status_t status() {
        return { allocCount - freeCount, allocSize - freeSize };
    }

    static status_frame_t frame_status() {
        return { frameCounter, frameCount - framefreeCount, frameSize - framefreeSize };
    }

    inline static std::size_t maxSize = 0;
    inline static std::size_t maxCount = 0;
    inline static std::size_t allocSize = 0;
    inline static std::size_t allocCount = 0;
    inline static std::size_t frameSize = 0;
    inline static std::size_t frameCount = 0;
    inline static std::size_t frameCounter = 0;
    inline static std::size_t freeCount = 0;
    inline static std::size_t freeSize = 0;
    inline static std::size_t framefreeCount = 0;
    inline static std::size_t framefreeSize = 0;
protected:
private:
    inline static bool active = false;
};

#ifdef TRACK_MEMORY

void* operator new(std::size_t size);
void* operator new[](std::size_t size);
void operator delete(void* ptr, std::size_t size) noexcept;
void operator delete[](void* ptr, std::size_t size) noexcept;

#endif
