#pragma once

#include "Engine/Core/TimeUtils.hpp"

#include <chrono>
#include <string>

using ProfileTimePoint = std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::microseconds>;

class ProfileLogScope {
public:
	ProfileLogScope(const std::string& scopeName);
    ~ProfileLogScope();
protected:
private:
    std::string _scope_name{};
    ProfileTimePoint _time_at_creation{};

    decltype(auto) NowAsProfileTimePoint() const noexcept;
};

#if !defined PROFILE_LOG_SCOPE && !defined PROFILE_LOG_SCOPE_FUNCTION
#define PROFILE_LOG_SCOPE(tag_str) ProfileLogScope __pscope_##__LINE__##(tag_str)
#define PROFILE_LOG_SCOPE_FUNCTION() PROFILE_LOG_SCOPE(__FUNCTION__)
#define PROFILE_LOG_SCOPE_ONCE(tag_str) static ProfileLogScope __pscope_once_##__LINE__##(tag_str)
#define PROFILE_LOG_SCOPE_FUNCTION_ONCE() PROFILE_LOG_SCOPE_ONCE(__FUNCTION__)
#endif