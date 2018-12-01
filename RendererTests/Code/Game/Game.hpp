#pragma once

#include "Engine/Core/Event.hpp"
#include "Engine/Core/Obj.hpp"
#include "Engine/Core/TimeUtils.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"

#include "Engine/Renderer/Camera2D.hpp"
#include "Engine/Renderer/Camera3D.hpp"

#include "Engine/UI/Types.hpp"

#include <functional>
#include <string>

class AnimatedSprite;
class ConstantBuffer;
class Texture;

namespace UI {
class Canvas;
class Panel;
class Label;
class Sprite;
}

class Game {
public:
    Game() = default;
    ~Game();

    void Initialize();
    void InitializeData();
    void InitializeUI();

    void BeginFrame();
    void Update(TimeUtils::FPSeconds deltaSeconds);
    void Render() const;
    void RenderStuff() const;
    void EndFrame();

protected:
private:
    void UpdateCameraFromKeyboard(TimeUtils::FPSeconds deltaSeconds);
    void UpdateCameraFromMouse(TimeUtils::FPSeconds deltaSeconds);
    void DrawWorldGrid() const;
    void DrawAxes() const;
    void DrawCube() const;
    void DrawCeres() const;

    UI::Canvas* _canvas = nullptr;
    UI::Panel* _panel = nullptr;
    UI::Label* _label = nullptr;
    const float _cameraSpeed = 10.0f;
    const float _camera_move_speed_multiplier = 10.0f;
    mutable Camera2D _camera2{};
    mutable Camera3D _camera3{};
    bool _debug = true;
    bool _slowmo = false;
};