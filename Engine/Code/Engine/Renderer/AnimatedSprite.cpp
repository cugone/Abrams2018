#include "Engine/Renderer/AnimatedSprite.hpp"

#include "Engine/Core/StringUtils.hpp"

#include "Engine/Math/MathUtils.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Texture2D.hpp"

AnimatedSprite::AnimatedSprite(Renderer& renderer,
                               SpriteSheet* spriteSheet,
                               float durationSeconds,
                               int startSpriteIndex,
                               int frameLength,
                               SpriteAnimMode playbackMode /*= SpriteAnimMode::LOOPING*/)
    : _renderer(&renderer)
    , _sheet(spriteSheet)
    , _duration_seconds(durationSeconds)
    , _playback_mode(playbackMode)
    , _start_index(startSpriteIndex)
    , _end_index(startSpriteIndex + frameLength)
{
    bool has_frames = frameLength > 0;
    _max_frame_delta_seconds = _duration_seconds / (has_frames ? (_end_index - _start_index) : 1);
}

AnimatedSprite::AnimatedSprite(Renderer& renderer, const XMLElement& elem)
    : _renderer(&renderer)
{
    LoadFromXml(*_renderer, elem);
}

AnimatedSprite::~AnimatedSprite() {
    _renderer = nullptr;
    delete _sheet;
    _sheet = nullptr;
}

void AnimatedSprite::Update(float deltaSeconds) {

    _elapsed_frame_delta_seconds += deltaSeconds;
    while(_elapsed_frame_delta_seconds >= _max_frame_delta_seconds) {
        _elapsed_frame_delta_seconds -= _max_frame_delta_seconds;
    }
    switch(_playback_mode) {
        case SpriteAnimMode::Looping:
            if(IsFinished()) {
                while(_elapsed_seconds >= _duration_seconds) {
                    _elapsed_seconds -= _duration_seconds;
                }
            }
            break;
        case SpriteAnimMode::Looping_Reverse:
            if(IsFinished()) {
                _elapsed_seconds = _duration_seconds;
                deltaSeconds *= -1.0f;
            }
            break;
        case SpriteAnimMode::Play_To_Beginning:
            if(IsFinished()) {
                _is_playing = false;
                _elapsed_seconds = 0.0f;
            }
            break;
        case SpriteAnimMode::Play_To_End:
            if(IsFinished()) {
                _is_playing = false;
                _elapsed_seconds = _duration_seconds;
            }
            break;
        case SpriteAnimMode::Ping_Pong:
            if(_elapsed_seconds < 0.0) {
                deltaSeconds *= -1.0f;
                while(_elapsed_seconds < 0.0f) {
                    _elapsed_seconds += _duration_seconds;
                }
            } else if(_elapsed_seconds >= _duration_seconds) {
                deltaSeconds *= -1.0f;
                while(_elapsed_seconds >= _duration_seconds) {
                    _elapsed_seconds -= _duration_seconds;
                }
            }
            break;
        default:
            break;
    }
    _elapsed_seconds += deltaSeconds;
}

AABB2 AnimatedSprite::GetCurrentTexCoords() const {

    auto framesPerSecond = 1.0f / _max_frame_delta_seconds;
    auto frameIndex = static_cast<int>(framesPerSecond * _elapsed_seconds);
    auto length = _end_index - _start_index;
    switch(_playback_mode) {
        case SpriteAnimMode::Play_To_End:
            if(frameIndex >= length) {
                frameIndex = _end_index - 1;
            }
            break;
        case SpriteAnimMode::Play_To_Beginning:
            if(frameIndex < 0) {
                frameIndex = 0;
            }
            break;
        case SpriteAnimMode::Looping: /* FALLTHROUGH */
        case SpriteAnimMode::Looping_Reverse:
            if(frameIndex >= length) {
                frameIndex = 0;
            }
            if(frameIndex < 0) {
                frameIndex = _end_index - 1;
            }
            break;
        case SpriteAnimMode::Ping_Pong:
            if(frameIndex >= length) {
                frameIndex = _end_index - 1;
            }
            if(frameIndex < 0) {
                frameIndex = 0;
            }
            break;
        default:
            break;
    }

    return _sheet->GetTexCoordsFromSpriteIndex(frameIndex);
}

const Texture* const AnimatedSprite::GetTexture() const {
    return &_sheet->GetTexture();
}

int AnimatedSprite::GetNumSprites() const {
    return _sheet->GetNumSprites();
}

IntVector2 AnimatedSprite::GetFrameDimensions() const {
    return _sheet->GetFrameDimensions();
}

void AnimatedSprite::TogglePause() {
    _is_playing = !_is_playing;
}

void AnimatedSprite::Pause() {
    _is_playing = false;
}

void AnimatedSprite::Resume() {
    _is_playing = true;
}

void AnimatedSprite::Reset() {
    _elapsed_seconds = 0.0f;
}

bool AnimatedSprite::IsFinished() const {
    if(!_is_playing) {
        return false;
    }
    switch(_playback_mode) {
        case SpriteAnimMode::Looping: /* FALL THROUGH */
        case SpriteAnimMode::Play_To_End:
            return !(_elapsed_seconds < _duration_seconds);
        case SpriteAnimMode::Looping_Reverse: /* FALL THROUGH */
        case SpriteAnimMode::Play_To_Beginning:
            return _elapsed_seconds < 0.0f;
        case SpriteAnimMode::Ping_Pong:
            return false;
        default:
            return !(_elapsed_seconds < _duration_seconds);
    }
}

bool AnimatedSprite::IsPlaying() const {
    return _is_playing;
}

float AnimatedSprite::GetDurationSeconds() const {
    return _duration_seconds;
}

float AnimatedSprite::GetSecondsElapsed() const {
    return _elapsed_seconds;
}

float AnimatedSprite::GetSecondsRemaining() const {
    return _duration_seconds - _elapsed_seconds;
}

float AnimatedSprite::GetFractionElapsed() const {
    return _elapsed_seconds / _duration_seconds;
}

float AnimatedSprite::GetFractionRemaining() const {
    return (_duration_seconds - _elapsed_seconds) / _duration_seconds;
}

void AnimatedSprite::SetSecondsElapsed(float secondsElapsed) {
    _elapsed_seconds = secondsElapsed;
}

void AnimatedSprite::SetFractionElapsed(float fractionElapsed) {
    _elapsed_seconds = _duration_seconds * fractionElapsed;
}

void AnimatedSprite::SetMaterial(Material* mat) {
    _material = mat;
}

Material* AnimatedSprite::GetMaterial() const {
    return _material;
}

AnimatedSprite::SpriteAnimMode AnimatedSprite::GetAnimModeFromOptions(bool looping, bool backwards, bool ping_pong /*= false*/) {
    if(ping_pong) {
        return SpriteAnimMode::Ping_Pong;
    }

    if(looping) {
        if(backwards) {
            return SpriteAnimMode::Looping_Reverse;
        }
        return SpriteAnimMode::Looping;
    }
    if(backwards) {
        return SpriteAnimMode::Play_To_Beginning;
    }
    return SpriteAnimMode::Play_To_End;
}

void AnimatedSprite::LoadFromXml(Renderer& renderer, const XMLElement& elem) {
    DataUtils::ValidateXmlElement(elem, "animation", "spritesheet,animationset", "", "", "name");
    auto xml_sheet = elem.FirstChildElement("spritesheet");
    DataUtils::ValidateXmlElement(*xml_sheet, "spritesheet", "", "src,dimensions");
    _sheet = renderer.CreateSpriteSheet(*xml_sheet);

    auto xml_animset = elem.FirstChildElement("animationset");
    DataUtils::ValidateXmlElement(*xml_animset, "animationset", "", "startindex,framelength,duration", "", "loop,reverse,pingpong");

    _start_index = DataUtils::ParseXmlAttribute(*xml_animset, "startindex", _start_index);
    int frameLength = DataUtils::ParseXmlAttribute(*xml_animset, "framelength", 0);
    _end_index = _start_index + frameLength;

    auto min_duration = 0.016f;
    _duration_seconds = DataUtils::ParseXmlAttribute(*xml_animset, "duration", _duration_seconds);
    if(_duration_seconds < min_duration) {
        _duration_seconds = min_duration;
    }

    bool is_looping = DataUtils::ParseXmlAttribute(*xml_animset, "loop", false);
    bool is_reverse = DataUtils::ParseXmlAttribute(*xml_animset, "reverse", false);
    bool is_pingpong = DataUtils::ParseXmlAttribute(*xml_animset, "pingpong", false);
    _playback_mode = GetAnimModeFromOptions(is_looping, is_reverse, is_pingpong);

    bool has_frames = frameLength > 0;
    _max_frame_delta_seconds = _duration_seconds / (has_frames ? (_end_index - _start_index) : 1);
}
