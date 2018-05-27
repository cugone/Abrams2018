#pragma once

class Camera3D;
class Camera2D;
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

protected:
private:
    float _angleDegrees = 0.0f;
    float _rotationRateDegrees = 10.0f;
    float _cameraSpeed = 10.0f;
    Camera3D* _camera3 = nullptr;
    Camera2D* _camera2 = nullptr;
    Texture* _tex = nullptr;
};