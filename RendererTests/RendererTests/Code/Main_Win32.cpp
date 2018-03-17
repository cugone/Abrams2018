#include <memory>
#include <sstream>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>

HWND _hwnd = nullptr;
WNDCLASSEX _wc{};
bool _isQuitting = false;
int window_width = 800;
int window_height = 600;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

bool RegisterWindow(HINSTANCE hInstance);
bool Create(HINSTANCE hInstance);
void Show(HWND hwnd, INT nCmdShow);
void RunMessagePump();

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch(uMsg) {
        case WM_CLOSE:
        case WM_QUIT:
        case WM_DESTROY:
        {
            _isQuitting = true;
            break;
        } case WM_PAINT:
        {
            break;
        } case WM_KEYDOWN:
            unsigned int key = static_cast<unsigned int>(wParam);
            switch(key) {
                case VK_ESCAPE:
                    _isQuitting = true;
            }
            break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

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


void Show(HWND hwnd, INT nCmdShow) {
    ::ShowWindow(hwnd, nCmdShow);
}

bool Create(HINSTANCE hInstance) {
    _hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        _wc.lpszClassName,              // Window class
        L"Learn to Program Windows",     // Window text
        WS_OVERLAPPEDWINDOW,            // Window style
        0, 0,                           //Position XY
        window_width, window_height,    //Size WH
        nullptr,       // Parent window    
        nullptr,       // Menu
        hInstance,     // Instance handle
        nullptr        // Additional application data
    );
    return _hwnd != nullptr;
}

bool RegisterWindow(HINSTANCE hInstance) {
    auto window_class_name = L"Simple Window Class";
    _wc.cbSize = sizeof(WNDCLASSEX);
    _wc.style = CS_HREDRAW | CS_VREDRAW;
    _wc.lpfnWndProc = WindowProc;
    _wc.cbClsExtra = 0;
    _wc.cbWndExtra = 0;
    _wc.hInstance = hInstance;
    _wc.hIcon = nullptr;
    _wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    _wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    _wc.lpszMenuName = NULL;
    _wc.lpszClassName = window_class_name;
    _wc.hIconSm = nullptr;
    return RegisterClassEx(&_wc);
}

int WINAPI wWinMain(HINSTANCE hInstance, [[maybe_unused]] HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd) {

    if(!RegisterWindow(hInstance)) {
        std::wostringstream ss;
        ss << "Register Window failed with error code: " << GetLastError();
        ::MessageBox(nullptr, ss.str().c_str(), L"ERROR", MB_OK | MB_ICONERROR);
        return 0;
    }

    std::wstring cmdLine(lpCmdLine ? lpCmdLine : L"");

    try {
        auto width_str = cmdLine.substr(0, cmdLine.find_first_of(' '));
        auto height_str = cmdLine.substr(cmdLine.find_first_of(' '));
        window_width = std::stoi(width_str);
        window_height = std::stoi(height_str);
    } catch(...) {
        /* DO NOTHING */
    }

    if(!Create(hInstance)) {
        std::wostringstream ss;
        ss << "Create Window failed with error code: " << GetLastError();
        ::MessageBox(nullptr, ss.str().c_str(), L"ERROR", MB_OK | MB_ICONERROR);
        return 0;
    }
    Show(_hwnd, nShowCmd);
    while(!_isQuitting) {
        ::Sleep(0);
        RunMessagePump();
    }
    return 0;
}