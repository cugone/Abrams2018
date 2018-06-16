#include "Engine/Audio/AudioSystem.hpp"

#include "Engine/Core/BuildConfig.cpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"

AudioSystem::AudioSystem() {
    //MSDN requires flags of zero and recommends default processor
    //Since these are the default parameters, nothing else is required.
    GUARANTEE_OR_DIE(SUCCEEDED(::XAudio2Create(&_xaudio2)), "Failed to create Audio System.");
}

AudioSystem::~AudioSystem() {
    _xaudio2->Release();
    _xaudio2 = nullptr;
}

void AudioSystem::Initialize() {
    FileUtils::CreateFolders("Data/Audio");
#ifdef AUDIO_DEBUG
    XAUDIO2_DEBUG_CONFIGURATION config{};
    config.LogFileline = true;
    config.LogFunctionName = true;
    config.LogThreadID = true;
    config.LogTiming = true;
    config.BreakMask = XAUDIO2_LOG_WARNINGS;
    config.TraceMask = XAUDIO2_LOG_DETAIL | XAUDIO2_LOG_WARNINGS | XAUDIO2_LOG_FUNC_CALLS;
    _xaudio2->SetDebugConfiguration(&config);
#endif
}

void AudioSystem::BeginFrame() {
    /* DO NOTHING */
}

void AudioSystem::Update(float /*deltaSeconds*/) {
    /* DO NOTHING */
}

void AudioSystem::Render() const {
    /* DO NOTHING */
}

void AudioSystem::EndFrame() {
    /* DO NOTHING */
}

bool AudioSystem::ProcessSystemMessage(const EngineMessage& /*msg*/) {
    return false;
}
