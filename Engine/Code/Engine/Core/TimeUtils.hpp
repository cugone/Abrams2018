#pragma once

#include <chrono>
#include <string>

namespace TimeUtils {

//Get the elapsed time between calls (defaults to double-precision seconds)
template<typename Duration = std::chrono::duration<double>
        , typename Clock = std::chrono::high_resolution_clock>
decltype(auto) GetCurrentTimeElapsed() {
    static auto initial_now = time_point_cast<Duration>(Clock::now());
    auto now = time_point_cast<Duration>(Clock::now());
    return (now - initial_now).count();
}

std::string GetDateTimeStampFromNow(bool use_24_hour_clock = true, bool include_ms = true);
std::string GetTimeStampFromNow(bool use_24_hour_clock = true, bool include_ms = true);
std::string GetDateStampFromNow();

}