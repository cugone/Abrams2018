#include "Game/Game.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Rgba.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/IntVector2.hpp"

#include "Engine/Renderer/Camera2D.hpp"
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
    _tex = g_theRenderer->CreateOrGetTexture("Data/Images/Test_StbiAndDirectX.png", IntVector3::XY_AXIS);
    g_theRenderer->RegisterMaterialsFromFolder(std::string{"Data/Materials"});
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
        _camera3->Translate(0.0f, -_cameraSpeed);
        _camera2->Translate(0.0f, -_cameraSpeed);
    } else if(g_theInput->IsKeyDown(KeyCode::DOWN)) {
        _camera3->Translate(0.0f, _cameraSpeed);
        _camera2->Translate(0.0f, _cameraSpeed);
    }

    if(g_theInput->IsKeyDown(KeyCode::LEFT)) {
        _camera3->Translate(-_cameraSpeed, 0.0f);
        _camera2->Translate(-_cameraSpeed, 0.0f);
    } else if(g_theInput->IsKeyDown(KeyCode::RIGHT)) {
        _camera3->Translate(_cameraSpeed, 0.0f);
        _camera2->Translate(_cameraSpeed, 0.0f);
    }

    if(g_theInput->IsKeyDown(KeyCode::R)) {
        _camera2->SetRotationDegrees(0.0f);
        _camera2->SetPosition(Vector2::ZERO);
    }
    if(g_theInput->IsKeyDown(KeyCode::E)) {
        _camera2->ApplyRotationDegrees(-_rotationRateDegrees * deltaSeconds);
    } else if(g_theInput->IsKeyDown(KeyCode::Q)) {
        _camera2->ApplyRotationDegrees(_rotationRateDegrees * deltaSeconds);
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

    Vector2 leftBottom = Vector2(-view_half_width, view_half_height);
    Vector2 rightTop   = Vector2(view_half_width, -view_half_height);
    Vector2 nearFar = Vector2(0.0f, 1.0f);
    Vector2 cam_pos2 = Vector2(_camera2->GetPosition());
    _camera2->SetupView(leftBottom, rightTop, nearFar, MathUtils::M_16_BY_9_RATIO, -Vector3::Y_AXIS);

    g_theRenderer->SetViewMatrix(_camera2->GetViewMatrix());
    g_theRenderer->SetProjectionMatrix(_camera2->GetProjectionMatrix());

    g_theRenderer->SetMaterial(nullptr);
    for(float x = -view_half_width; x <= view_half_width; x += 100.0f) {
        Vector2 start{ x, -view_half_height };
        Vector2 end{ x, view_half_height };
        g_theRenderer->DrawLine2D(start, end, Rgba::WHITE);
    }
    for(float y = -view_half_height; y <= view_half_height; y += 100.0f) {
        Vector2 start{ view_half_width, y };
        Vector2 end{ -view_half_width, y };
        g_theRenderer->DrawLine2D(start, end, Rgba::WHITE);
    }

    Matrix4 s = Matrix4::CreateScaleMatrix(Vector2((float)_tex->GetDimensions().x, (float)_tex->GetDimensions().y) * 0.50f);
    Matrix4 t = Matrix4::GetIdentity();
    Matrix4 r = Matrix4::GetIdentity();
    Matrix4 mat = t * r * s;
    g_theRenderer->SetModelMatrix(mat);
    g_theRenderer->SetMaterial(g_theRenderer->GetMaterial("Test"));
    g_theRenderer->DrawQuad2D(Vector2::ZERO);

}

void Game::EndFrame() {
    /* DO NOTHING */
}

