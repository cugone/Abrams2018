#pragma once

#include "Engine/Core/TimeUtils.hpp"

#include <chrono>
#include <ratio>

class Stopwatch {
public:
    //TODO: Add special member functions
    explicit Stopwatch(const TimeUtils::FPSeconds& seconds) noexcept;
    explicit Stopwatch(unsigned int frequency) noexcept;
    void SetSeconds(const TimeUtils::FPSeconds& seconds) noexcept;
    void SetFrequency(unsigned int hz) noexcept;
    bool Check() const noexcept;
    bool CheckAndDecrement() noexcept;
    bool CheckAndReset() noexcept;
    unsigned int DecrementAll() noexcept;
    void Reset() noexcept;
private:
    TimeUtils::FPSeconds interval_time{};
    TimeUtils::FPSeconds target_time{};

};