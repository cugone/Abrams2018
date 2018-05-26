#pragma once

class Camera;
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
    float _thickness = 0.0f;
    Camera* _camera = nullptr;
    size_t _poly_sides = 2;
    Texture* _tex = nullptr;
};