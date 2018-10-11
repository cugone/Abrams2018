
#include "Engine/Core/ArgumentParser.hpp"
#include "Engine/Core/KeyValueParser.hpp"
#include "Engine/Core/Config.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Win.hpp"

#include "Engine/Renderer/Window.hpp"

#include "Engine/RHI/RHIOutput.hpp"

#include "Game/GameCommon.hpp"
#include "Game/GameConfig.hpp"

#include <sstream>

void Initialize(HINSTANCE hInstance, LPSTR lpCmdLine, int nShowCmd);
void MainLoop();
void RunMessagePump();
void Shutdown();

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR lpCmdLine, int nShowCmd) {

    Initialize(hInstance, lpCmdLine, nShowCmd);
    MainLoop();
    Shutdown();
    return 0;
}

void Initialize(HINSTANCE /*hInstance*/, LPSTR lpCmdLine, int /*nShowCmd*/) {



    std::ostringstream ss_cmdLine{};
    KeyValueParser kvp{ lpCmdLine };
    Config cmdLine{ std::move(kvp) };
    cmdLine.PrintConfigs(ss_cmdLine);
    cmdLine.GetValue("vsync", GRAPHICS_OPTION_VSYNC);
    cmdLine.GetValue("width", GRAPHICS_OPTION_WINDOW_WIDTH);
    cmdLine.GetValue("height", GRAPHICS_OPTION_WINDOW_HEIGHT);
    std::string logName{"game"};
    cmdLine.GetValue("log", logName);
    g_theJobSystem = std::make_unique<JobSystem>(-1, static_cast<std::size_t>(JobType::Max), new std::condition_variable);
    g_theFileLogger = std::make_unique<FileLogger>(*g_theJobSystem, logName);
    g_theApp = std::make_unique<App>(*g_theJobSystem);

    g_theConsole->SetNextHandler(g_theInput);
    g_theInput->SetNextHandler(g_theApp.get());

    g_theSubsystemHead = g_theConsole;

    g_theFileLogger->LogLineAndFlush("---COMMAND LINE PARAMETERS---");
    g_theFileLogger->LogLineAndFlush(ss_cmdLine.str());
    g_theFileLogger->LogLineAndFlush("---COMMAND LINE PARAMETERS---");
    g_theApp->Initialize();
}

void MainLoop() {
    if(g_theApp->applet_mode) {
        return;
    }
    while(!g_theApp->IsQuitting()) {
        ::Sleep(0);
        RunMessagePump();
        g_theApp->RunFrame();
    }
}

void RunMessagePump() {
    MSG msg{};
    for(;;) {
        const BOOL hasMsg = ::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
        if(!hasMsg) {
            break;
        }
        auto window = g_theRenderer->GetOutput()->GetWindow();
        auto hWnd = window->GetWindowHandle();
        HACCEL tbl = reinterpret_cast<HACCEL>(g_theConsole->GetAcceleratorTable());
        if(!::TranslateAcceleratorA(hWnd, tbl, &msg)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }
}

void Shutdown() {
    //Required due to WinProc needing to destroy window
    g_theSubsystemHead = g_theApp.get();
}