#pragma once

#include "Engine/Core/EngineSubsystem.hpp"

class App : public EngineSubsystem {
public:
    App();
    ~App();

    bool IsQuitting() const;
    void SetIsQuitting(bool quit);
    void Initialize();

    void RunFrame();


    virtual bool ProcessSystemMessage(const EngineMessage& msg) override;

protected:
private:
    virtual void BeginFrame() override;
    virtual void Update(float deltaSeconds) override;
    virtual void Render() const override;
    virtual void EndFrame() override;

    bool _isQuitting = false;

};