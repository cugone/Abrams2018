#pragma once

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/TimeUtils.hpp"

#include <chrono>
#include <string>

using ProfileTimePoint = std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::milliseconds>;

class ProfileLogScope {
public:
	ProfileLogScope(const std::string& scopeName);
    ~ProfileLogScope();
protected:
private:
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