#include "Engine/Renderer/Window.hpp"

#include "Engine/Core/EngineBase.hpp"

#include "Engine/Math/IntVector2.hpp"

#include "Engine/RHI/RHITypes.hpp"

std::size_t Window::_refCount = 0;

Window::Window() {
    if(_refCount == 0) {
        if(Register()) {
            ++_refCount;
        }
    }

    RECT desktopRect;
    HWND desktopHwnd = ::GetDesktopWindow();
    GetClientRect(desktopHwnd, &desktopRect);

    ::AdjustWindowRectEx(&desktopRect, _styleFlags, _hasMenu, _styleFlagsEx);
    ::GetClipCursor(&_initialClippingArea);

}

Window::~Window() {
    Close();
    if(_refCount != 0) {
        --_refCount;
        if(_refCount == 0) {
            Unregister();
        }
    }
}

void Window::Open() {
    if(Create()) {
        Show();
        SetForegroundWindow();
        SetFocus();

        HCURSOR cursor = ::LoadCursor(nullptr, IDC_ARROW);
        ::SetCursor(cursor);
    }
}

void Window::Close() {
    ::DestroyWindow(_hWnd);
}

void Window::Show() {
    ::ShowWindow(_hWnd, SW_SHOW);
}

void Window::Hide() {
    ::ShowWindow(_hWnd, SW_HIDE);
}

void Window::UnHide() {
    Show();
}

bool Window::IsOpen() {
    return 0 != ::IsWindow(_hWnd);
}

bool Window::IsClosed() {
    return !IsOpen();
}

bool Window::IsWindowed() const {
    return _currentDisplayMode != RHIOutputMode::FULLSCREEN_DEDICATED;
}

IntVector2 Window::GetClientSize() const {
    return IntVector2(_width, _height);
}

void Window::SetClientSize(const IntVector2& client_position, const IntVector2& client_size) {
    RECT r;
    r.top = client_position.y;
    r.left = client_position.x;
    r.bottom = r.top + client_size.y;
    r.right = r.left + client_size.x;
    ::AdjustWindowRectEx(&r, _styleFlags, _hasMenu, _styleFlagsEx);
    _width = r.right - r.left;
    _height = r.bottom - r.top;
    _positionX = r.left;
    _positionY = r.top;
}

void Window::SetForegroundWindow() {
    ::SetForegroundWindow(_hWnd);
}

void Window::SetFocus() {
    ::SetFocus(_hWnd);
}

HWND Window::GetWindowHandle() {
    return _hWnd;
}

void Window::SetWindowHandle(HWND hWnd) {
    _hWnd = hWnd;
}

const RHIOutputMode& Window::GetDisplayMode() const {
    return _currentDisplayMode;
}

void Window::SetDisplayMode(const RHIOutputMode& display_mode) {
    _currentDisplayMode = display_mode;
    RECT r;
    r.top = _positionY;
    r.left = _positionX;
    r.bottom = _height;
    r.right = _width;
    switch(_currentDisplayMode) {
        case RHIOutputMode::BORDERLESS:
        {
            _styleFlags = WS_POPUP;
            _hasMenu = false;

            ::GetClientRect(_hWnd, &r);

            long width = r.right - r.left;
            long height = r.bottom - r.top;
            ::SetWindowLongPtr(_hWnd, GWL_STYLE, _styleFlags);
            ::SetWindowPos(_hWnd, nullptr, 0, 0, width, height, SWP_SHOWWINDOW);
            SetClientSize(IntVector2::ZERO, IntVector2(width, height));
            ::AdjustWindowRectEx(&r, _styleFlags, _hasMenu, _styleFlagsEx);
            return;

            break;
        } case RHIOutputMode::WINDOWED:
        {
            _styleFlags = WS_CAPTION | WS_BORDER | WS_SYSMENU | WS_OVERLAPPED;
            break;
        } case RHIOutputMode::FULLSCREEN_WINDOW:
        {

            _styleFlags = WS_POPUP;

            RECT desktopRect;
            HWND desktopWindowHandle = GetDesktopWindow();
            ::GetClientRect(desktopWindowHandle, &desktopRect);

            long width = desktopRect.right - desktopRect.left;
            long height = desktopRect.bottom - desktopRect.top;
            ::SetWindowLongPtr(_hWnd, GWL_STYLE, _styleFlags);
            ::SetWindowPos(_hWnd, nullptr, 0, 0, width, height, SWP_SHOWWINDOW);
            SetClientSize(IntVector2::ZERO, IntVector2(width, height));
            ::AdjustWindowRectEx(&r, _styleFlags, _hasMenu, _styleFlagsEx);
            return;
        } case RHIOutputMode::FULLSCREEN_DEDICATED:
            break;
        default:
            /* DO NOTHING */;
    }
    ::SetWindowLongPtr(_hWnd, GWL_STYLE, _styleFlags);
    ::AdjustWindowRectEx(&r, _styleFlags, _hasMenu, _styleFlagsEx);

}

bool Window::Register() {
    _hInstance = GetModuleHandle(nullptr);
    memset(&_wc, 0, sizeof(_wc));
    auto window_class_name = L"Simple Window Class";
    _wc.cbSize = sizeof(_wc);
    _wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    _wc.lpfnWndProc = EngineMessageHandlingProcedure;
    _wc.cbClsExtra = 0;
    _wc.cbWndExtra = 0;
    _wc.hInstance = _hInstance;
    _wc.hIcon = nullptr;
    _wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    _wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    _wc.lpszMenuName = NULL;
    _wc.lpszClassName = window_class_name;
    _wc.hIconSm = nullptr;
    return 0 != RegisterClassEx(&_wc);
}

bool Window::Unregister() {
    return 0 != ::UnregisterClass(_wc.lpszClassName, nullptr);
}

bool Window::Create() {
    _styleFlags = WS_CAPTION | WS_BORDER | WS_SYSMENU | WS_OVERLAPPED;
    _styleFlagsEx = WS_EX_APPWINDOW;
    _hWnd = ::CreateWindowEx(
        _styleFlagsEx,                              // Optional window styles.
        _wc.lpszClassName,              // Window class
        L"Learn to Program Windows",     // Window text
        _styleFlags,            // Window style
        _positionX, _positionY,                           //Position XY
        _width, _height,    //Size WH
        nullptr,       // Parent window    
        nullptr,       // Menu
        _hInstance,     // Instance handle
        this        // Additional application data
    );
    return _hWnd != nullptr;
}

