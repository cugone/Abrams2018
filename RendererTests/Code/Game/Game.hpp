#pragma once

#include "Engine/Core/Event.hpp"

#include <functional>
#include <string>

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
    void Update(float deltaSeconds);
    void Render() const;
    void EndFrame();

    Event<> OnBeginFrame;
    void OnBeganFrame();
    void OnBeganFrameWithArg();
protected:
private:
    void DoExport();
    void GenerateImageData(void* data);


    float _cameraSpeed = 10.0f;
    Camera3D* _camera3 = nullptr;
    Camera2D* _camera2 = nullptr;
    Texture* _tex = nullptr;
    AnimatedSprite* _gif_test = nullptr;

};