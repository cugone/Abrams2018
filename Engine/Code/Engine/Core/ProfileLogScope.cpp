#include "Engine/Core/ProfileLogScope.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include <iomanip>
#include <sstream>

decltype(auto) ProfileLogScope::NowAsProfileTimePoint() const noexcept {
    return std::chrono::time_point_cast<ProfileTimePoint::duration>(ProfileTimePoint::clock::now());
}

ProfileLogScope::ProfileLogScope(const std::string& scopeName)
    : _scope_name(scopeName)
    , _time_at_creation(NowAsProfileTimePoint())
{

}

ProfileLogScope::~ProfileLogScope() {
    auto now = NowAsProfileTimePoint();
    auto elapsedTime = (now - _time_at_creation).count();
    std::ostringstream ss;
    ss << "ProfileLogScope " << _scope_name << " took " << elapsedTime << " time units.\n";
    DebuggerPrintf(ss.str().c_str());
}
