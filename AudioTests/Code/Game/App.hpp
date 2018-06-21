#pragma once

#include "Engine/Core/EngineSubsystem.hpp"
#include "Engine/Core/JobSystem.hpp"

#include <condition_variable>

class App : public EngineSubsystem {
public:
    App(JobSystem& jobSystem, std::condition_variable* cv = nullptr);
    virtual ~App();

    bool IsQuitting() const;
    void SetIsQuitting(bool quit);
    void Initialize() override;

    void RunFrame();
    virtual bool ProcessSystemMessage(const EngineMessage& msg) override;

protected:
private:
    virtual void BeginFrame() override;
    virtual void Update(float deltaSeconds) override;
    virtual void Render() const override;
    virtual void EndFrame() override;

    bool _isQuitting = false;
    JobSystem* _job_system = nullptr;
    std::condition_variable* _cv = nullptr;
};