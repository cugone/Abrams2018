#include "Engine/Core/TimeUtils.hpp"

#include <ctime>
#include <sstream>
#include <iomanip>

namespace TimeUtils {

std::string GetDateTimeStampFromNow(bool use_separator /*= false*/, bool use_24_hour_clock /*= true*/, bool include_ms /*= true*/) {
    using namespace std::chrono;
    auto now = system_clock::now();
    std::time_t t = system_clock::to_time_t(now);
    std::tm tm;
    ::localtime_s(&tm, &t);
    std::stringstream msg;
    std::string fmt = "%Y%m%d%I%M%S";
    if(use_24_hour_clock) {
        fmt = "%Y%m%d%H%M%S";
    }
    if(use_separator) {
        fmt.insert(fmt.begin() + 2, '-');
        fmt.insert(fmt.begin() + 5, '-');
        fmt.insert(fmt.begin() + 8, ' ');
        fmt.insert(fmt.begin() + 11, ':');
        fmt.insert(fmt.begin() + 14, ':');
        if(include_ms) {
            fmt.insert(fmt.end() - 1, '.');
        }
    }
    msg << std::put_time(&tm, fmt.c_str());

    if(include_ms) {
        auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
        msg << std::fixed << std::setw(3) << std::setfill('0') << ms.count();
    }

    return msg.str();
}

std::string GetTimeStampFromNow(bool use_separator /*= false*/, bool use_24_hour_clock /*= true*/, bool include_ms /*= true*/) {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto t = system_clock::to_time_t(now);
    std::tm tm;
    ::localtime_s(&tm, &t);
    std::stringstream msg;
    std::string fmt = "%I%M%S";
    if(use_24_hour_clock) {
        fmt = "%H%M%S";
    }
    if(use_separator) {
        fmt.insert(fmt.begin() + 2, ':');
        fmt.insert(fmt.begin() + 5, ':');
        if(include_ms) {
            fmt.insert(fmt.end() - 1, '.');
        }
    }
    msg << std::put_time(&tm, fmt.c_str());

    if(include_ms) {
        auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
        msg << ms.count();
    }

    return msg.str();
}

std::string GetDateStampFromNow(bool use_separator /*= false*/) {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto t = system_clock::to_time_t(now);
    std::tm tm;
    ::localtime_s(&tm, &t);
    std::stringstream msg;
    msg << std::put_time(&tm, use_separator ? "%Y-%m-%d" : "%Y%m%d");
    return msg.str();
}

}
