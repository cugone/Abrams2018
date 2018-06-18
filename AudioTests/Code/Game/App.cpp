#include "Game/App.hpp"

#include "Engine/Audio/AudioSystem.hpp"

#include "Engine/Core/EngineSubsystem.hpp"

#include "Engine/Core/TimeUtils.hpp"
#include "Engine/Core/FileUtils.hpp"

#include "Engine/Math/MathUtils.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Window.hpp"

#include "Engine/RHI/RHIOutput.hpp"

#include "Game/GameCommon.hpp"
#include "Game/GameConfig.hpp"

bool CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    EngineMessage msg = {};
    msg.hWnd = hwnd;
    msg.wmMessageCode = EngineSubsystem::GetWindowsSystemMessageFromUintMessage(uMsg);
    msg.wparam = wParam;
    msg.lparam = lParam;

    if(g_theSubsystemHead && g_theSubsystemHead->EngineSubsystem::ProcessSystemMessage(msg)) {
        return true;
    }
    return false;
}

App::App() {
    g_theRenderer = new Renderer(static_cast<unsigned int>(GRAPHICS_OPTION_WINDOW_WIDTH), static_cast<unsigned int>(GRAPHICS_OPTION_WINDOW_HEIGHT));
    g_theAudio = new AudioSystem();
    g_theInput = new InputSystem();
    g_theConsole = new Console(g_theRenderer);
    g_theGame = new Game();
}

App::~App() {
    delete g_theGame;
    g_theGame = nullptr;

    delete g_theConsole;
    g_theConsole = nullptr;

    delete g_theInput;
    g_theInput = nullptr;
    
    delete g_theAudio;
    g_theAudio = nullptr;

    delete g_theRenderer;
    g_theRenderer = nullptr;

}

bool App::IsQuitting() const {
    return _isQuitting;
}

void App::SetIsQuitting(bool quit) {
    _isQuitting = quit;
}

void App::Initialize() {
    FileUtils::CreateFolders("Data/");
    g_theRenderer->Initialize();
    g_theRenderer->SetVSync(GRAPHICS_OPTION_VSYNC);
    if(g_theRenderer->GetOutput()) {
        Window* window = g_theRenderer->GetOutput()->GetWindow();
        if(window) {
            window->custom_message_handler = WindowProc;
            window->SetTitle("Test Title");
            bool is_fullscreen = false;
            if(is_fullscreen) {
                window->SetDisplayMode(RHIOutputMode::FULLSCREEN_WINDOW);
            }
        }
    }
    g_theAudio->Initialize();
    g_theInput->Initialize();
    g_theConsole->Initialize();
    g_theGame->Initialize();

    Console::Command quit{};
    quit.command_name = "quit";
    quit.help_text_short = "Quits the application.";
    quit.help_text_long = "Quits the application.";
    quit.command_function = [this](const std::string& /*args*/) { this->SetIsQuitting(true); };
    g_theConsole->RegisterCommand(quit);

}

void App::RunFrame() {
    using namespace std::chrono;

    BeginFrame();

    static float previousFrameTime = TimeUtils::GetCurrentTimeElapsed<duration<float>>();
    float currentFrameTime = TimeUtils::GetCurrentTimeElapsed<duration<float>>();
    float deltaSeconds = currentFrameTime - previousFrameTime;
    previousFrameTime = currentFrameTime;

    deltaSeconds = MathUtils::Clamp(deltaSeconds, 0.0f, 0.16f);

    Update(deltaSeconds);
    Render();
    EndFrame();
}

bool App::ProcessSystemMessage(const EngineMessage& msg) {
    switch(msg.wmMessageCode) {
        case WindowsSystemMessage::WINDOW_CLOSE:
        case WindowsSystemMessage::WINDOW_QUIT:
        case WindowsSystemMessage::WINDOW_DESTROY:
        {
            SetIsQuitting(true);
            return true;
        }
        default:
            return false;
    }
}

void App::BeginFrame() {
    g_theAudio->BeginFrame();
    g_theInput->BeginFrame();
    g_theConsole->BeginFrame();
    g_theGame->BeginFrame();
    g_theRenderer->BeginFrame();
}

void App::Update(float deltaSeconds) {
    g_theAudio->Update(deltaSeconds);
    g_theInput->Update(deltaSeconds);
    g_theConsole->Update(deltaSeconds);
    g_theGame->Update(deltaSeconds);
    g_theRenderer->Update(deltaSeconds);
}

void App::Render() const {
    g_theGame->Render();
    g_theConsole->Render();
    g_theInput->Render();
    g_theAudio->Render();
    g_theRenderer->Render();
}

void App::EndFrame() {
    g_theGame->EndFrame();
    g_theConsole->EndFrame();
    g_theInput->EndFrame();
    g_theAudio->EndFrame();
    g_theRenderer->EndFrame();
}