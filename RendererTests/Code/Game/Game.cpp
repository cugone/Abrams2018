#include "Game/Game.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Rgba.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/IntVector2.hpp"

#include "Engine/Renderer/Camera3D.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Texture1D.hpp"
#include "Engine/Renderer/Texture2D.hpp"
#include "Engine/Renderer/Texture3D.hpp"

#include "Engine/RHI/RHIOutput.hpp"

#include "Game/GameCommon.hpp"
#include "Game/GameConfig.hpp"

#include <sstream>

Game::Game() {
    _camera = new Camera3D;
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
        _camera->Translate2D(0.0f, _cameraSpeed);
    } else if(g_theInput->IsKeyDown(KeyCode::DOWN)) {
        _camera->Translate2D(0.0f, -_cameraSpeed);
    }

    if(g_theInput->IsKeyDown(KeyCode::LEFT)) {
        _camera->Translate2D(-_cameraSpeed, 0.0f);
    } else if(g_theInput->IsKeyDown(KeyCode::RIGHT)) {
        _camera->Translate2D(_cameraSpeed, 0.0f);
    }

}

void Game::Render() const {

    g_theRenderer->SetRenderTarget(nullptr);
    g_theRenderer->ClearColor(Rgba::OLIVE);
    g_theRenderer->ClearDepthStencilBuffer();

    g_theRenderer->SetViewport(0, 0, static_cast<unsigned int>(GRAPHICS_OPTION_WINDOW_WIDTH), static_cast<unsigned int>(GRAPHICS_OPTION_WINDOW_HEIGHT));
    _camera->SetupView(90.0f);

    const auto& window_dimensions = g_theRenderer->GetOutput()->GetDimensions();
    float window_width = static_cast<float>(window_dimensions.x);
    float window_height = static_cast<float>(window_dimensions.y);
    float view_half_width  = window_width * 0.50f;
    float view_half_height = window_height * 0.50f;

    Vector2 leftBottom = Vector2(-view_half_width, view_half_height);
    Vector2 rightTop   = Vector2(view_half_width, -view_half_height);
    Vector2 nearFar = Vector2(0.0f, 1.0f);
    Vector2 cam_pos = Vector2(_camera->GetPosition());

    Matrix4 vT = Matrix4::CreateTranslationMatrix(cam_pos);
    Matrix4 v = vT;
    g_theRenderer->SetViewMatrix(v);
    //g_theRenderer->SetOrthoProjection(leftBottom, rightTop, nearFar);
    //g_theRenderer->SetProjectionMatrix(_camera->GetProjectionMatrix());
    //g_theRenderer->SetOrthoProjectionFromViewWidth(1600.0f, _camera->GetAspectRatio(), _camera->GetNearDistance(), _camera->GetFarDistance());
    Matrix4 s = Matrix4::CreateScaleMatrix(Vector2((float)_tex->GetDimensions().x, (float)_tex->GetDimensions().y) * 0.50f);
    Matrix4 t = Matrix4::GetIdentity();
    Matrix4 r = Matrix4::GetIdentity();
    Matrix4 mat = t * r * s;
    g_theRenderer->SetModelMatrix(mat);
    g_theRenderer->SetMaterial(g_theRenderer->GetMaterial("__default"));
    g_theRenderer->SetTexture(_tex);
    g_theRenderer->DrawQuad2D(Vector2::ZERO);

}

void Game::EndFrame() {
    /* DO NOTHING */
}

