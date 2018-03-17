#pragma once

#include <functional>
#include <string>

#include "Engine/Core/Win.hpp"

class Window {
public:
    Window();
    ~Window();

    void Open();
    void Close();

    void Show();
    //void Hide();
    //void UnHide();
    //bool IsOpen();

    void SetForegroundWindow();
    void SetFocus();
    
    HWND GetWindowHandle();
    void SetWindowHandle(HWND hWnd);

    std::function<bool(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)> custom_message_handler;

protected:
    bool Register();
    bool Unregister();
    bool Create();
private:
    HWND _hWnd{};
    HINSTANCE _hInstance{};
    std::wstring _title = L"DEFAULT WINDOW";
    INT _cmdShow{};
    WNDCLASSEX _wc{};
    RECT _initialClippingArea;
    int _positionX = 0;
    int _positionY = 0;
    unsigned int _width = 800;
    unsigned int _height = 600;
    unsigned long _styleFlags = 0;
    unsigned long _styleFlagsEx = 0;
    bool _hasMenu = false;
    static std::size_t _refCount;
};