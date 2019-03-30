
#include "Engine/Core/ArgumentParser.hpp"
#include "Engine/Core/Config.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/KeyValueParser.hpp"
#include "Engine/Core/Win.hpp"

#include "Engine/Renderer/Window.hpp"

#include "Engine/RHI/RHIOutput.hpp"

#include "Game/GameCommon.hpp"
#include "Game/GameConfig.hpp"

#include <sstream>

void Initialize(HINSTANCE hInstance, LPSTR lpCmdLine, int nShowCmd);
App* CreateApp();
void MainLoop();
void RunMessagePump();
void Shutdown();

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR lpCmdLine, int nShowCmd) {
    Initialize(hInstance, lpCmdLine, nShowCmd);
    MainLoop();
    Shutdown();
    return 0;
}

void Initialize(HINSTANCE /*hInstance*/, LPSTR /*lpCmdLine*/, int /*nShowCmd*/) {
    g_theApp = CreateApp();
    g_theApp->Initialize();
}

App* CreateApp() {
    std::unique_ptr<JobSystem> jobSystem = std::make_unique<JobSystem>(-1, static_cast<std::size_t>(JobType::Max), new std::condition_variable);
    std::unique_ptr<FileLogger> fileLogger = std::make_unique<FileLogger>(*jobSystem, "game");
    return new App(std::move(jobSystem), std::move(fileLogger));
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
    delete g_theApp;
    g_theApp = nullptr;
}