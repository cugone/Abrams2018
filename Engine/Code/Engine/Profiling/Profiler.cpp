#include "Engine/Core/Profiler.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Renderer/Camera2D.hpp"

#include <sstream>

Profiler::Profiler(Renderer* renderer, const std::string& tag_str)
    : EngineSubsystem()
    , _scope_name(tag_str)
    , _renderer(renderer)
    , _camera(new Camera2D)
{
    /* DO NOTHING */
}

Profiler::~Profiler() {
    std::ostringstream ss;
    ss << "Profiler " << _scope_name << "\n";
    DebuggerPrintf(ss.str().c_str());

    delete _camera;
    _renderer = nullptr;
}

void Profiler::Initialize() {
    /* DO NOTHING */
}

void Profiler::BeginFrame() {
    /* DO NOTHING */
}

void Profiler::Update(float /*deltaSeconds*/) {
    /* DO NOTHING */
}

void Profiler::Render() const {
    /* DO NOTHING */
}

void Profiler::EndFrame() {
    /* DO NOTHING */
}

bool Profiler::ProcessSystemMessage(const EngineMessage& /*msg*/) {
    return false;
}

#ifdef PROFILE_BUILD

#else

#endif