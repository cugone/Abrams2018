#pragma once

#include <functional>
#include <string>

#include "Engine/Core/Win.hpp"

#include "Engine/RHI/RHITypes.hpp"

class IntVector2;

class Window {
public:
    Window();
    explicit Window(const IntVector2& position, const IntVector2& dimensions);
    ~Window();

    void Open();
    void Close();

    void Show();
    void Hide();
    void UnHide();
    bool IsOpen();
    bool IsClosed();
    bool IsWindowed() const;
    bool IsFullscreen() const;

    IntVector2 GetDimensions() const;
    IntVector2 GetPosition() const;

    void SetDimensionsAndPosition(const IntVector2& new_position, const IntVector2& new_size);
    void SetPosition(const IntVector2& new_position);
    void SetDimensions(const IntVector2& new_dimensions);
    void SetForegroundWindow();
    void SetFocus();
    
    HWND GetWindowHandle();
    void SetWindowHandle(HWND hWnd);

    const RHIOutputMode& GetDisplayMode() const;
    void SetDisplayMode(const RHIOutputMode& display_mode);

    std::function<bool(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)> custom_message_handler;

    void SetTitle(const std::string& title);

protected:
    bool Register();
    bool Unregister();
    bool Create();
private:
    RHIOutputMode _currentDisplayMode = RHIOutputMode::Windowed;
    HWND _hWnd = {};
    HINSTANCE _hInstance = {};
    std::string _title = "DEFAULT WINDOW";
    INT _cmdShow = {};
    WNDCLASSEX _wc = {};
    RECT _initialClippingArea = {};
    int _positionX = 0;
    int _positionY = 0;
    unsigned int _width = 800;
    unsigned int _height = 600;
    unsigned long _styleFlags = 0;
    unsigned long _styleFlagsEx = 0;
    bool _hasMenu = false;
    static std::size_t _refCount;
};