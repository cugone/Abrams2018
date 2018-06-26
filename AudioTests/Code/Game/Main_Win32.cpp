#include <memory>
#include <sstream>

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Win.hpp"

#include "Engine/Renderer/Window.hpp"

#include "Engine/RHI/RHIOutput.hpp"

#include "Game/GameCommon.hpp"


void Initialize(HINSTANCE hInstance, LPWSTR lpCmdLine, int nShowCmd);
void Shutdown();
void RunMessagePump();

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPWSTR lpCmdLine, int nShowCmd) {

    Initialize(hInstance, lpCmdLine, nShowCmd);

    while(!g_theApp->IsQuitting()) {
        ::Sleep(0);
        RunMessagePump();
        g_theApp->RunFrame();
    }
    Shutdown();
    return 0;
}


void Initialize(HINSTANCE /*hInstance*/, LPWSTR /*lpCmdLine*/, int /*nShowCmd*/) {
    
    g_theJobSystem = new JobSystem();
    g_theFileLogger = new FileLogger();
    std::condition_variable* cv = new std::condition_variable;
    g_theApp = new App(*g_theJobSystem, cv);

    g_theConsole->SetNextHandler(g_theInput);
    g_theInput->SetNextHandler(g_theApp);
    g_theApp->SetNextHandler(nullptr);

    g_theSubsystemHead = g_theConsole;

    g_theJobSystem->Initialize(-1, static_cast<std::size_t>(JobType::Max), cv);
    g_theFileLogger->Initialize(*g_theJobSystem, "game");
    g_theApp->Initialize();
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
    g_theSubsystemHead = g_theApp;
    delete g_theApp;
    g_theApp = nullptr;
    g_theSubsystemHead = nullptr;

    delete g_theFileLogger;
    g_theFileLogger = nullptr;

    delete g_theJobSystem;
    g_theJobSystem = nullptr;
}