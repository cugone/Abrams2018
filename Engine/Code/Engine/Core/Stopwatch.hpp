#pragma once

#include "Engine/Core/TimeUtils.hpp"

#include <chrono>
#include <ratio>

class Stopwatch {
public:
    explicit Stopwatch(const FPSeconds& seconds);
    explicit Stopwatch(unsigned int frequency);
    void SetSeconds(const FPSeconds& seconds);
    void SetFrequency(unsigned int hz);
    bool Check();
    bool CheckAndDecrement();
    bool CheckAndReset();
    unsigned int DecrementAll();
    void Reset();
private:
    FPSeconds interval_time;
    FPSeconds target_time;

};