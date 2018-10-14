#pragma once

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/EngineSubsystem.hpp"

#include <string>

class Renderer;
class Camera2D;

class Profiler : public EngineSubsystem {
public:
    Profiler(Renderer* renderer, const std::string& tag_str);
	virtual ~Profiler();

    virtual void Initialize() override;
    virtual void BeginFrame() override;
    virtual void Update(float deltaSeconds) override;
    virtual void Render() const override;
    virtual void EndFrame() override;

    virtual bool ProcessSystemMessage(const EngineMessage& msg) override;

protected:
private:
    std::string _scope_name{};
    Renderer* _renderer = nullptr;
    Camera2D* _camera = nullptr;

    uint8_t _is_running: 1;
    uint8_t _is_open: 1;
    uint8_t _is_paused: 1;
    uint8_t _snapshot_requested: 1;
    uint8_t _pause_requested: 1;
    uint8_t _unpause_requested: 1;
    uint8_t _bitfield_dummy2: 1;
    uint8_t _bitfield_dummy3: 1;

};

#if defined PROFILER_SCOPE || defined PROFILER_SCOPE_FUNCTION
#undef PROFILER_SCOPE
#undef PROFILER_SCOPE_FUNCTION
#endif
#ifdef PROFILE_BUILD
#define PROFILER_SCOPE(renderer_ptr, tag_str) Profiler TOKEN_PASTE(__pscope_,__LINE__)(renderer_ptr, tag_str)
#define PROFILER_SCOPE_FUNCTION(renderer_ptr) PROFILER_SCOPE(renderer_ptr, __FUNCTION__)
#else
#define PROFILER_SCOPE(renderer_ptr, tag_str)
#define PROFILER_SCOPE_FUNCTION(renderer_ptr)
#endif