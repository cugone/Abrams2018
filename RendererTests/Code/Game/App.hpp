#pragma once

#include "Engine/Core/EngineSubsystem.hpp"

#include <condition_variable>

class JobSystem;

class App : public EngineSubsystem {
public:
    App(JobSystem& jobSystem, std::condition_variable* mainJobSignal);
    ~App();

    bool IsQuitting() const;
    void SetIsQuitting(bool quit);
    void Initialize();

    void RunFrame();
    bool HasFocus() const;
    bool LostFocus() const;
    bool GainedFocus() const;

    virtual bool ProcessSystemMessage(const EngineMessage& msg) override;

    static bool applet_mode;
protected:
private:
    virtual void BeginFrame() override;
    virtual void Update(float deltaSeconds) override;
    virtual void Render() const override;
    virtual void EndFrame() override;

    bool _isQuitting = false;
    bool _current_focus = true;
    bool _previous_focus = false;
    std::condition_variable* _main_job_signal = nullptr;
    JobSystem* _job_system = nullptr;
};