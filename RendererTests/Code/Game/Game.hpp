#pragma once

#include "Engine/Core/Event.hpp"
#include "Engine/Core/Obj.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"

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
class Sprite;
}

struct health_buffer_t {
    float health_percentage = 0.0f;
    float PADDING[3] = { 0.0f };
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
    void DrawWorldGrid() const;
    void DrawAxes() const;
    void DrawCube() const;

    UI::Canvas* _canvas = nullptr;
    UI::Panel* _panel = nullptr;
    UI::Label* _label = nullptr;
    UI::Label* _label_deltaSeconds = nullptr;
    UI::PivotPosition _label_pivot = UI::PivotPosition::Center;
    UI::Sprite* _sprite = nullptr;
    const float _cameraSpeed = 10.0f;
    const float _camera_move_speed_multiplier = 10.0f;
    Camera2D* _camera2 = nullptr;
    Camera3D* _camera3 = nullptr;
    bool _debug = true;
    bool _slowmo = false;
};