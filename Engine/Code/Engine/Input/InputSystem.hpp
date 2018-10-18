#pragma once
#include "Engine/Input/XboxController.hpp"

#include <array>
#include <bitset>

#include "Engine/Core/EngineSubsystem.hpp"
#include "Engine/Core/Stopwatch.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"

class Window;

enum class KeyCode : int {
    LButton
    ,RButton
    ,Cancel
    ,MButton
    ,XButton1
    ,XButton2
    ,Back  /* Also BackSpace */
    ,First_ = Back
    ,Backspace = Back
    ,Tab
    ,Clear
    ,Return  /* Also Enter */
    ,Enter = Return
    ,Shift
    ,Ctrl  /* Also Control */
    ,Menu  /* Also Alt */
    ,Alt = Menu /* Also Menu */
    ,Pause
    ,Capital  /* Also CapsLock */
    ,CAPSLOCK = Capital
    ,Kana
    ,Hangul  /* Also Hangeul */
    ,Hangeul = Hangul
    ,Junja
    ,Final
    ,Hanja
    ,Kanji
    ,Escape  /* Also Esc */
    ,Esc = Escape
    ,Convert
    ,NonConvert
    ,Accept
    ,ModeChange
    ,Space  /* Also Spacebar */
    ,Spacebar = Space
    ,Prior  /* Also PageUp */
    ,PageUp = Prior
    ,Next  /* Also PageDown  PageDn */
    ,PageDown = Next
    ,PageDn = Next
    ,End
    ,Home
    ,Left
    ,Up
    ,Right
    ,Down
    ,Select
    ,Print
    ,Execute
    ,Snapshot  /* Also PrintScreen */
    ,PrintScreen = Snapshot
    ,Insert
    ,Delete  /* Also Del */
    ,Del = Delete
    ,Help
    ,Numeric0
    ,Numeric1
    ,Numeric2
    ,Numeric3
    ,Numeric4
    ,Numeric5
    ,Numeric6
    ,Numeric7
    ,Numeric8
    ,Numeric9
    ,A
    ,B
    ,C
    ,D
    ,E
    ,F
    ,G
    ,H
    ,I
    ,J
    ,K
    ,L
    ,M
    ,N
    ,O
    ,P
    ,Q
    ,R
    ,S
    ,T
    ,U
    ,V
    ,W
    ,X
    ,Y
    ,Z
    ,LWin
    ,RWin
    ,Apps
    ,Sleep
    ,NumPad0
    ,NumPad1
    ,NumPad2
    ,NumPad3
    ,NumPad4
    ,NumPad5
    ,NumPad6
    ,NumPad7
    ,NumPad8
    ,NumPad9
    ,Multiply
    ,Add
    ,Separator  /* Also NumPadEnter */
    ,NumPadEnter = Separator
    ,Subtract
    ,Decimal
    ,Divide
    ,F1
    ,F2
    ,F3
    ,F4
    ,F5
    ,F6
    ,F7
    ,F8
    ,F9
    ,F10
    ,F11
    ,F12
    ,F13
    ,F14
    ,F15
    ,F16
    ,F17
    ,F18
    ,F19
    ,F20
    ,F21
    ,F22
    ,F23
    ,F24
    ,NumLock
    ,Scroll  /* Also ScrollLock */
    ,ScrollLock = Scroll
    ,Oem_Nec_Equal
    ,Oem_Fj_Jisho
    ,Oem_Fj_Masshou
    ,Oem_Fj_Touroku
    ,Oem_Fj_Loya
    ,Oem_Fj_Roya
    ,LShift
    ,RShift
    ,LControl  /* Also LCtrl */
    ,LCtrl = LControl
    ,RControl  /* Also RCtrl */
    ,RCtrl = RControl
    ,RMenu  /* Also RAlt */
    ,RAlt = RMenu
    ,LMenu  /* Also LAlt */
    ,LAlt = LMenu
    ,Browser_Back
    ,Browser_Forward
    ,Browser_Refresh
    ,Browser_Stop
    ,Browser_Search
    ,Browser_Favorites
    ,Browser_Home
    ,Volume_Mute
    ,Volume_Down
    ,Volume_Up
    ,Media_Next_Track
    ,Media_Prev_Track
    ,Media_Stop
    ,Media_Play_Pause
    ,Launch_Mail
    ,Launch_Media_Select
    ,Launch_App1
    ,LAUNCH_APP2
    ,Oem_1  /* ;: */
    ,SEMICOLON = Oem_1
    ,Oem_Plus  /* =+ */
    ,Equals = Oem_Plus
    ,Oem_Comma  /* ,< */
    ,Comma = Oem_Comma
    ,Oem_Minus  /* -_ */
    ,Minus = Oem_Minus
    ,Oem_Period /* .> */
    ,Period = Oem_Period
    ,Oem_2 /* /? */
    ,ForwardSlash = Oem_2
    ,FSlash = Oem_2
    ,Oem_3 /* `~ */
    ,Backquote = Oem_3
    ,Tilde = Oem_3
    ,Gamepad_A
    ,Gamepad_B
    ,Gamepad_X
    ,Gamepad_Y
    ,Gamepad_Right_Shoulder
    ,Gamepad_Left_Shoulder
    ,Gamepad_Left_Trigger
    ,Gamepad_Right_Trigger
    ,Gamepad_DPad_Up
    ,Gamepad_DPad_Down
    ,Gamepad_DPad_Left
    ,Gamepad_DPad_Right
    ,Gamepad_Menu
    ,Gamepad_View
    ,Gamepad_Left_Thumbstick_Button
    ,Gamepad_Right_Thumbstick_Button
    ,Gamepad_Left_Thumbstick_Up
    ,Gamepad_Left_Thumbstick_Down
    ,Gamepad_Left_Thumbstick_Right
    ,Gamepad_Left_Thumbstick_Left
    ,Gamepad_Right_Thumbstick_Up
    ,Gamepad_Right_Thumbstick_Down
    ,Gamepad_Right_Thumbstick_Right
    ,Gamepad_Right_Thumbstick_Left
    ,Oem_4  /* [{ */
    ,LeftBracket = Oem_4
    ,LBracket = Oem_4
    ,Oem_5  /* \| */
    ,Backslash = Oem_5
    ,Oem_6  /* ]} */
    ,RightBracket = Oem_6
    ,RBracket = Oem_6
    ,Oem_7  /* '" */
    ,Apostrophe = Oem_7
    ,SingleQuote = Oem_7
    ,Oem_8  /* misc. unknown */
    ,Oem_Ax
    ,Oem_102  /* RT 102's "<>" or "\|" */
    ,Ico_Help  /* Help key on ICO keyboard */
    ,Ico_00  /* 00 key on ICO keyboard */
    ,ProcessKey
    ,Ico_Clear  /* Clear key on ICO keyboard */
    ,Packet  /* Key is packet of data */
    ,Oem_Reset
    ,Oem_Jump
    ,Oem_Pa1
    ,Oem_Pa2
    ,Oem_Pa3
    ,Oem_WsCtrl
    ,Oem_CuSel
    ,Oem_Attn
    ,Oem_Finish
    ,Oem_Copy
    ,Oem_Auto
    ,Oem_EnlW
    ,Oem_BackTab
    ,Attn
    ,CrSel
    ,ExSel
    ,ErEof
    ,Play
    ,Zoom
    ,NoName
    ,Pa1
    ,Oem_Clear
    ,Last_
    ,Unknown = 0xFF
    ,Max
};

KeyCode& operator++(KeyCode& keycode);
KeyCode operator++(KeyCode& keycode, int);

class InputSystem : public EngineSubsystem {
public:
    InputSystem() = default;
    virtual ~InputSystem() = default;

    void RegisterKeyDown(unsigned char keyIndex);
    void RegisterKeyUp(unsigned char keyIndex);
    
    virtual bool ProcessSystemMessage(const EngineMessage& msg) override;
    virtual void Initialize() override;
    virtual void BeginFrame() override;
    virtual void Update(float) override;
    virtual void Render() const override;
    virtual void EndFrame() override;

    bool WasAnyKeyPressed() const;
    bool IsKeyUp(const KeyCode& key) const;
    bool WasKeyJustPressed(const KeyCode& key) const;
    bool IsKeyDown(const KeyCode& key) const;
    bool IsAnyKeyDown() const;
    bool WasKeyJustReleased(const KeyCode& key) const;
    bool WasMouseWheelJustScrolledUp() const;
    bool WasMouseWheelJustScrolledDown() const;

    std::size_t GetConnectedControllerCount() const;
    bool IsAnyControllerConnected() const;
    const XboxController& GetXboxController(const std::size_t& controllerIndex) const;
    XboxController& GetXboxController(const std::size_t& controllerIndex);

    static unsigned char ConvertKeyCodeToWinVK(const KeyCode& code);
    static KeyCode ConvertWinVKToKeyCode(unsigned char winVK);

    void SetCursorScreenPosition(const Vector2& screen_pos);
    Vector2 GetCursorScreenPosition() const;

    void SetCursorWindowPosition(const Window& window, const Vector2& window_pos);
    Vector2 GetCursorWindowPosition(const Window& window_ref) const;

    void SetCursorToScreenCenter();
    void SetCursorToWindowCenter(const Window& window_ref);
    const Vector2& GetMouseCoords() const;

    int GetMouseWheelPosition() const;
    int GetMouseWheelPositionNormalized() const;

protected:
private:

    void UpdateXboxConnectedState();

    Vector2 GetScreenCenter() const;
    Vector2 GetWindowCenter(const Window& window) const;

    std::array<XboxController, 4> _xboxControllers{};
    std::bitset<(std::size_t)KeyCode::Max> _previousKeys{};
    std::bitset<(std::size_t)KeyCode::Max> _currentKeys{};
    Vector2 _mouseCoords = Vector2::ZERO;
    Vector2 _mouseDelta = Vector2::ZERO;
    Stopwatch _connection_poll = Stopwatch(FPSeconds(1.0f));
    int _mouseWheelPosition = 0;
    int _connected_controller_count = 0;
};