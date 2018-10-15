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
#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Renderer/RasterState.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Texture1D.hpp"
#include "Engine/Renderer/Texture2D.hpp"
#include "Engine/Renderer/Texture3D.hpp"
#include "Engine/Renderer/Window.hpp"

#include "Engine/RHI/RHIDeviceContext.hpp"
#include "Engine/RHI/RHIOutput.hpp"

#include "Engine/Profiling/Profiler.hpp"

#include "Engine/UI/UI.hpp"

#include "Game/GameCommon.hpp"
#include "Game/GameConfig.hpp"

#include <algorithm>
#include <functional>
#include <numeric>
#include <sstream>

Game::Game() {
    _camera2 = new Camera2D;
    _camera3 = new Camera3D;
}

Game::~Game() {

    delete _health_cb;
    _health_cb = nullptr;

    delete _camera3;
    _camera3 = nullptr;

    delete _camera2;
    _camera2 = nullptr;

}

void Game::Initialize() {
    InitializeData();
    InitializeUI();
}

void Game::InitializeData() {
    g_theRenderer->RegisterTexturesFromFolder(std::string{ "Data/Images" });
    g_theRenderer->RegisterShadersFromFolder(std::string{ "Data/Shaders" });
    g_theRenderer->RegisterMaterialsFromFolder(std::string{ "Data/Materials" });
    g_theRenderer->RegisterFontsFromFolder(std::string{ "Data/Fonts" });

    _health_cb = g_theRenderer->CreateConstantBuffer(&health_data, sizeof(health_data));
    _camera3->SetPosition(Vector3(0.0f, 5.0f, -10.0f));
}

void Game::InitializeUI() {
    /* DO NOTHING */
}

void Game::BeginFrame() {
    /* DO NOTHING */
}

void Game::Update(float deltaSeconds) {
    if(g_theInput->WasKeyJustPressed(KeyCode::Esc)) {
        g_theApp->SetIsQuitting(true);
        return;
    }

    _slowmo = false;
    if(g_theInput->IsKeyDown(KeyCode::RCtrl)) {
        _slowmo = true;
    }

    g_theRenderer->UpdateGameTime(_slowmo ? deltaSeconds * 0.10f : deltaSeconds);
    UpdateCameraFromKeyboard(deltaSeconds);
    UpdateCameraFromMouse(deltaSeconds);

    if(g_theInput->WasKeyJustPressed(KeyCode::F1)) {
        _debug = !_debug;
    }

    if(g_theInput->WasKeyJustPressed(KeyCode::F2)) {
        DoExport();
    }

    if(g_theInput->WasKeyJustPressed(KeyCode::L)) {
        g_theJobSystem->Run(JobType::Generic, [this](void*) {
            if(!(_obj.IsLoaded() || _obj.IsLoading())) {
                _obj.Load(std::string{ "Data/Models/suzanne.obj" });
            }
        }, nullptr);
        
    }

    if(g_theInput->WasKeyJustPressed(KeyCode::M)) {
        g_theJobSystem->Run(JobType::Generic, [this](void*) {
            if(!_obj.IsSaving()) {
                _obj.Save(std::string{ "Data/Models/suzanne_out.obj" });
            }
        }, nullptr);
    }

    if(g_theInput->WasKeyJustPressed(KeyCode::F3)) {
        _wireframe_mode = !_wireframe_mode;
    }

    if(g_theInput->IsKeyDown(KeyCode::Up)) {
        health_data.health_percentage += 0.1f * deltaSeconds;
    } else if(g_theInput->IsKeyDown(KeyCode::Down)) {
        health_data.health_percentage -= 0.1f * deltaSeconds;
    }
    health_data.health_percentage = std::clamp(health_data.health_percentage, 0.0f, 1.0f);
    _health_cb->Update(g_theRenderer->GetDeviceContext(), &health_data);

    _camera2->Update(deltaSeconds);
    _camera3->Update(deltaSeconds);

}

void Game::UpdateCameraFromKeyboard(float deltaSeconds) {

    bool is_fast = false;
    if(g_theInput->IsKeyDown(KeyCode::Shift)) {
        is_fast = true;
    }

    float camera_move_speed = _cameraSpeed * deltaSeconds * (is_fast ? _camera_move_speed_multiplier : 1.0f);
    if(g_theInput->IsKeyDown(KeyCode::W)) {
        _camera3->Translate(_camera3->GetForward() * camera_move_speed);
    } else if(g_theInput->IsKeyDown(KeyCode::S)) {
        _camera3->Translate(-_camera3->GetForward() * camera_move_speed);
    }

    if(g_theInput->IsKeyDown(KeyCode::A)) {
        _camera3->Translate(-_camera3->GetRight() * camera_move_speed);
    } else if(g_theInput->IsKeyDown(KeyCode::D)) {
        _camera3->Translate(_camera3->GetRight() * camera_move_speed);
    }

    if(g_theInput->IsKeyDown(KeyCode::Q)) {
        Vector3 angles = Vector3{ _camera3->GetPitchDegrees(), _camera3->GetYawDegrees(), _camera3->GetRollDegrees() - 1.0f };
        _camera3->SetEulerAnglesDegrees(angles);
    } else if(g_theInput->IsKeyDown(KeyCode::E)) {
        Vector3 angles = Vector3{ _camera3->GetPitchDegrees(), _camera3->GetYawDegrees(), _camera3->GetRollDegrees() + 1.0f };
        _camera3->SetEulerAnglesDegrees(angles);
    }

    if(g_theInput->IsKeyDown(KeyCode::Space)) {
        _camera3->Translate(_camera3->GetUp() * camera_move_speed);
    } else if(g_theInput->IsKeyDown(KeyCode::Ctrl)) {
        _camera3->Translate(-_camera3->GetUp() * camera_move_speed);
    }

    if(g_theInput->WasKeyJustPressed(KeyCode::R)) {
        _camera3->SetPosition(Vector3::ZERO);
        _camera3->SetEulerAngles(Vector3{0.0f, 0.0f, 0.0f});
        _camera2->SetPosition(Vector2::ZERO);
        _camera2->SetRotationDegrees(0.0f);
        health_data.health_percentage = 0.0f;
    }

}

void Game::UpdateCameraFromMouse(float /*deltaSeconds*/) {
    if(g_theApp->HasFocus()) {
        const auto& window = *(g_theRenderer->GetOutput()->GetWindow());
        auto mouse_pos = g_theInput->GetCursorWindowPosition(window);
        g_theInput->SetCursorToWindowCenter(window);
        auto mouse_delta_pos = (mouse_pos - g_theInput->GetCursorWindowPosition(window));
        auto moved_x = mouse_delta_pos.x;
        auto moved_y = mouse_delta_pos.y;
        Vector3 angles = Vector3{ _camera3->GetPitchDegrees() - moved_y, _camera3->GetYawDegrees() - moved_x, _camera3->GetRollDegrees() };
        _camera3->SetEulerAnglesDegrees(angles);
    }
}

void Game::Render() const {
    g_theRenderer->SetRenderTarget();
    g_theRenderer->ClearColor(Rgba::BLACK);
    g_theRenderer->ClearDepthStencilBuffer();

    g_theRenderer->SetViewport(0, 0, static_cast<unsigned int>(GRAPHICS_OPTION_WINDOW_WIDTH), static_cast<unsigned int>(GRAPHICS_OPTION_WINDOW_HEIGHT));

    g_theRenderer->SetModelMatrix(Matrix4::GetIdentity());
    g_theRenderer->SetViewMatrix(Matrix4::GetIdentity());
    g_theRenderer->SetProjectionMatrix(Matrix4::GetIdentity());
    _camera3->SetupView(45.0f, MathUtils::M_16_BY_9_RATIO, 0.01f, 1000.0f);

    g_theRenderer->SetProjectionMatrix(_camera3->GetProjectionMatrix());
    g_theRenderer->SetViewMatrix(_camera3->GetViewMatrix());

    g_theRenderer->SetAmbientLight(Rgba::WHITE, 5.0f);
    g_theRenderer->SetLightingEyePosition(_camera3->GetPosition());

    DirectionalLightDesc dl_desc{};
    dl_desc.color = Rgba::RED;
    dl_desc.direction = -Vector3::Y_AXIS;
    dl_desc.intensity = 10.0f;
    g_theRenderer->SetDirectionalLight(0, dl_desc);

    RenderStuff();

    const auto& window_dimensions = g_theRenderer->GetOutput()->GetDimensions();
    auto window_width = static_cast<float>(window_dimensions.x);
    auto window_height = static_cast<float>(window_dimensions.y);
    float view_half_width = window_width * 0.50f;
    float view_half_height = window_height * 0.50f;

    Vector2 view_leftBottom = Vector2(-view_half_width, view_half_height);
    Vector2 view_rightTop = Vector2(view_half_width, -view_half_height);
    Vector2 view_nearFar = Vector2(0.0f, 1.0f);
    Vector2 cam_pos2 = Vector2(_camera2->GetPosition());

    _camera2->SetupView(view_leftBottom, view_rightTop, view_nearFar, MathUtils::M_16_BY_9_RATIO);
    g_theRenderer->SetViewMatrix(_camera2->GetViewMatrix());
    g_theRenderer->SetProjectionMatrix(_camera2->GetProjectionMatrix());

    auto font = g_theRenderer->GetFont("System32");
    g_theRenderer->SetMaterial(font->GetMaterial());
    {
        std::ostringstream ss;
        ss << "Health" << std::fixed << std::setprecision(1) << std::setw(4) << health_data.health_percentage * 100.0f << '%';
        ss << "\nPos:" << _camera3->GetPosition();
        ss << "\nAngles:" << _camera3->GetEulerAngles();
        ss << "\nLoading: " << std::boolalpha << _obj.IsLoading() << std::noboolalpha;
        ss << "\nLoaded: " << std::boolalpha << _obj.IsLoaded() << std::noboolalpha;
        ss << "\nSaving: " << std::boolalpha << _obj.IsSaving() << std::noboolalpha;
        ss << "\nSaved: " << std::boolalpha << _obj.IsSaved() << std::noboolalpha;
        ss << "\ntime: " << g_theRenderer->GetGameTime();
        Matrix4 T = Matrix4::GetIdentity();
        g_theRenderer->SetModelMatrix(T);
        g_theRenderer->DrawMultilineText(font, ss.str());
    }
}

void Game::RenderStuff() const {
    DrawCube();
    DrawObj();
    DrawWorldGrid();
    DrawAxes();

    g_theRenderer->EnableDepth();
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

void Game::DrawWorldGrid() const {
    if(!_debug) {
        return;
    }
    g_theRenderer->DrawWorldGridXZ();
}

void Game::DrawAxes() const {
    if(!_debug) {
        return;
    }
    g_theRenderer->DrawAxes();
}

void Game::DrawCube() const {
    auto aabb2 = AABB2::ZERO_TO_ONE;
    auto mins = aabb2.mins;
    auto maxs = aabb2.maxs;
    auto tex_left_bottom  = Vector2{mins.x, maxs.y};
    auto tex_left_top     = Vector2{mins.x, mins.y};
    auto tex_right_top    = Vector2{maxs.x, mins.y};
    auto tex_right_bottom = Vector2{maxs.x, maxs.y};
    std::vector<Vertex3D> vbo = {
        //Bottom
        Vertex3D(Vector3(-0.5f, -0.5f,  0.5f), Rgba::WHITE, tex_left_bottom, -Vector3::Y_AXIS),
        Vertex3D(Vector3(-0.5f, -0.5f, -0.5f), Rgba::WHITE, tex_left_top, -Vector3::Y_AXIS),
        Vertex3D(Vector3(0.5f,  -0.5f, -0.5f), Rgba::WHITE, tex_right_top, -Vector3::Y_AXIS),
        Vertex3D(Vector3(0.5f,  -0.5f,  0.5f), Rgba::WHITE, tex_right_bottom, -Vector3::Y_AXIS),
        //Top
        Vertex3D(Vector3(-0.5f, 0.5f, -0.5f), Rgba::WHITE, tex_left_bottom, Vector3::Y_AXIS),
        Vertex3D(Vector3(-0.5f, 0.5f, 0.5f), Rgba::WHITE,  tex_left_top, Vector3::Y_AXIS),
        Vertex3D(Vector3(0.5f,  0.5f, 0.5f), Rgba::WHITE,  tex_right_top, Vector3::Y_AXIS),
        Vertex3D(Vector3(0.5f,  0.5f, -0.5f), Rgba::WHITE, tex_right_bottom, Vector3::Y_AXIS),
        //Left
        Vertex3D(Vector3(-0.5f, 0.5f, -0.5f), Rgba::WHITE,  tex_right_top, -Vector3::X_AXIS),
        Vertex3D(Vector3(-0.5f, -0.5f, -0.5f), Rgba::WHITE, tex_right_bottom, -Vector3::X_AXIS),
        Vertex3D(Vector3(-0.5f, -0.5f, 0.5f), Rgba::WHITE,  tex_left_bottom, -Vector3::X_AXIS),
        Vertex3D(Vector3(-0.5f, 0.5f, 0.5f), Rgba::WHITE,   tex_left_top, -Vector3::X_AXIS),
        //Right
        Vertex3D(Vector3(0.5f, -0.5f, -0.5f), Rgba::WHITE, tex_left_bottom, Vector3::X_AXIS),
        Vertex3D(Vector3(0.5f,  0.5f, -0.5f), Rgba::WHITE, tex_left_top, Vector3::X_AXIS),
        Vertex3D(Vector3(0.5f,  0.5f,  0.5f), Rgba::WHITE, tex_right_top, Vector3::X_AXIS),
        Vertex3D(Vector3(0.5f, -0.5f,  0.5f), Rgba::WHITE, tex_right_bottom, Vector3::X_AXIS),
        //Front
        Vertex3D(Vector3(-0.5f, -0.5f, -0.5f), Rgba::WHITE, tex_left_bottom, -Vector3::Z_AXIS),
        Vertex3D(Vector3(-0.5f,  0.5f, -0.5f), Rgba::WHITE, tex_left_top, -Vector3::Z_AXIS),
        Vertex3D(Vector3(0.5f,   0.5f, -0.5f), Rgba::WHITE, tex_right_top, -Vector3::Z_AXIS),
        Vertex3D(Vector3(0.5f,  -0.5f, -0.5f), Rgba::WHITE, tex_right_bottom, -Vector3::Z_AXIS),
        //Back
        Vertex3D(Vector3(0.5f,  -0.5f, 0.5f), Rgba::WHITE, tex_left_bottom, Vector3::Z_AXIS),
        Vertex3D(Vector3(0.5f,   0.5f, 0.5f), Rgba::WHITE, tex_left_top, Vector3::Z_AXIS),
        Vertex3D(Vector3(-0.5f,  0.5f, 0.5f), Rgba::WHITE, tex_right_top, Vector3::Z_AXIS),
        Vertex3D(Vector3(-0.5f, -0.5f, 0.5f), Rgba::WHITE, tex_right_bottom, Vector3::Z_AXIS),
    };

    std::vector<unsigned int> ibo = {
         0,  1,  2,  0,  2,  3,
         4,  5,  6,  4,  6,  7,
         8,  9, 10,  8, 10, 11,
        12, 13, 14, 12, 14, 15,
        16, 17, 18, 16, 18, 19,
        20, 21, 22, 20, 22, 23,
    };

    g_theRenderer->SetModelMatrix(Matrix4::GetIdentity());
    g_theRenderer->SetMaterial(g_theRenderer->GetMaterial("Test"));
    g_theRenderer->DrawIndexed(PrimitiveType::Triangles, vbo, ibo);
}

void Game::DrawObj() const {
    if(_obj.IsLoaded()) {
        g_theRenderer->SetAmbientLight(Rgba::WHITE, 0.25f);
        SpotLightDesc sl_desc{};
        sl_desc.color = Rgba::WHITE;
        sl_desc.direction = _camera3->GetForward();
        sl_desc.position = _camera3->GetPosition();
        sl_desc.intensity = 1.0f;
        sl_desc.inner_outer_anglesDegrees = Vector2{10.0f, 30.0f};
        g_theRenderer->SetSpotlight(2, sl_desc);
        Matrix4 T = Matrix4::GetIdentity();
        Matrix4 R = Matrix4::GetIdentity();
        Matrix4 S = Matrix4::CreateScaleMatrix(10.0f);
        Matrix4 M = T * R * S;
        g_theRenderer->SetModelMatrix(M);
        g_theRenderer->SetMaterial(g_theRenderer->GetMaterial("__default"));
        auto raster = _wireframe_mode ? g_theRenderer->GetRasterState("__wireframe")
                                      : g_theRenderer->GetRasterState("__solid");
        g_theRenderer->SetRasterState(raster);
        g_theRenderer->SetUseVertexNormalsForLighting(true);
        g_theRenderer->DrawIndexed(PrimitiveType::Triangles, _obj.GetVbo(), _obj.GetIbo());
    }
}
