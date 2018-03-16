#include "Engine/Core/Time.hpp"

#include <ctime>
#include <sstream>
#include <iomanip>

namespace TimeUtils {

std::string GetDateTimeStampFromNow(bool use_24_hour_clock /*= true*/, bool include_ms /*= true*/) {
    using namespace std::chrono;
    auto now = system_clock::now();
    std::time_t t = system_clock::to_time_t(now);
    std::tm tm;
    ::localtime_s(&tm, &t);
    std::stringstream msg;
    msg << std::put_time(&tm, use_24_hour_clock ? "%Y%m%d%H%M%S" : "%Y%m%d%I%M%S");

    if(include_ms) {
        auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
        msg << ms.count();
    }

    return msg.str();
}

std::string GetTimeStampFromNow(bool use_24_hour_clock /*= true*/, bool include_ms /*= true*/) {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto t = system_clock::to_time_t(now);
    std::tm tm;
    ::localtime_s(&tm, &t);
    std::stringstream msg;
    msg << std::put_time(&tm, use_24_hour_clock ? "%H%M%S" : "%I%M%S");

    if(include_ms) {
        auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
        msg << ms.count();
    }

    return msg.str();
}

std::string GetDateStampFromNow() {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto t = system_clock::to_time_t(now);
    std::tm tm;
    ::localtime_s(&tm, &t);
    std::stringstream msg;
    msg << std::put_time(&tm, "%Y%m%d");
    return msg.str();
}

}
