#include <memory>
#include <sstream>

#include "Engine/Core/Win.hpp"

#include "Engine/Renderer/Window.hpp"

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
    
    g_theFileLogger = new FileLogger();
    g_theApp = new App();

    g_theInput->SetNextHandler(g_theApp);
    g_theApp->SetNextHandler(nullptr);

    g_theSubsystemHead = g_theInput;

    g_theFileLogger->Initialize("game");
    g_theApp->Initialize();
}

void RunMessagePump() {
    MSG msg{};
    for(;;) {
        const BOOL hasMsg = ::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
        if(!hasMsg) {
            break;
        }
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
}

void Shutdown() {
    g_theSubsystemHead = g_theApp;
    delete g_theApp;
    g_theApp = nullptr;
    g_theSubsystemHead = nullptr;
    delete g_theFileLogger;
    g_theFileLogger = nullptr;
}