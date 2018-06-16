#pragma once

#include "Engine/Core/EngineSubsystem.hpp"

#include <Xaudio2.h>

#pragma comment(lib, "Xaudio2.lib")

class AudioSystem : public EngineSubsystem {
public:
    AudioSystem();
    virtual ~AudioSystem();

    virtual void Initialize() override;
    virtual void BeginFrame() override;
    virtual void Update(float deltaSeconds) override;
    virtual void Render() const override;
    virtual void EndFrame() override;
    virtual bool ProcessSystemMessage(const EngineMessage& msg) override;

protected:
private:
    IXAudio2* _xaudio2 = nullptr;
};