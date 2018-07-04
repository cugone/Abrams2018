#pragma once

#include "Engine/Core/Event.hpp"
#include "Engine/UI/Types.hpp"

#include <functional>
#include <string>

class Camera2D;

namespace UI {
    class Canvas;
}

class Game {
public:
    Game();
    ~Game();

    void Initialize();
    void BeginFrame();
    void Update(float deltaSeconds);
    void Render() const;
    void DrawPivotPositionText(const Vector2& position) const;
    void EndFrame();

protected:
private:
    void DoExport();
    void GenerateImageData(void* data);

    float _cameraSpeed = 10.0f;
    Camera2D* _camera2 = nullptr;
    UI::Canvas* _canvas = nullptr;
    UI::PivotPosition _pivot_position = UI::PivotPosition::First_;
    bool _debug = true;

};