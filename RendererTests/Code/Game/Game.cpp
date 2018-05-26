#include "Game/Game.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Rgba.hpp"

#include "Engine/Math/MathUtils.hpp"

#include "Engine/Renderer/Camera.hpp"

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
    /* DO NOTHING */
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


    if(g_theInput->WasKeyJustPressed(KeyCode::UP)) {
        if(_poly_sides <= 64) {
            ++_poly_sides;
        }
    } else if(g_theInput->WasKeyJustPressed(KeyCode::DOWN)) {
        if(_poly_sides > 0) {
            --_poly_sides;
        }
    }


    if(g_theInput->IsKeyDown(KeyCode::LEFT)) {
        _thickness -= 0.001f;
    } else if(g_theInput->IsKeyDown(KeyCode::RIGHT)) {
        _thickness += 0.001f;
    }

    _thickness = MathUtils::Clamp(_thickness, 0.0f, _thickness);

}

void Game::Render() const {

    g_theRenderer->SetRenderTarget(nullptr);
    g_theRenderer->ClearColor(Rgba::BLACK);
    g_theRenderer->ClearDepthStencilBuffer();

    g_theRenderer->SetViewport(0, 0, static_cast<unsigned int>(GRAPHICS_OPTION_WINDOW_WIDTH), static_cast<unsigned int>(GRAPHICS_OPTION_WINDOW_HEIGHT));
    _camera->SetupView(90.0f);

    g_theRenderer->SetProjectionMatrix(Matrix4::GetIdentity());

    Vector3 cam_pos = Vector3(0.0f, 0.0f, 0.0f);
    g_theRenderer->SetViewMatrix(Matrix4::CreateTranslationMatrix(-cam_pos));
    
    float view_height = 900.0f;
    float view_width = view_height * _camera->GetAspectRatio();
    Vector2 view_half_extents = Vector2(view_width, view_height) * 0.50f;
    Vector2 leftBottom = Vector2(-view_half_extents.x, view_half_extents.y);
    Vector2 rightTop = Vector2(view_half_extents.x, -view_half_extents.y);
    g_theRenderer->SetOrthoProjection(leftBottom, rightTop, Vector2(0.0f, 1.0f));

    Matrix4 s = Matrix4::GetIdentity();
    Matrix4 t = Matrix4::GetIdentity();
    Matrix4 r = Matrix4::Create2DRotationDegreesMatrix(_angleDegrees);
    Matrix4 mat = t * r * s;
    g_theRenderer->SetModelMatrix(mat);
    g_theRenderer->SetMaterial(g_theRenderer->GetMaterial("__default"));
    g_theRenderer->DrawCircle2D(Vector2::ZERO, 100.0f);
    g_theRenderer->DrawPoint2D(Vector2::ZERO);
    //g_theRenderer->DrawLine2D(Vector2::ZERO, Vector2(800.0f, 0.0f), Rgba::WHITE, _thickness);
    //g_theRenderer->DrawPolygon2D(Vector2::ZERO, 1.0f, poly_sides);

}

void Game::EndFrame() {
    /* DO NOTHING */
}

