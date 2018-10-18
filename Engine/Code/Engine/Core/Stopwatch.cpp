#include "Engine/Core/Stopwatch.hpp"

Stopwatch::Stopwatch(float frequency)
    : Stopwatch(FPSeconds(1.0f / frequency))
{
    /* DO NOTHING */
}

Stopwatch::Stopwatch(const FPSeconds& seconds) {
    SetSeconds(seconds);
}

void Stopwatch::SetSeconds(const FPSeconds& seconds) {
    interval_time = seconds;
    target_time = FPSeconds{ TimeUtils::GetCurrentTimeElapsed<FPSeconds, std::chrono::steady_clock>()
        + seconds.count() };
}

void Stopwatch::SetFrequency(float hz) {
    SetSeconds(FPSeconds(1.0f / hz));
}

bool Stopwatch::Check() {
    auto current_time = TimeUtils::GetCurrentTimeElapsed<FPSeconds, std::chrono::steady_clock>();
    return (target_time.count() < current_time);
}

bool Stopwatch::CheckAndDecrement() {
    if(Check()) {
        target_time += interval_time;
        return true;
    } else {
        return false;
    }
}

bool Stopwatch::CheckAndReset() {
    if(Check()) {
        Reset();
        return true;
    } else {
        return false;
    }
}

unsigned int Stopwatch::DecrementAll() {
    unsigned int count = 0;
    while(CheckAndDecrement()) {
        ++count;
    }

    return count;
}

void Stopwatch::Reset() {
    target_time = FPSeconds{ TimeUtils::GetCurrentTimeElapsed<FPSeconds, std::chrono::steady_clock>()
                             + interval_time.count() };
}
