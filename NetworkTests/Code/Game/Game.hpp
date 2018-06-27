#pragma once

#include "Engine/Core/Event.hpp"

#include "Engine/Renderer/Camera2D.hpp" 

#include <memory>

class Game {
public:
    Game();
    ~Game() = default;

    void Initialize();
    void BeginFrame();
    void Update(float deltaSeconds);
    void Render() const;
    void EndFrame();

protected:
private:
    std::unique_ptr<Camera2D> _camera2d{};
};