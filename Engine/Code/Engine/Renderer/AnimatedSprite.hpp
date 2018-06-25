#pragma once

#include "Engine/Core/DataUtils.hpp"

#include "Engine/Math/AABB2.hpp"

class Material;
class Renderer;
class SpriteSheet;
class Texture;

class AnimatedSprite {
public:
    enum class SpriteAnimMode : int {
        Play_To_End,	   // Play from time=0 to durationSeconds, then finish
        Play_To_Beginning, // Play from time=durationSeconds to 0, then finish
        Looping,		   // Play from time=0 to end then repeat (never finish)
        Looping_Reverse,   // Play from time=durationSeconds then repeat (never finish)
        Ping_Pong, 	       // Play forwards, backwards, forwards, backwards...
        Max,
    };

    AnimatedSprite(Renderer& renderer, const XMLElement& elem);
    ~AnimatedSprite();

    void Update(float deltaSeconds);
    AABB2 GetCurrentTexCoords() const;	               // Based on the current elapsed time
    const Texture* const GetTexture() const;
    int GetNumSprites() const;
    IntVector2 GetFrameDimensions() const;
    void TogglePause();
    void Pause();					                   // Starts unpaused (playing) by default
    void Resume();				                       // Resume after pausing
    void Reset();					                   // Rewinds to time 0 and starts (re)playing
    bool IsFinished() const;                           //{ return m_isFinished; }
    bool IsPlaying() const;                            //{ return m_isPlaying; }
    float GetDurationSeconds() const;                  //{ return m_durationSeconds; }
    float GetSecondsElapsed() const;                   //{ return m_elapsedSeconds; }
    float GetSecondsRemaining() const;
    float GetFractionElapsed() const;
    float GetFractionRemaining() const;
    void SetSecondsElapsed(float secondsElapsed);	   // Jump to specific time
    void SetFractionElapsed(float fractionElapsed);    // e.g. 0.33f for one-third in
    void SetMaterial(Material* mat);
    Material* GetMaterial() const;
protected:
private:
    AnimatedSprite(Renderer& renderer, SpriteSheet* spriteSheet, float durationSeconds,
                   int startSpriteIndex, int frameLength, SpriteAnimMode playbackMode = SpriteAnimMode::Looping);

    void LoadFromXml(Renderer& renderer, const XMLElement& elem);
    SpriteAnimMode GetAnimModeFromOptions(bool looping, bool backwards, bool ping_pong /*= false*/);

    Renderer* _renderer = nullptr;
    Material* _material = nullptr;
    SpriteSheet* _sheet = nullptr;
    float _duration_seconds = 0.016f;
    float _elapsed_seconds = 0.0f;
    float _elapsed_frame_delta_seconds = 0.0f;
    float _max_frame_delta_seconds = 0.0f;
    SpriteAnimMode _playback_mode = SpriteAnimMode::Looping;
    int _start_index = 0;
    int _end_index = 1;
    bool _is_playing = true;
    bool _padding[3]{};

    friend class Renderer;

};