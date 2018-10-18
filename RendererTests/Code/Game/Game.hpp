#pragma once

#include "Engine/Core/Event.hpp"
#include "Engine/Core/Obj.hpp"

#include "Engine/UI/Types.hpp"

#include <functional>
#include <string>

class Camera2D;
class Camera3D;
class ConstantBuffer;
class Texture;

namespace UI {
    class Canvas;
    class Panel;
    class Label;
}

struct health_buffer_t {
    float health_percentage = 0.0f;
    float PADDING[3] = {0.0f};
};

class Game {
public:
    Game();
    ~Game();

    void Initialize();
    void InitializeData();
    void InitializeUI();

    void BeginFrame();
    void Update(float deltaSeconds);
    void Render() const;
    void RenderStuff() const;
    void EndFrame();

protected:
private:
    void UpdateCameraFromKeyboard(float deltaSeconds);
    void UpdateCameraFromMouse(float deltaSeconds);
    void DoExport();
    void GenerateImageData(void* data);
    void DrawWorldGrid() const;
    void DrawAxes() const;
    void DrawCube() const;
    void DrawObj() const;

    UI::Canvas* _canvas = nullptr;
    FileUtils::Obj _obj{};
    Texture* _offscreenUiTexture = nullptr;
    Texture* _testDepthStencil = nullptr;
    ConstantBuffer* _health_cb = nullptr;
    health_buffer_t health_data{};
    const float _cameraSpeed = 10.0f;
    const float _camera_move_speed_multiplier = 10.0f;
    Camera2D* _camera2 = nullptr;
    Camera3D* _camera3 = nullptr;
    bool _debug = true;
    bool _slowmo = false;
    bool _wireframe_mode = false;
};