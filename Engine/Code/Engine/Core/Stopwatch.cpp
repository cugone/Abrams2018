#include "Engine/Core/Stopwatch.hpp"

void Stopwatch::SetSeconds(float seconds) {
    auto fpsecs = FPSeconds{ seconds };
    interval_time = fpsecs;
    target_time = FPSeconds{ TimeUtils::GetCurrentTimeElapsed<FPSeconds, std::chrono::steady_clock>()
        + fpsecs.count() };
}

void Stopwatch::SetFrequency(float hz) {
    SetSeconds(1.0f / hz);
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
