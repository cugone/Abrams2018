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

#include "Engine/UI/UI.hpp"

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

    delete _cnvFullscreen;
    _cnvFullscreen = nullptr;

}

void Game::Initialize() {
    InitializeData();
    InitializeUI();
}

void Game::InitializeData() {
    g_theRenderer->RegisterTexturesFromFolder(std::string{ "Data/Images" });
    g_theRenderer->RegisterMaterialsFromFolder(std::string{ "Data/Materials" });
    g_theRenderer->RegisterFontsFromFolder(std::string{ "Data/Fonts" });
}

void Game::InitializeUI() {
    _cnvFullscreen = new UI::Canvas(*g_theRenderer, nullptr, (std::min)(GRAPHICS_OPTION_WINDOW_WIDTH, GRAPHICS_OPTION_WINDOW_HEIGHT));
    _cnvFullscreen->SetDebugColors(Rgba::CYAN, Rgba::NOALPHA);
    _cnvFullscreen->SetPivot(UI::PivotPosition::Center);

    _lblPivotName = new UI::Label(_cnvFullscreen);
    _lblPivotName->SetDebugColors(Rgba::BLACK, Rgba::NOALPHA, Rgba::CYAN);
    _lblPivotName->SetPivot(UI::PivotPosition::Center);
    _lblPivotName->SetText("Hello World");
    _lblPivotName->SetFont(g_theRenderer->GetFont("System32"));
    _lblPivotName->SetPosition(UI::Metric{ UI::Ratio{Vector2(0.0f, 0.0f)}, Vector2::ZERO });
    _cnvFullscreen->AddChild(_lblPivotName);

    _lblPivotPosition = new UI::Label(_cnvFullscreen);
    _lblPivotPosition->SetFont(g_theRenderer->GetFont("System32"));
    _lblPivotPosition->SetDebugColors(Rgba::BLACK, Rgba::NOALPHA, Rgba::CYAN);
    _lblPivotPosition->SetPivot(UI::PivotPosition::Center);
    _lblPivotPosition->SetText("Hello World");
    _lblPivotPosition->SetPosition(UI::Metric{ UI::Ratio{Vector2(0.0f, 0.0f)}, Vector2(0.0f, 0.1f) });//_lblPivotName->GetSize().y) });
    _cnvFullscreen->AddChild(_lblPivotPosition);

    SetPivotPositionText();
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
        _lblPivotName->SetPivot(--_pivot_position);
        SetPivotPositionText();
    } else if(g_theInput->WasKeyJustPressed(KeyCode::E)) {
        _lblPivotName->SetPivot(++_pivot_position);
        SetPivotPositionText();
    }
    
    if(g_theInput->WasKeyJustPressed(KeyCode::R)) {
        _camera2->SetPosition(Vector2::ZERO);
        
        _cnvFullscreen->SetOrientationDegrees(0.0f);
        _cnvFullscreen->SetPivot(UI::PivotPosition::TopLeft);
        _cnvFullscreen->SetPosition(UI::Metric{ UI::Ratio{}, Vector2::ZERO });

        _lblPivotPosition->SetOrientationDegrees(0.0f);
        _lblPivotPosition->SetPivot(UI::PivotPosition::BottomLeft);
        _lblPivotPosition->SetPosition(UI::Metric{ UI::Ratio{Vector2{0.0f, 0.0f}}, Vector2::ZERO });

        _lblPivotName->SetOrientationDegrees(0.0f);
        _lblPivotName->SetPivot(UI::PivotPosition::BottomLeft);
        _lblPivotName->SetPosition(UI::Metric{ UI::Ratio{Vector2{0.0f, 0.0f}}, Vector2::ZERO });

    }

    if(g_theInput->WasKeyJustPressed(KeyCode::F1)) {
        _debug = !_debug;
    }

    if(g_theInput->WasKeyJustPressed(KeyCode::F2)) {
        DoExport();
    }

    _camera2->Update(deltaSeconds);
    _cnvFullscreen->Update(deltaSeconds);
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

    g_theRenderer->SetViewMatrix(_camera2->GetViewMatrix());

    _cnvFullscreen->Render(g_theRenderer);
    if(_debug) {
        _cnvFullscreen->DebugRender(g_theRenderer);
    }
}

void Game::SetPivotPositionText() const {
    switch(_pivot_position) {
        case UI::PivotPosition::Center:
            _lblPivotName->SetText("Pivot Position: CENTER");
            break;
        case UI::PivotPosition::TopLeft:
            _lblPivotName->SetText("Pivot Position: TOP LEFT");
            break;
        case UI::PivotPosition::Top:
            _lblPivotName->SetText("Pivot Position: TOP");
            break;
        case UI::PivotPosition::TopRight:
            _lblPivotName->SetText("Pivot Position: TOP RIGHT");
            break;
        case UI::PivotPosition::Right:
            _lblPivotName->SetText("Pivot Position: RIGHT");
            break;
        case UI::PivotPosition::BottomRight:
            _lblPivotName->SetText("Pivot Position: BOTTOM RIGHT");
            break;
        case UI::PivotPosition::Bottom:
            _lblPivotName->SetText("Pivot Position: BOTTOM");
            break;
        case UI::PivotPosition::BottomLeft:
            _lblPivotName->SetText("Pivot Position: BOTTOM LEFT");
            break;
        case UI::PivotPosition::Left:
            _lblPivotName->SetText("Pivot Position: LEFT");
            break;
        default:
            _lblPivotName->SetText("Pivot Position: NONE");
            break;
    }
    {
        std::ostringstream ss;
        ss << _lblPivotName->GetPivot();
        _lblPivotPosition->SetText(ss.str());
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
