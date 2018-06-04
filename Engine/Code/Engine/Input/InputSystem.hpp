#pragma once
#include "Engine/Input/XboxController.hpp"

#include <array>
#include <bitset>

#include "Engine/Core/EngineSubsystem.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"

enum class KeyCode : unsigned char {
    LBUTTON
    ,RBUTTON
    ,CANCEL
    ,MBUTTON
    ,BACK  /* Also BACKSPACE */
    ,FIRST_ = BACK
    ,BACKSPACE = BACK
    ,TAB
    ,CLEAR
    ,RETURN  /* Also ENTER */
    ,ENTER = RETURN
    ,SHIFT
    ,CTRL  /* Also CTRL */
    ,MENU  /* Also ALT */
    ,ALT = MENU /* Also LMENU */
    ,LMENU = MENU  /* Also LALT*/
    ,LALT = LMENU
    ,PAUSE
    ,CAPITAL  /* Also CAPSLOCK */
    ,CAPSLOCK = CAPITAL
    ,KANA
    ,HANGUL  /* Also HANGEUL */
    ,HANGEUL = HANGUL
    ,JUNJA
    ,FINAL
    ,HANJA
    ,KANJI
    ,ESCAPE  /* Also ESC */
    ,ESC = ESCAPE
    ,CONVERT
    ,NONCONVERT
    ,ACCEPT
    ,MODECHANGE
    ,SPACE  /* Also SPACEBAR */
    ,SPACEBAR = SPACE
    ,PRIOR  /* Also PAGEUP */
    ,PAGEUP = PRIOR
    ,NEXT  /* Also PAGEDOWN  PAGEDN */
    ,PAGEDOWN = NEXT
    ,PAGEDN = NEXT
    ,END
    ,HOME
    ,LEFT
    ,UP
    ,RIGHT
    ,DOWN
    ,SELECT
    ,PRINT
    ,EXECUTE
    ,SNAPSHOT  /* Also PRINTSCREEN */
    ,PRINTSCREEN = SNAPSHOT
    ,INSERT
    ,DELETE_KEYCODE  /* Also DEL */
    ,DEL = DELETE_KEYCODE
    ,HELP
    ,NUMERIC0
    ,NUMERIC1
    ,NUMERIC2
    ,NUMERIC3
    ,NUMERIC4
    ,NUMERIC5
    ,NUMERIC6
    ,NUMERIC7
    ,NUMERIC8
    ,NUMERIC9
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
    ,LWIN
    ,RWIN
    ,APPS
    ,SLEEP
    ,NUMPAD0
    ,NUMPAD1
    ,NUMPAD2
    ,NUMPAD3
    ,NUMPAD4
    ,NUMPAD5
    ,NUMPAD6
    ,NUMPAD7
    ,NUMPAD8
    ,NUMPAD9
    ,MULTIPLY
    ,ADD
    ,SEPARATOR  /* Also NUMPADENTER */
    ,NUMPADENTER = SEPARATOR
    ,SUBTRACT
    ,DECIMAL
    ,DIVIDE
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
    ,NUMLOCK
    ,SCROLL  /* Also SCROLLLOCK */
    ,SCROLLLOCK = SCROLL
    ,OEM_NEC_EQUAL
    ,OEM_FJ_JISHO
    ,OEM_FJ_MASSHOU
    ,OEM_FJ_TOUROKU
    ,OEM_FJ_LOYA
    ,OEM_FJ_ROYA
    ,LSHIFT
    ,RSHIFT
    ,LCONTROL  /* Also LCTRL */
    ,LCTRL = LCONTROL
    ,RCONTROL  /* Also RCTRL */
    ,RCTRL = RCONTROL
    ,RMENU  /* Also RALT */
    ,RALT = RMENU
    ,BROWSER_BACK
    ,BROWSER_FORWARD
    ,BROWSER_REFRESH
    ,BROWSER_STOP
    ,BROWSER_SEARCH
    ,BROWSER_FAVORITES
    ,BROWSER_HOME
    ,VOLUME_MUTE
    ,VOLUME_DOWN
    ,VOLUME_UP
    ,MEDIA_NEXT_TRACK
    ,MEDIA_PREV_TRACK
    ,MEDIA_STOP
    ,MEDIA_PLAY_PAUSE
    ,LAUNCH_MAIL
    ,LAUNCH_MEDIA_SELECT
    ,LAUNCH_APP1
    ,LAUNCH_APP2
    ,OEM_1  /* ;: */
    ,SEMICOLON = OEM_1
    ,OEM_PLUS  /* =+ */
    ,EQUALS = OEM_PLUS
    ,OEM_COMMA  /* ,< */
    ,COMMA = OEM_COMMA
    ,OEM_MINUS  /* -_ */
    ,MINUS = OEM_MINUS
    ,OEM_PERIOD /* .> */
    ,PERIOD = OEM_PERIOD
    ,OEM_2 /* /? */
    ,FORWARDSLASH = OEM_2
    ,FSLASH = OEM_2
    ,OEM_3 /* `~ */
    ,BACKQUOTE = OEM_3
    ,TILDE = OEM_3
    ,GAMEPAD_A
    ,GAMEPAD_B
    ,GAMEPAD_X
    ,GAMEPAD_Y
    ,GAMEPAD_RIGHT_SHOULDER
    ,GAMEPAD_LEFT_SHOULDER
    ,GAMEPAD_LEFT_TRIGGER
    ,GAMEPAD_RIGHT_TRIGGER
    ,GAMEPAD_DPAD_UP
    ,GAMEPAD_DPAD_DOWN
    ,GAMEPAD_DPAD_LEFT
    ,GAMEPAD_DPAD_RIGHT
    ,GAMEPAD_MENU
    ,GAMEPAD_VIEW
    ,GAMEPAD_LEFT_THUMBSTICK_BUTTON
    ,GAMEPAD_RIGHT_THUMBSTICK_BUTTON
    ,GAMEPAD_LEFT_THUMBSTICK_UP
    ,GAMEPAD_LEFT_THUMBSTICK_DOWN
    ,GAMEPAD_LEFT_THUMBSTICK_RIGHT
    ,GAMEPAD_LEFT_THUMBSTICK_LEFT
    ,GAMEPAD_RIGHT_THUMBSTICK_UP
    ,GAMEPAD_RIGHT_THUMBSTICK_DOWN
    ,GAMEPAD_RIGHT_THUMBSTICK_RIGHT
    ,GAMEPAD_RIGHT_THUMBSTICK_LEFT
    ,OEM_4  /* [{ */
    ,LEFTBRACKET = OEM_4
    ,LBRACKET = OEM_4
    ,OEM_5  /* \| */
    ,BACKSLASH = OEM_5
    ,OEM_6  /* ]} */
    ,RIGHTBRACKET = OEM_6
    ,RBRACKET = OEM_6
    ,OEM_7  /* '" */
    ,APOSTROPHE = OEM_7
    ,SINGLEQUOTE = OEM_7
    ,OEM_8  /* misc. unknown */
    ,OEM_AX
    ,OEM_102  /* RT 102's "<>" or "\|" */
    ,ICO_HELP  /* Help key on ICO keyboard */
    ,ICO_00  /* 00 key on ICO keyboard */
    ,PROCESSKEY
    ,ICO_CLEAR  /* Clear key on ICO keyboard */
    ,PACKET  /* Key is packet of data */
    ,OEM_RESET
    ,OEM_JUMP
    ,OEM_PA1
    ,OEM_PA2
    ,OEM_PA3
    ,OEM_WSCTRL
    ,OEM_CUSEL
    ,OEM_ATTN
    ,OEM_FINISH
    ,OEM_COPY
    ,OEM_AUTO
    ,OEM_ENLW
    ,OEM_BACKTAB
    ,ATTN
    ,CRSEL
    ,EXSEL
    ,EREOF
    ,PLAY
    ,ZOOM
    ,NONAME
    ,PA1
    ,OEM_CLEAR
    ,LAST_
    ,UNKNOWN = 0xFF
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
    virtual void Update(float deltaSeconds) override;
    virtual void Render() const override;
    virtual void EndFrame() override;

    bool WasAnyKeyPressed() const;
    bool IsKeyUp(const KeyCode& key) const;
    bool WasKeyJustPressed(const KeyCode& key) const;
    bool IsKeyDown(const KeyCode& key) const;
    bool WasKeyJustReleased(const KeyCode& key) const;

    std::size_t GetConnectedControllerCount() const;
    bool IsAnyControllerConnected() const;
    const XboxController& GetXboxController(const std::size_t& controllerIndex) const;
    XboxController& GetXboxController(const std::size_t& controllerIndex);

    static unsigned char ConvertKeyCodeToWinVK(const KeyCode& code);
    static KeyCode ConvertWinVKToKeyCode(unsigned char winVK);

protected:
private:

    std::array<XboxController, 4> _xboxControllers{};
    std::bitset<(std::size_t)KeyCode::UNKNOWN> _previousKeys{};
    std::bitset<(std::size_t)KeyCode::UNKNOWN> _currentKeys{};
};