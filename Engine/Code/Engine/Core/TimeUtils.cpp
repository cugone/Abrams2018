#include "Engine/Core/TimeUtils.hpp"

#include "Engine/Core/Win.hpp"

#include <ctime>
#include <sstream>
#include <iomanip>

namespace TimeUtils {

//-----------------------------------------------------------------------------------------------
double InitializeTime(LARGE_INTEGER& out_initialTime) {
    LARGE_INTEGER countsPerSecond;
    ::QueryPerformanceFrequency(&countsPerSecond);
    ::QueryPerformanceCounter(&out_initialTime);
    return(1.0 / static_cast<double>(countsPerSecond.QuadPart));
}
//-----------------------------------------------------------------------------------------------
double GetCurrentTimeSeconds() {
    static LARGE_INTEGER initialTime;
    static double secondsPerCount = InitializeTime(initialTime);
    LARGE_INTEGER currentCount;
    ::QueryPerformanceCounter(&currentCount);
    LONGLONG elapsedCountsSinceInitialTime = currentCount.QuadPart - initialTime.QuadPart;

    double currentSeconds = static_cast<double>(elapsedCountsSinceInitialTime) * secondsPerCount;
    return currentSeconds;
}
//-----------------------------------------------------------------------------------------------

std::string GetDateTimeStampFromNow(const DateTimeStampOptions& options /*= DateTimeStampOptions{}*/) {
    using namespace std::chrono;
    auto now = system_clock::now();
    std::time_t t = system_clock::to_time_t(now);
    std::tm tm;
    ::localtime_s(&tm, &t);
    std::stringstream msg;
    std::string fmt = options.use_24_hour_clock ? (options.use_separator ? "%Y-%m-%d %H:%M:%S" : "%Y%m%d%H%M%S")
                                                : (options.use_separator ? "%Y-%m-%d %I:%M:%S" : "%Y%m%d%I%M%S");
    msg << std::put_time(&tm, fmt.c_str());

    if(options.include_milliseconds) {
        auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
        msg << '.' << std::fixed << std::setw(3) << std::setfill('0') << ms.count();
    }

    return msg.str();
}

std::string GetTimeStampFromNow(const DateTimeStampOptions& options /*= DateTimeStampOptions{}*/) {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto t = system_clock::to_time_t(now);
    std::tm tm;
    ::localtime_s(&tm, &t);
    std::ostringstream msg;
    std::string fmt = options.use_24_hour_clock ? (options.use_separator ? "%H:%M:%S" : "%H%M%S")
                                                : (options.use_separator ? "%I:%M:%S" : "%I%M%S");
    msg << std::put_time(&tm, fmt.c_str());

    if(options.include_milliseconds) {
        auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
        msg << '.' << std::fixed << std::setw(3) << std::setfill('0') << ms.count();
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
    std::string fmt = options.use_separator ? "%Y-%m-%d" : "%Y%m%d";
    msg << std::put_time(&tm, fmt.c_str());
    return msg.str();
}

}
