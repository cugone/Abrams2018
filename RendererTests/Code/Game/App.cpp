#include "Game/App.hpp"

#include "Engine/Core/EngineSubsystem.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/TimeUtils.hpp"
#include "Engine/Core/System.hpp"

#include "Engine/Math/MathUtils.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Window.hpp"

#include "Engine/RHI/RHIOutput.hpp"

#include "Game/GameCommon.hpp"
#include "Game/GameConfig.hpp"

bool App::applet_mode = false;

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

App::App(std::unique_ptr<JobSystem>&& jobSystem, std::unique_ptr<FileLogger>&& fileLogger)
    : EngineSubsystem()
    , _theJobSystem(std::move(jobSystem))
    , _theFileLogger(std::move(fileLogger))
    , _theConfig(std::make_unique<Config>())
    , _theRenderer{ std::make_unique<Renderer>(static_cast<unsigned int>(GRAPHICS_OPTION_WINDOW_WIDTH), static_cast<unsigned int>(GRAPHICS_OPTION_WINDOW_HEIGHT)) }
    , _theInputSystem{ std::make_unique<InputSystem>() }
    , _theConsole{ std::make_unique<Console>(_theRenderer.get()) }
    , _theGame{ std::make_unique<Game>() }
{
    g_theJobSystem = _theJobSystem.get();
    g_theFileLogger = _theFileLogger.get();
    g_theConfig = _theConfig.get();
    g_theRenderer = _theRenderer.get();
    g_theInput = _theInputSystem.get();
    g_theConsole = _theConsole.get();
    g_theGame = _theGame.get();
}

App::~App() {
    _theGame.reset();
    _theConsole.reset();
    _theInputSystem.reset();
    _theRenderer.reset();
    _theConfig.reset();
    _theFileLogger.reset();
    _theJobSystem.reset();

    g_theJobSystem = nullptr;
    g_theFileLogger = nullptr;
    g_theGame = nullptr;
    g_theConsole = nullptr;
    g_theInput = nullptr;
    g_theRenderer = nullptr;
    g_theConfig = nullptr;

}

bool App::IsQuitting() const {
    return _isQuitting;
}

void App::SetIsQuitting(bool quit) {
    _isQuitting = quit;
}

void App::Initialize() {
    FileUtils::CreateFolders("Data/");
    {
        std::ostringstream ss;
        ss << System::GetSystemDesc();
        g_theFileLogger->LogAndFlush(ss.str());
    }
    g_theRenderer->Initialize(applet_mode);
    if(applet_mode) {
        return;
    }
    g_theRenderer->SetVSync(GRAPHICS_OPTION_VSYNC);
    g_theRenderer->SetWindowTitle("Test Title");
    g_theRenderer->SetWinProc(WindowProc);
    g_theRenderer->SetFullscreen(GRAPHICS_OPTION_FULLSCREEN);
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
    using namespace TimeUtils;

    BeginFrame();
    static FPSeconds previousFrameTime = TimeUtils::GetCurrentTimeElapsed();
    FPSeconds currentFrameTime = TimeUtils::GetCurrentTimeElapsed();
    FPSeconds deltaSeconds = (currentFrameTime - previousFrameTime);
    previousFrameTime = currentFrameTime;

    #if _DEBUG
    deltaSeconds = FPSeconds{std::clamp(FPFrames{ deltaSeconds }, FPFrames{ 0 }, FPFrames{ 1 })};
    #endif

    Update(deltaSeconds);
    Render();
    EndFrame();
}

bool App::HasFocus() const {
    return _current_focus;
}

bool App::LostFocus() const {
    return _previous_focus && !_current_focus;
}

bool App::GainedFocus() const {
    return !_previous_focus && _current_focus;
}

bool App::ProcessSystemMessage(const EngineMessage& msg) {

    WPARAM wp = msg.wparam;
    switch(msg.wmMessageCode) {
    case WindowsSystemMessage::Window_Close:
    case WindowsSystemMessage::Window_Quit:
    case WindowsSystemMessage::Window_Destroy:
    {
        SetIsQuitting(true);
        return true;
    }
    case WindowsSystemMessage::Window_ActivateApp:
    {
        bool losing_focus = wp == FALSE;
        bool gaining_focus = wp == TRUE;
        if(losing_focus) {
            _current_focus = false;
            _previous_focus = true;
        }
        if(gaining_focus) {
            _current_focus = true;
            _previous_focus = false;
        }
        return true;
    }
    case WindowsSystemMessage::Keyboard_Activate:
    {
        auto active_type = LOWORD(wp);
        switch(active_type) {
        case WA_ACTIVE: /* FALLTHROUGH */
        case WA_CLICKACTIVE:
            _current_focus = true;
            _previous_focus = false;
            return true;
        case WA_INACTIVE:
            _current_focus = false;
            _previous_focus = true;
            return true;
        default:
            return false;
        }
    }
    default:
        return false;
    }
}

void App::BeginFrame() {
    g_theJobSystem->BeginFrame();
    g_theInput->BeginFrame();
    g_theConsole->BeginFrame();
    g_theGame->BeginFrame();
    g_theRenderer->BeginFrame();
}

void App::Update([[maybe_unused]]TimeUtils::FPSeconds deltaSeconds) {
    g_theInput->Update(deltaSeconds);
    g_theConsole->Update(deltaSeconds);
    g_theGame->Update(deltaSeconds);
    g_theRenderer->Update(deltaSeconds);
}

void App::Render() const {
    g_theGame->Render();
    g_theConsole->Render();
    g_theInput->Render();
    g_theRenderer->Render();
}

void App::EndFrame() {
    g_theGame->EndFrame();
    g_theConsole->EndFrame();
    g_theInput->EndFrame();
    g_theRenderer->EndFrame();
}

