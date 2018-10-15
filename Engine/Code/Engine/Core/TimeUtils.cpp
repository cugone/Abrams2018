#include "Engine/Core/TimeUtils.hpp"

#include <ctime>
#include <sstream>
#include <iomanip>

namespace TimeUtils {

std::string GetDateTimeStampFromNow(const DateTimeStampOptions& options /*= DateTimeStampOptions{}*/) {
    using namespace std::chrono;
    auto now = system_clock::now();
    std::time_t t = system_clock::to_time_t(now);
    std::tm tm;
    ::localtime_s(&tm, &t);
    std::stringstream msg;
    std::string fmt = "%Y%m%d%I%M%S";
    if(options.use_24_hour_clock) {
        fmt = "%Y%m%d%H%M%S";
    }
    if(options.use_separator) {
        fmt.insert(fmt.begin() + 2, '-');
        fmt.insert(fmt.begin() + 5, '-');
        fmt.insert(fmt.begin() + 8, ' ');
        fmt.insert(fmt.begin() + 11, ':');
        fmt.insert(fmt.begin() + 14, ':');
        if(options.include_milliseconds) {
            fmt.insert(fmt.end(), '.');
        }
    }
    msg << std::put_time(&tm, fmt.c_str());

    if(options.include_milliseconds) {
        auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
        msg << std::fixed << std::setw(3) << std::setfill('0') << ms.count();
    }

    return msg.str();
}

std::string GetTimeStampFromNow(const DateTimeStampOptions& options /*= DateTimeStampOptions{}*/) {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto t = system_clock::to_time_t(now);
    std::tm tm;
    ::localtime_s(&tm, &t);
    std::stringstream msg;
    std::string fmt = "%I%M%S";
    if(options.use_24_hour_clock) {
        fmt = "%H%M%S";
    }
    if(options.use_separator) {
        fmt.insert(fmt.begin() + 2, ':');
        fmt.insert(fmt.begin() + 5, ':');
        if(options.include_milliseconds) {
            fmt.insert(fmt.end() - 1, '.');
        }
    }
    msg << std::put_time(&tm, fmt.c_str());

    if(options.include_milliseconds) {
        auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
        msg << ms.count();
    }

    return msg.str();
}

std::string GetDateStampFromNow(const DateTimeStampOptions& options /*= DateTimeStampOptions{}*/) {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto t = system_clock::to_time_t(now);
    std::tm tm;
    ::localtime_s(&tm, &t);
    std::stringstream msg;
    msg << std::put_time(&tm, options.use_separator ? "%Y-%m-%d" : "%Y%m%d");
    return msg.str();
}

}
