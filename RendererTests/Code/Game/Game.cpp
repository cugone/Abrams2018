#include "Game/Game.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Rgba.hpp"

#include "Engine/Math/MathUtils.hpp"

#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Texture1D.hpp"
#include "Engine/Renderer/Texture2D.hpp"
#include "Engine/Renderer/Texture3D.hpp"

#include "Game/GameCommon.hpp"
#include "Game/GameConfig.hpp"

#include <sstream>

Game::Game() {
    _camera = new Camera;
}

Game::~Game() {
    delete _camera;
    _camera = nullptr;
}

void Game::Initialize() {
    _tex = g_theRenderer->CreateOrGetTexture("Data/Images/Test_StbiAndDirectX.png", IntVector3::XY_AXIS);
}

void Game::BeginFrame() {
    /* DO NOTHING */
}

void Game::Update(float deltaSeconds) {

    if(g_theInput->WasKeyJustPressed(KeyCode::ESC)) {
        g_theApp->SetIsQuitting(true);
        return;
    }

    _angleDegrees += _rotationRateDegrees * deltaSeconds;
    while(_angleDegrees >= 360.0f) {
        _angleDegrees -= 360.f;
    }


    if(g_theInput->IsKeyDown(KeyCode::UP)) {
        ++_poly_sides;
    } else if(g_theInput->IsKeyDown(KeyCode::DOWN)) {
        --_poly_sides;
    }


    if(g_theInput->IsKeyDown(KeyCode::LEFT)) {
        _thickness -= 0.001f;
    } else if(g_theInput->IsKeyDown(KeyCode::RIGHT)) {
        _thickness += 0.001f;
    }

    _poly_sides = MathUtils::Clamp(_poly_sides, static_cast<std::size_t>(1), _poly_sides);
    _thickness = MathUtils::Clamp(_thickness, 0.0f, _thickness);

}

void Game::Render() const {

    g_theRenderer->SetRenderTarget(nullptr);
    g_theRenderer->ClearColor(Rgba::OLIVE);
    g_theRenderer->ClearDepthStencilBuffer();

    g_theRenderer->SetViewport(0, 0, static_cast<unsigned int>(GRAPHICS_OPTION_WINDOW_WIDTH), static_cast<unsigned int>(GRAPHICS_OPTION_WINDOW_HEIGHT));
    _camera->SetupView(90.0f);

    g_theRenderer->SetProjectionMatrix(Matrix4::GetIdentity());

    //g_theRenderer->SetViewMatrix(Matrix4::CreateTranslationMatrix(-_camera->position));    
    g_theRenderer->SetViewMatrix(Matrix4::GetIdentity());
    //g_theRenderer->SetOrthoProjectionFromViewHeight(2.0f, _camera->aspectRatio, 0.0f, 1.0f);

    Matrix4 s = Matrix4::GetIdentity();
    Matrix4 t = Matrix4::GetIdentity();
    Matrix4 r = Matrix4::GetIdentity();
    Matrix4 mat = t * r * s;
    g_theRenderer->SetModelMatrix(mat);
    g_theRenderer->SetMaterial(g_theRenderer->GetMaterial("__default"));
    g_theRenderer->SetTexture(_tex);
    g_theRenderer->DrawQuad2D(Vector2::ZERO, Vector2::ONE);

}

void Game::EndFrame() {
    /* DO NOTHING */
}

