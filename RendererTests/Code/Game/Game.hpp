#pragma once

class Camera3D;
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
    float _rotationRateDegrees = 0.0f;
    float _cameraSpeed = 1.0f;
    Camera3D* _camera = nullptr;
    Texture* _tex = nullptr;
};