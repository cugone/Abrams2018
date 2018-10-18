#include "Engine/Profiling/ProfileLogScope.hpp"

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/TimeUtils.hpp"

#include <iomanip>
#include <sstream>

ProfileLogScope::ProfileLogScope(const std::string& scopeName)
    : _scope_name(scopeName)
    , _time_at_creation(TimeUtils::Now<ProfileTimePoint::duration, ProfileTimePoint::clock>())
{
    /* DO NOTHING */
}

ProfileLogScope::~ProfileLogScope() {
    auto now = TimeUtils::Now<ProfileTimePoint::duration, ProfileTimePoint::clock>();
    auto elapsedTime = (now - _time_at_creation).count();
    std::ostringstream ss;
    ss << "ProfileLogScope " << _scope_name << " took " << elapsedTime << " ms.\n";
    DebuggerPrintf(ss.str().c_str());
}

