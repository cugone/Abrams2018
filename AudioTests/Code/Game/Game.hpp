#pragma once

#include "Engine/Core/TimeUtils.hpp"

class AnimatedSprite;
class Camera3D;
class Camera2D;
class Image;
class SpriteSheet;
class Texture;

class Game {
public:
    Game();
    ~Game();

    void Initialize();
    void BeginFrame();
    void Update([[maybe_unused]]TimeUtils::FPSeconds deltaSeconds);
    void Render() const;
    void EndFrame();

protected:
private:
    Camera3D* _camera3 = nullptr;
    Camera2D* _camera2 = nullptr;
};