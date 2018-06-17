#include "Engine/Audio/AudioSystem.hpp"

#include "Engine/Core/BuildConfig.cpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"

#include "Engine/Audio/Wav.hpp"

#include <filesystem>

AudioSystem::AudioSystem() {
    //MSDN requires flags of zero and recommends default processor
    //Since these are the default parameters, nothing else is required.
    GUARANTEE_OR_DIE(SUCCEEDED(::XAudio2Create(&_xaudio2)), "Failed to create Audio System.");
}

AudioSystem::~AudioSystem() {

    for(auto& v : _voices) {
        v->voice->Stop();
        v->voice->FlushSourceBuffers();
        v->voice->DestroyVoice();
        v->voice = nullptr;
        delete v;
        v = nullptr;
    }
    _voices.clear();
    _voices.shrink_to_fit();

    _sounds.clear();
    for(auto& wav : _wave_files) {
        delete wav.second;
        wav.second = nullptr;
    }
    _wave_files.clear();

    _master_voice->DestroyVoice();
    _master_voice = nullptr;

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
    _xaudio2->CreateMasteringVoice(&_master_voice, XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE);
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

void AudioSystem::PlaySound(const std::string& nameOrFile) {
    namespace FS = std::experimental::filesystem;
    FS::path filepath(nameOrFile);
    auto found_wav = _wave_files.find(filepath.string());
    if(found_wav != _wave_files.end()) {
        auto found_snd = _sounds.find((*found_wav).second);
        if(found_snd != _sounds.end()) {
            auto snd = (*found_snd).second;
            IXAudio2SourceVoice* voice = nullptr;
            if(FAILED(_xaudio2->CreateSourceVoice(&voice, reinterpret_cast<WAVEFORMATEX*>(&snd.format_ex)))) {
                return;
            }
            if(FAILED(voice->SubmitSourceBuffer(&snd.buffer))) {
                return;
            }
            auto v = new Voice;
            v->voice = voice;
            _voices.push_back(v);
            v->voice->Start();
        }
    }
}

bool AudioSystem::LoadWavFile(const std::string& filename) {
    namespace FS = std::experimental::filesystem;
    auto wav = new Wav;
    FS::path filepath = filename;
    auto wav_result = wav->Load(filepath.string());
    if(wav_result != Wav::WAV_SUCCESS) {
        std::string e;
        switch(wav_result) {
            case Wav::WAV_ERROR_NOT_A_WAV:
            {
                e = filepath.string() + " is not a .wav file.\n";
                DebuggerPrintf(e.c_str());
            }
            case Wav::WAV_ERROR_BAD_FILE:
            {
                e = filepath.string() + " is improperly formatted.\n";
                DebuggerPrintf(e.c_str());
            }
        }
        return false;
    }
    auto found_iter = _wave_files.find(filepath.string());
    if(found_iter != _wave_files.end()) {
        delete (*found_iter).second;
        (*found_iter).second = nullptr;
    }
    _wave_files.insert_or_assign(filepath.string(), wav);
    WAVEFORMATEXTENSIBLE wfmtex{};
    std::memcpy(&wfmtex, wav->GetFormatAsBuffer(), sizeof(wfmtex));

    XAUDIO2_BUFFER buffer{};
    buffer.AudioBytes = wav->GetDataBufferSize();
    buffer.pAudioData = wav->GetDataBuffer();
    buffer.Flags = XAUDIO2_END_OF_STREAM;

    Sound snd{};
    snd.format_ex = wfmtex;
    snd.buffer = buffer;
    _sounds.insert_or_assign(wav, snd);

    return true;
}
