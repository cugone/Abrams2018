#include "Game/Game.hpp"

#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/MathUtils.hpp"

#include "Game/GameCommon.hpp"
#include "Game/GameConfig.hpp"

void Game::Initialize() {
    /* DO NOTHING */
}

void Game::BeginFrame() {
    /* DO NOTHING */
}

void Game::Update(float /*deltaSeconds*/) {

    if(g_theInput->WasKeyJustPressed(KeyCode::ESC)) {
        g_theApp->SetIsQuitting(true);
        return;
    }

}

void Game::Render() const {

    g_theRenderer->SetRenderTarget(nullptr);
    g_theRenderer->ClearColor(Rgba::BLACK);
    g_theRenderer->ClearDepthStencilBuffer();

    g_theRenderer->SetViewport(0, 0, static_cast<unsigned int>(GRAPHICS_OPTION_WINDOW_WIDTH), static_cast<unsigned int>(GRAPHICS_OPTION_WINDOW_HEIGHT));

    //Vector2 leftBottom = Vector2(0.0f, GRAPHICS_OPTION_WINDOW_HEIGHT);
    //Vector2 rightTop = Vector2(GRAPHICS_OPTION_WINDOW_WIDTH, 0.0f);
    //g_theRenderer->SetOrthoProjection(leftBottom, rightTop, Vector2(0.0f, 1.0f));


    g_theRenderer->SetMaterial(g_theRenderer->GetMaterial("__default"));
    Vector3 pos = Vector3::ZERO;//(0.50f, 0.50f, 0.0f);//Vector2(GRAPHICS_OPTION_WINDOW_WIDTH * 0.25f, GRAPHICS_OPTION_WINDOW_HEIGHT * 0.50f), 0.0f);
    std::vector<Vertex3D> vbo{ Vertex3D(pos) };
    g_theRenderer->SetModelMatrix(Matrix4::GetIdentity());//Matrix4::CreateTranslationMatrix(Vector3(pos)));
    g_theRenderer->SetViewMatrix(Matrix4::GetIdentity());//Matrix4::CreateTranslationMatrix(Vector3(pos)));
    g_theRenderer->SetProjectionMatrix(Matrix4::GetIdentity());//Matrix4::CreateTranslationMatrix(Vector3(pos)));
    g_theRenderer->Draw(PrimitiveType::POINTS, vbo);
    //Vector3 pos1 = Vector3(Vector2(GRAPHICS_OPTION_WINDOW_WIDTH * 0.25f, GRAPHICS_OPTION_WINDOW_HEIGHT * 0.50f), 0.0f);
    //Vector3 pos2 = Vector3(Vector2(GRAPHICS_OPTION_WINDOW_WIDTH * 0.75f, GRAPHICS_OPTION_WINDOW_HEIGHT * 0.50f), 0.0f);
    ////g_theRenderer->DrawLine(pos1, pos2);
    //std::vector<Vertex3D> vbo{ Vertex3D(pos1), Vertex3D(pos2) };
    //g_theRenderer->Draw(PrimitiveType::LINES, vbo);

}

void Game::EndFrame() {
    /* DO NOTHING */
}

