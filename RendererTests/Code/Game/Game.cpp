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
    delete _gif_test;
    _gif_test = nullptr;
}

void Game::Initialize() {
    g_theRenderer->RegisterTexturesFromFolder(std::string{"Data/Images"});
    g_theRenderer->RegisterMaterialsFromFolder(std::string{"Data/Materials"});
    g_theRenderer->RegisterFontsFromFolder(std::string{"Data/Fonts"});
    _gif_test = g_theRenderer->CreateAnimatedSprite("Data/Images/cute_sif.gif");
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

    _camera3->Update(deltaSeconds);
    _camera2->Update(deltaSeconds);
    _gif_test->Update(deltaSeconds);

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

    auto tex_dims = _tex->GetDimensions();
    Matrix4 s = Matrix4::CreateScaleMatrix(Vector2(static_cast<float>(tex_dims.x), static_cast<float>(tex_dims.y)) * 0.50f);
    Matrix4 t = Matrix4::GetIdentity();
    Matrix4 r = Matrix4::GetIdentity();
    Matrix4 mat = t * r * s;
    g_theRenderer->SetModelMatrix(mat);
    g_theRenderer->SetMaterial(g_theRenderer->GetMaterial("Test"));
    g_theRenderer->DrawQuad2D();

    auto gif_dims = _gif_test->GetFrameDimensions();
    Matrix4 gif_s = Matrix4::CreateScaleMatrix(Vector2(static_cast<float>(gif_dims.x), static_cast<float>(gif_dims.y)) * 0.50f);
    Matrix4 gif_t = Matrix4::CreateTranslationMatrix(Vector2(-view_half_width, view_half_height) * 0.50f);
    Matrix4 gif_r = Matrix4::GetIdentity();
    Matrix4 gif_mat = gif_t * gif_r * gif_s;
    g_theRenderer->SetModelMatrix(gif_mat);

    auto tex_coords = _gif_test->GetCurrentTexCoords();
    Vector2 lefttop = Vector2(tex_coords.mins.x, tex_coords.mins.y);
    Vector2 rightbottom = Vector2(tex_coords.maxs.x, tex_coords.maxs.y);
    g_theRenderer->SetMaterial(_gif_test->GetMaterial());
    g_theRenderer->DrawQuad2D(Vector4(lefttop, rightbottom));

}

void Game::EndFrame() {
    /* DO NOTHING */
}

