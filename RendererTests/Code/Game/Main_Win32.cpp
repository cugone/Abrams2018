#include <memory>
#include <sstream>

#include "Engine/Core/Win.hpp"

#include "Engine/Renderer/Window.hpp"

#include "Game/GameCommon.hpp"

HWND _hwnd = nullptr;
WNDCLASSEX _wc{};
bool _isQuitting = false;
int window_width = 800;
int window_height = 600;

bool CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void Initialize(HINSTANCE hInstance, LPWSTR lpCmdLine, int nShowCmd);
void Shutdown();
void RunMessagePump();

void RunMessagePump() {
    MSG msg{};
    for(;;) {
        const BOOL hasMsg = ::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
        if(!hasMsg) {
            break;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

int WINAPI wWinMain([[maybe_unused]] HINSTANCE hInstance, [[maybe_unused]] HINSTANCE hPrevInstance, [[maybe_unused]] LPWSTR lpCmdLine, [[maybe_unused]] int nShowCmd) {

    Initialize(hInstance, lpCmdLine, nShowCmd);

    while(!g_theApp->IsQuitting()) {
        ::Sleep(0);
        RunMessagePump();
        g_theApp->RunFrame();
    }
    Shutdown();
    return 0;
}


void Initialize([[maybe_unused]] HINSTANCE hInstance, [[maybe_unused]] LPWSTR lpCmdLine, [[maybe_unused]] int nShowCmd) {
    g_theApp = new App();
}

void Shutdown() {
    delete g_theApp;
    g_theApp = nullptr;
}