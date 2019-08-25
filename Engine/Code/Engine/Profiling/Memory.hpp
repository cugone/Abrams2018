#pragma once
//https://www.youtube.com/watch?v=e2ZQyYr0Oi0
//C++17 - The Best Features - Nicolai Josuttis [ACCU 2018]

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//#include "Engine/Profiling/StackTrace.hpp"

#include <array>
#include <new>
#include <sstream>
#include <iostream>
#include <ostream>
#include <charconv>

class Memory {
public:

    struct status_t {
        std::size_t leaked_objs  = 0;
        std::size_t leaked_bytes = 0;
        operator bool() const noexcept {
            return leaked_objs || leaked_bytes;
        }
        operator std::string() const noexcept {
#ifdef TRACK_MEMORY
            static std::array<char, 1024> s{"Leaked objects: %f for %f bytes.\n"};
            if (auto[p, ec] = std::to_chars(s.data(), s.data() + s.size(), static_cast<float>(leaked_objs));
                ec != std::errc()) {
                DebuggerPrintf("Memory profiler could not convert leaked objects value for printing");
                return {};
            }
            if (auto[p, ec] = std::to_chars(s.data(), s.data() + s.size(), static_cast<float>(leaked_bytes));
                ec != std::errc()) {
                DebuggerPrintf("Memory profiler could not convert leaked bytes value for printing");
                return {};
            }
            return std::string(std::string_view(s.data(), s.size()));
#else
            return {};
#endif
        }
        friend std::ostream& operator<<(std::ostream& os, [[maybe_unused]]const status_t& s) noexcept {
#ifdef TRACK_MEMORY
            static std::array<char, 1024> str{ "Leaked objects: %f for %f bytes.\n" };
            if (auto[p, ec] = std::to_chars(str.data(), str.data() + str.size(), static_cast<float>(s.leaked_objs));
                ec != std::errc()) {
                DebuggerPrintf("Memory profiler could not convert leaked objects value for printing");
                return os;
            }
            if (auto[p, ec] = std::to_chars(str.data(), str.data() + str.size(), static_cast<float>(s.leaked_bytes));
                ec != std::errc()) {
                DebuggerPrintf("Memory profiler could not convert leaked bytes value for printing");
                return os;
            }
#endif
            return os;
        }
    };
    struct status_frame_t {
        std::size_t frame_id = 0;
        std::size_t leaked_objs = 0;
        std::size_t leaked_bytes = 0;
        operator bool() const noexcept {
            return leaked_objs || leaked_bytes;
        }
        operator std::string() const noexcept {
#ifdef TRACK_MEMORY
            std::ostringstream ss;
            ss << "Frame " << frame_id << ": Leaked objects: " << leaked_objs << " for " << leaked_bytes << " bytes.\n";
            std::string s = ss.str();
            return s;
#else
            return {};
#endif
        }
        friend std::ostream& operator<<(std::ostream& os, [[maybe_unused]]const status_frame_t& s) noexcept {
#ifdef TRACK_MEMORY
            os << "Frame " << s.frame_id << ": Leaked objects: " << s.leaked_objs << " for " << s.leaked_bytes << " bytes.\n";
#endif
            return os;
        }
    };

    [[nodiscard]] static void* allocate(std::size_t n) noexcept {
        if(is_enabled()) {
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
        if(is_enabled()) {
            ++framefreeCount;
            framefreeSize += size;
            ++freeCount;
            freeSize += size;
        }
        std::free(ptr);
    }

    static void enable([[maybe_unused]]bool e) noexcept {
#ifdef TRACK_MEMORY
        _active = e;
#endif
    }

    static bool is_enabled() noexcept {
#ifdef TRACK_MEMORY
        return _active;
#else
        return false;
#endif
    }

    static void trace([[maybe_unused]]bool doTrace) noexcept {
#ifdef TRACK_MEMORY
        _trace = doTrace;
#endif
    }

    static void tick() noexcept {
#ifdef TRACK_MEMORY
        if(auto f = Memory::frame_status()) {
            std::cout << f << '\n';
        }
        ++frameCounter;
        resetframecounters();
#endif
    }

    static void resetframecounters() noexcept {
#ifdef TRACK_MEMORY
        frameSize = 0;
        frameCount = 0;
        framefreeCount = 0;
        framefreeSize = 0;
#endif
    }

    static status_t status() noexcept {
        return { allocCount - freeCount, allocSize - freeSize };
    }

    static status_frame_t frame_status() noexcept {
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
    inline static bool _active = false;
    inline static bool _trace = false;
};

#ifdef TRACK_MEMORY

void* operator new(std::size_t size);
void* operator new[](std::size_t size);
void operator delete(void* ptr, std::size_t size) noexcept;
void operator delete[](void* ptr, std::size_t size) noexcept;

#endif
