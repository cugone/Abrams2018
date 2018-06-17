#pragma once

#include "Engine/Core/EngineSubsystem.hpp"

#include <map>
#include <vector>
#include <Xaudio2.h>

#pragma comment(lib, "Xaudio2.lib")

class Wav;

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

    bool LoadWavFile(const std::string& filename);
    void PlaySound(const std::string& nameOrFile);
protected:
private:
    struct Sound {
        WAVEFORMATEXTENSIBLE format_ex{};
        XAUDIO2_BUFFER buffer{};
    };
    struct Voice {
        IXAudio2SourceVoice* voice{};
    };
    std::map<std::string, Wav*> _wave_files{};
    std::map<Wav*, Sound> _sounds{};
    std::vector<Voice*> _voices{};
    IXAudio2* _xaudio2 = nullptr;
    IXAudio2MasteringVoice* _master_voice = nullptr;
};