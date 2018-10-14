#pragma once

#include <atomic>
#include <vector>

class StackTrace final {
public:
    StackTrace();
	StackTrace([[maybe_unused]]unsigned long framesToSkip,
               [[maybe_unused]]unsigned long framesToCapture);
	~StackTrace();
    bool operator==(const StackTrace& rhs);
    bool operator!=(const StackTrace& rhs);
    bool operator<(const StackTrace& rhs);
protected:
private:

    static void Initialize();
    static void Shutdown();

    unsigned long _hash = 0;
    unsigned long _frame_count = 0;
    std::vector<void*> _frames{};
    static std::atomic_uint64_t _refs;

};