#pragma once

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/TimeUtils.hpp"

#include <chrono>
#include <string>


class ProfileLogScope {
public:
    explicit ProfileLogScope(const std::string& scopeName) noexcept;
    ~ProfileLogScope() noexcept;

    ProfileLogScope() = delete;
    ProfileLogScope(const ProfileLogScope&) = delete;
    ProfileLogScope(ProfileLogScope&&) = delete;
    ProfileLogScope& operator=(const ProfileLogScope&) = delete;
    ProfileLogScope& operator=(ProfileLogScope&&) = delete;
protected:
private:
    using ProfileTimePoint = std::chrono::time_point<std::chrono::high_resolution_clock, FPMilliseconds>;

    std::string _scope_name{};
    ProfileTimePoint _time_at_creation{};
};

#if defined PROFILE_LOG_SCOPE || defined PROFILE_LOG_SCOPE_FUNCTION
#undef PROFILE_LOG_SCOPE
#undef PROFILE_LOG_SCOPE_FUNCTION
#endif
#ifdef PROFILE_BUILD
#define PROFILE_LOG_SCOPE(tag_str) ProfileLogScope TOKEN_PASTE(__plscope_,__LINE__)(tag_str)
#define PROFILE_LOG_SCOPE_FUNCTION() PROFILE_LOG_SCOPE(__FUNCTION__)
#else
#define PROFILE_LOG_SCOPE(tag_str) 
#define PROFILE_LOG_SCOPE_FUNCTION()
#endif
