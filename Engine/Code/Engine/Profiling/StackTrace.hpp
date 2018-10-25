#pragma once

#include "Engine/Core/BuildConfig.hpp"

#include <atomic>
#include <shared_mutex>
#include <vector>

class StackTrace final {
public:
    StackTrace();
	StackTrace([[maybe_unused]]unsigned long framesToSkip,
               [[maybe_unused]]unsigned long framesToCapture);
    ~StackTrace();
    bool operator==(const StackTrace& rhs);
    bool operator!=(const StackTrace& rhs);
protected:
private:

    struct callstack_line_t {
        std::string filename{};
        std::string function_name{};
        uint32_t line{};
        uint32_t offset{};
    };

    static void Initialize();
    static void Shutdown();
    [[maybe_unused]] static std::vector<StackTrace::callstack_line_t> GetLines([[maybe_unused]]StackTrace* st,
                          [[maybe_unused]]unsigned long max_lines);
    unsigned long hash = 0;
    unsigned long _frame_count = 0;
    static constexpr auto MAX_FRAMES_PER_CALLSTACK = 128ul;
    void* _frames[MAX_FRAMES_PER_CALLSTACK];
    static std::shared_mutex _cs;
    static std::atomic_uint64_t _refs;
    static std::atomic_bool _did_init;
};


#ifdef PROFILE_BUILD
#undef LOCAL_STACKTRACE
#define LOCAL_STACKTRACE {static StackTrace TOKEN_PASTE(st,__LINE__);}
#else
#undef LOCAL_STACKTRACE
#define LOCAL_STACKTRACE
#endif