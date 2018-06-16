#include "Game/Game.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/KerningFont.hpp"
#include "Engine/Core/Rgba.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/IntVector2.hpp"

#include "Engine/Renderer/AnimatedSprite.hpp"
#include "Engine/Renderer/Camera2D.hpp"
#include "Engine/Renderer/Camera3D.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Texture1D.hpp"
#include "Engine/Renderer/Texture2D.hpp"
#include "Engine/Renderer/Texture3D.hpp"

#include "Engine/RHI/RHIOutput.hpp"

#include "Game/GameCommon.hpp"
#include "Game/GameConfig.hpp"

#include <sstream>

Game::Game() {
    _camera3 = new Camera3D;
    _camera2 = new Camera2D;
}

Game::~Game() {
    delete _camera3;
    _camera3 = nullptr;
    delete _camera2;
    _camera2 = nullptr;
}

void Game::Initialize() {
    g_theRenderer->RegisterTexturesFromFolder(std::string{"Data/Images"});
    g_theRenderer->RegisterMaterialsFromFolder(std::string{"Data/Materials"});
    g_theRenderer->RegisterFontsFromFolder(std::string{"Data/Fonts"});
}

void Game::BeginFrame() {
    /* DO NOTHING */
}

void Game::Update(float deltaSeconds) {

    if(g_theInput->WasKeyJustPressed(KeyCode::ESC)) {
        g_theApp->SetIsQuitting(true);
        return;
    }

    _camera3->Update(deltaSeconds);
    _camera2->Update(deltaSeconds);

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
    Vector2 cam_pos2 = Vector2(_camera2->GetPosition());
    _camera2->SetupView(view_leftBottom, view_rightTop, view_nearFar, MathUtils::M_16_BY_9_RATIO);

    g_theRenderer->SetViewMatrix(_camera2->GetViewMatrix());
    g_theRenderer->SetProjectionMatrix(_camera2->GetProjectionMatrix());

}

void Game::EndFrame() {
    /* DO NOTHING */
}

