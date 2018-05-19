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
    g_theRenderer->SetModelMatrix(Matrix4::GetIdentity());
    g_theRenderer->SetViewMatrix(Matrix4::GetIdentity());
    g_theRenderer->SetProjectionMatrix(Matrix4::GetIdentity());

    g_theRenderer->SetMaterial(g_theRenderer->GetMaterial("__default"));
    Vector3 pos0 = Vector3(-1.0f, -1.0f, 0.0f);
    Vector3 pos1 = Vector3(-1.0f, 1.0f, 0.0f);
    Vector3 pos2 = Vector3(1.0f, 1.0f, 0.0f);
    Vector3 pos3 = Vector3(1.0f, -1.0f, 0.0f);
    std::vector<Vertex3D> vbo{ Vertex3D(pos0, Rgba::RED),  Vertex3D(pos1, Rgba::GREEN), Vertex3D(pos2, Rgba::BLUE), Vertex3D(pos3, Rgba::RED) };
    std::vector<unsigned int> ibo{
        0, 1, 2
        ,0, 2, 3
    };
    //g_theRenderer->Draw(PrimitiveType::TRIANGLES, vbo);
    g_theRenderer->DrawIndexed(PrimitiveType::TRIANGLES, vbo, ibo);

}

void Game::EndFrame() {
    /* DO NOTHING */
}

