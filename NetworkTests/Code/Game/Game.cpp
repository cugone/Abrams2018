#include "Game/Game.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/KerningFont.hpp"
#include "Engine/Core/Rgba.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/IntVector2.hpp"

#include "Engine/Renderer/Camera2D.hpp"

#include "Engine/RHI/RHIOutput.hpp"

#include "Game/GameCommon.hpp"
#include "Game/GameConfig.hpp"

#include <functional>
#include <sstream>

Game::Game()
    : _camera2d(std::make_unique<Camera2D>())
{
    /* DO NOTHING */
}

void Game::Initialize() {
    /* DO NOTHING */
}

void Game::BeginFrame() {
    /* DO NOTHING */
}

void Game::Update(float /*deltaSeconds*/) {
    if(g_theInput->WasKeyJustPressed(KeyCode::Esc)) {
        g_theApp->SetIsQuitting(true);
        return;
    }

}

void Game::Render() const {
    g_theRenderer->SetRenderTarget(nullptr);
    g_theRenderer->ClearColor(Rgba::OLIVE);
    g_theRenderer->ClearDepthStencilBuffer();

    g_theRenderer->SetViewport(0, 0, static_cast<unsigned int>(GRAPHICS_OPTION_WINDOW_WIDTH), static_cast<unsigned int>(GRAPHICS_OPTION_WINDOW_HEIGHT));

    g_theRenderer->SetModelMatrix(Matrix4::GetIdentity());
    g_theRenderer->SetViewMatrix(Matrix4::GetIdentity());
    g_theRenderer->SetProjectionMatrix(Matrix4::GetIdentity());

    const auto& window_dimensions = g_theRenderer->GetOutput()->GetDimensions();
    float window_width = static_cast<float>(window_dimensions.x);
    float window_height = static_cast<float>(window_dimensions.y);
    float view_half_width  = window_width * 0.50f;
    float view_half_height = window_height * 0.50f;

    Vector2 view_leftBottom = Vector2(-view_half_width, view_half_height);
    Vector2 view_rightTop   = Vector2(view_half_width, -view_half_height);
    Vector2 view_nearFar = Vector2(0.0f, 1.0f);
    Vector2 cam_pos2 = Vector2(_camera2d->GetPosition());
    _camera2d->SetupView(view_leftBottom, view_rightTop, view_nearFar, MathUtils::M_16_BY_9_RATIO);

    g_theRenderer->SetViewMatrix(_camera2d->GetViewMatrix());
    g_theRenderer->SetProjectionMatrix(_camera2d->GetProjectionMatrix());

}

void Game::EndFrame() {
    /* DO NOTHING */
}
