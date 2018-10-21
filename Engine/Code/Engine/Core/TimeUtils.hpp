#pragma once

#include <chrono>
#include <string>

using FPSeconds = std::chrono::duration<float>;
using FPMilliseconds = std::chrono::duration<float, std::ratio_multiply<std::chrono::seconds::period, std::milli>>;
using FPMicroseconds = std::chrono::duration<float, std::ratio_multiply<std::chrono::seconds::period, std::micro>>;
using FPNanoseconds = std::chrono::duration<float, std::ratio_multiply<std::chrono::seconds::period, std::nano>>;

namespace TimeUtils {

template<typename Duration = std::chrono::duration<double>
    , typename Clock = std::chrono::high_resolution_clock>
decltype(auto) Now() noexcept {
    return std::chrono::time_point_cast<Duration>(Clock::now());
}

//Get the elapsed time between calls (defaults to double-precision seconds)
template<typename Duration = std::chrono::duration<double>
        , typename Clock = std::chrono::high_resolution_clock>
decltype(auto) GetCurrentTimeElapsed() noexcept {
    using namespace std::chrono;
    static auto initial_now = Now<Duration,Clock>();
    auto now = Now<Duration,Clock>();
    return (now - initial_now).count();
}

struct DateTimeStampOptions {
    bool use_separator = false;
    bool use_24_hour_clock = true;
    bool include_milliseconds = true;
};

std::string GetDateTimeStampFromNow(const DateTimeStampOptions& options = DateTimeStampOptions{});
std::string GetTimeStampFromNow(const DateTimeStampOptions& options = DateTimeStampOptions{});
std::string GetDateStampFromNow(const DateTimeStampOptions& options = DateTimeStampOptions{});

} //End TimeUtils