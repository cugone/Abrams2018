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

#include "Engine/UI/Canvas.hpp"

#include "Game/GameCommon.hpp"
#include "Game/GameConfig.hpp"

#include <algorithm>
#include <functional>
#include <sstream>

Game::Game() {
    _camera2 = new Camera2D;
}

Game::~Game() {
    delete _camera2;
    _camera2 = nullptr;

    delete _canvas;
    _canvas = nullptr;

}

void Game::Initialize() {
    g_theRenderer->RegisterTexturesFromFolder(std::string{"Data/Images"});
    g_theRenderer->RegisterMaterialsFromFolder(std::string{"Data/Materials"});
    g_theRenderer->RegisterFontsFromFolder(std::string{"Data/Fonts"});

    _canvas = new UI::Canvas(*g_theRenderer, nullptr, (std::min)(GRAPHICS_OPTION_WINDOW_WIDTH, GRAPHICS_OPTION_WINDOW_HEIGHT));
    _canvas->SetDebugColors(Rgba::CYAN, Rgba::GREEN, Rgba::RED);
    _canvas->SetPivot(_pivot_position);
}

void Game::BeginFrame() {
    /* DO NOTHING */
}

void Game::Update(float deltaSeconds) {
    if(g_theInput->WasKeyJustPressed(KeyCode::Esc)) {
        g_theApp->SetIsQuitting(true);
        return;
    }

    if(g_theInput->WasKeyJustPressed(KeyCode::Q)) {
        _canvas->SetPivot(--_pivot_position);
    } else if(g_theInput->WasKeyJustPressed(KeyCode::E)) {
        _canvas->SetPivot(++_pivot_position);
    }

    if(g_theInput->IsKeyDown(KeyCode::Up)) {
        _camera2->Translate(Vector2(0.0f, -_cameraSpeed));
    } else if(g_theInput->IsKeyDown(KeyCode::Down)) {
        _camera2->Translate(Vector2(0.0f, _cameraSpeed));
    }

    if(g_theInput->IsKeyDown(KeyCode::Left)) {
        _camera2->Translate(Vector2(-_cameraSpeed, 0.0f));
    } else if(g_theInput->IsKeyDown(KeyCode::Right)) {
        _camera2->Translate(Vector2(_cameraSpeed, 0.0f));
    }

    if(g_theInput->WasKeyJustPressed(KeyCode::R)) {
        _camera2->SetPosition(Vector2::ZERO);
        _canvas->SetPivot(UI::PivotPosition::Center);
    }

    if(g_theInput->WasKeyJustPressed(KeyCode::F1)) {
        _debug = !_debug;
    }

    _camera2->Update(deltaSeconds);
    _canvas->Update(deltaSeconds);
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

    g_theRenderer->SetProjectionMatrix(_camera2->GetProjectionMatrix());

    DrawPivotPositionText(Vector2(view_leftBottom.x, view_rightTop.y));

    g_theRenderer->SetViewMatrix(_camera2->GetViewMatrix());

    _canvas->Render(g_theRenderer);
    if(_debug) {
        _canvas->DebugRender(g_theRenderer);
    }
}

void Game::DrawPivotPositionText(const Vector2 &position) const {
    auto f = g_theRenderer->GetFont("System32");
    Vector2 leftTop = Vector2(position.x + 1, position.y + f->GetLineHeight());
    g_theRenderer->SetModelMatrix(Matrix4::CreateTranslationMatrix(leftTop));
    g_theRenderer->SetMaterial(g_theRenderer->GetMaterial("Font_System32"));
    switch(_pivot_position) {
        case UI::PivotPosition::Center:
            g_theRenderer->DrawTextLine(f, "Pivot Position: CENTER");
            break;
        case UI::PivotPosition::TopLeft:
            g_theRenderer->DrawTextLine(f, "Pivot Position: TOP LEFT");
            break;
        case UI::PivotPosition::Top:
            g_theRenderer->DrawTextLine(f, "Pivot Position: TOP");
            break;
        case UI::PivotPosition::TopRight:
            g_theRenderer->DrawTextLine(f, "Pivot Position: TOP RIGHT");
            break;
        case UI::PivotPosition::Right:
            g_theRenderer->DrawTextLine(f, "Pivot Position: RIGHT");
            break;
        case UI::PivotPosition::BottomRight:
            g_theRenderer->DrawTextLine(f, "Pivot Position: BOTTOM RIGHT");
            break;
        case UI::PivotPosition::Bottom:
            g_theRenderer->DrawTextLine(f, "Pivot Position: BOTTOM");
            break;
        case UI::PivotPosition::BottomLeft:
            g_theRenderer->DrawTextLine(f, "Pivot Position: BOTTOM LEFT");
            break;
        case UI::PivotPosition::Left:
            g_theRenderer->DrawTextLine(f, "Pivot Position: LEFT");
            break;
        default:
            g_theRenderer->DrawTextLine(f, "Pivot Position: NONE");
            break;
    }
    {
        g_theRenderer->SetModelMatrix(Matrix4::CreateTranslationMatrix(Vector2(leftTop.x, leftTop.y + f->GetLineHeight())));
        std::ostringstream ss;
        ss << _canvas->GetPivot();
        g_theRenderer->DrawTextLine(f, ss.str());
    }
}

void Game::EndFrame() {
    /* DO NOTHING */
}

struct generate_image_job_t {
    Image* img = nullptr;
    unsigned int width = 0;
    unsigned int height = 0;
    std::string filepath{};
};

void Game::DoExport() {
    auto job_data = new generate_image_job_t;
    static int index = 0;
    job_data->width = 1600;
    job_data->height = 900;
    std::filesystem::path p("Data/Images/Test_");
    p += std::to_string(index++);
    p += ".png";
    p.make_preferred();
    job_data->filepath = p.string();
    g_theJobSystem->Run(JobType::Generic, [this](void* user_data) { this->GenerateImageData(user_data); }, job_data);
}

void Game::GenerateImageData(void* data) {
    auto width = ((generate_image_job_t*)data)->width;
    auto height = ((generate_image_job_t*)data)->height;
    std::vector<Rgba> img_data;
    img_data.resize(width * height);
    std::generate(std::begin(img_data), std::end(img_data), Rgba::Random);
    Image* img = new Image(img_data, width, height);
    ((generate_image_job_t*)data)->img = img;
    auto image = ((generate_image_job_t*)data)->img;
    auto filepath = ((generate_image_job_t*)data)->filepath;
    image->Export(filepath);
}
