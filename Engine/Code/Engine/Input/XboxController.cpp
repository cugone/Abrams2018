#include "Engine/Input/XboxController.hpp"

#include "Engine/Core/Win.hpp"

#include "Engine/Math/MathUtils.hpp"

bool XboxController::WasAnyButtonJustPressed() const {
    return (_previousButtonState.to_ulong() < _currentButtonState.to_ulong());
}

bool XboxController::WasAnyButtonJustReleased() const {
    return (_currentButtonState.to_ulong() < _previousButtonState.to_ulong());
}

bool XboxController::IsAnyButtonDown() const {
    return _currentButtonState.any();
}

bool XboxController::IsButtonUp(const Button& button) const {
    return !_currentButtonState[(std::size_t)button];
}

bool XboxController::WasButtonJustPressed(const Button& button) const {
    return !_previousButtonState[(std::size_t)button] && _currentButtonState[(std::size_t)button];
}

bool XboxController::IsButtonDown(const Button& button) const {
    return _currentButtonState[(std::size_t)button];
}

bool XboxController::WasButtonJustReleased(const Button& button) const {
    return _previousButtonState[(std::size_t)button] && !_currentButtonState[(std::size_t)button];
}

bool XboxController::WasJustConnected() const {
    return !_previousActiveState[(std::size_t)ActiveState::CONNECTED] && _currentActiveState[(std::size_t)ActiveState::CONNECTED];
}

bool XboxController::IsConnected() const {
    return _previousActiveState[(std::size_t)ActiveState::CONNECTED] && _currentActiveState[(std::size_t)ActiveState::CONNECTED];
}

bool XboxController::WasJustDisconnected() const {
    return _previousActiveState[(std::size_t)ActiveState::CONNECTED] && !_currentActiveState[(std::size_t)ActiveState::CONNECTED];
}

bool XboxController::IsDisconnected() const {
    return !_previousActiveState[(std::size_t)ActiveState::CONNECTED] && !_currentActiveState[(std::size_t)ActiveState::CONNECTED];
}

void XboxController::Update(int controller_number) {
    XINPUT_STATE state;
    std::memset(&state, 0, sizeof(state));

    auto error_status = ::XInputGetState(controller_number, &state);
    _previousPacketNumber = _currentPacketNumber;
    _currentPacketNumber = state.dwPacketNumber;
    if(_previousPacketNumber == _currentPacketNumber) {
        return;
    }

    if(error_status == ERROR_DEVICE_NOT_CONNECTED) {
        _previousActiveState = _currentActiveState;
        _currentActiveState[(std::size_t)ActiveState::CONNECTED] = false;
        return;
    }

    if(error_status == ERROR_SUCCESS) {
        _previousActiveState = _currentActiveState;
        if(!_currentActiveState[(std::size_t)ActiveState::CONNECTED]) {
            _currentActiveState[(std::size_t)ActiveState::CONNECTED] = true;
        }

        _previousRawInput = _currentRawInput;
        _currentRawInput = state.Gamepad.wButtons;

        UpdateState();

        _leftThumbDistance = Vector2(state.Gamepad.sThumbLX, state.Gamepad.sThumbLY);
        _rightThumbDistance = Vector2(state.Gamepad.sThumbRX, state.Gamepad.sThumbRY);
        _triggerDistances = Vector2(state.Gamepad.bLeftTrigger, state.Gamepad.bRightTrigger);

        float leftRadius = _leftThumbDistance.CalcLength();

        leftRadius = MathUtils::RangeMap<float>(leftRadius, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, 32000, 0.0f, 1.0f);
        leftRadius = MathUtils::Clamp<float>(leftRadius, 0.0f, 1.0f);

        _leftThumbDistance.SetLength(leftRadius);

        float rightRadius = _rightThumbDistance.CalcLength();

        rightRadius = MathUtils::RangeMap<float>(rightRadius, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE, 32000, 0.0f, 1.0f);
        rightRadius = MathUtils::Clamp<float>(rightRadius, 0.0f, 1.0f);

        _rightThumbDistance.SetLength(rightRadius);

        _triggerDistances.x = MathUtils::RangeMap<float>(_triggerDistances.x, static_cast<float>(XINPUT_GAMEPAD_TRIGGER_THRESHOLD), 255.0f, 0.0f, 1.0f);
        _triggerDistances.y = MathUtils::RangeMap<float>(_triggerDistances.y, static_cast<float>(XINPUT_GAMEPAD_TRIGGER_THRESHOLD), 255.0f, 0.0f, 1.0f);

        if(DidMotorStateChange()) {
            SetMotorSpeed(controller_number, Motor::LEFT, _leftMotorState);
            SetMotorSpeed(controller_number, Motor::RIGHT, _rightMotorState);
        }

    }
}

void XboxController::StopLeftMotor() {
    SetLeftMotorSpeed(0);
}

void XboxController::StopRightMotor() {
    SetRightMotorSpeed(0);
}

void XboxController::StopMotors() {
    StopLeftMotor();
    StopRightMotor();
}

void XboxController::SetLeftMotorSpeed(unsigned short speed) {
    if(speed == _leftMotorState) {
        return;
    }
    _leftMotorState = speed;
    _currentActiveState[(std::size_t)ActiveState::MOTOR] = true;
}

void XboxController::SetRightMotorSpeed(unsigned short speed) {
    if(speed == _rightMotorState) {
        return;
    }
    _rightMotorState = speed;
    _currentActiveState[(std::size_t)ActiveState::MOTOR] = true;
}

void XboxController::SetBothMotorSpeed(unsigned short speed) {
    SetLeftMotorSpeed(speed);
    SetRightMotorSpeed(speed);
}

void XboxController::SetLeftMotorSpeedToMax() {
    SetLeftMotorSpeedAsPercent(1.0f);
}

void XboxController::SetRightMotorSpeedToMax() {
    SetRightMotorSpeedAsPercent(1.0f);
}

void XboxController::SetBothMotorSpeedToMax() {
    SetLeftMotorSpeedToMax();
    SetRightMotorSpeedToMax();
}

void XboxController::SetLeftMotorSpeedAsPercent(float speed) {
    SetLeftMotorSpeed(static_cast<unsigned short>(static_cast<float>((std::numeric_limits<unsigned short>::max)()) * speed));
}

void XboxController::SetRightMotorSpeedAsPercent(float speed) {
    SetRightMotorSpeed(static_cast<unsigned short>(static_cast<float>((std::numeric_limits<unsigned short>::max)()) * speed));
}

void XboxController::SetBothMotorSpeedAsPercent(float speed) {
    SetLeftMotorSpeedAsPercent(speed);
    SetRightMotorSpeedAsPercent(speed);
}

void XboxController::UpdateState() {

    _previousButtonState = _currentButtonState;

    _currentButtonState[(std::size_t)Button::UP]    = (_currentRawInput & XINPUT_GAMEPAD_DPAD_UP) != 0;
    _currentButtonState[(std::size_t)Button::DOWN]  = (_currentRawInput & XINPUT_GAMEPAD_DPAD_DOWN) != 0;
    _currentButtonState[(std::size_t)Button::LEFT]  = (_currentRawInput & XINPUT_GAMEPAD_DPAD_LEFT) != 0;
    _currentButtonState[(std::size_t)Button::RIGHT] = (_currentRawInput & XINPUT_GAMEPAD_DPAD_RIGHT) != 0;

    _currentButtonState[(std::size_t)Button::START]      = (_currentRawInput & XINPUT_GAMEPAD_START) != 0;
    _currentButtonState[(std::size_t)Button::BACK]       = (_currentRawInput & XINPUT_GAMEPAD_BACK) != 0;
    _currentButtonState[(std::size_t)Button::LEFTTHUMB]  = (_currentRawInput & XINPUT_GAMEPAD_LEFT_THUMB) != 0;
    _currentButtonState[(std::size_t)Button::RIGHTTHUMB] = (_currentRawInput & XINPUT_GAMEPAD_RIGHT_THUMB) != 0;

    _currentButtonState[(std::size_t)Button::LEFTBUMPER]  = (_currentRawInput & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0;
    _currentButtonState[(std::size_t)Button::RIGHTBUMPER] = (_currentRawInput & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;

    _currentButtonState[(std::size_t)Button::A] = (_currentRawInput & XINPUT_GAMEPAD_A) != 0;
    _currentButtonState[(std::size_t)Button::B] = (_currentRawInput & XINPUT_GAMEPAD_B) != 0;
    _currentButtonState[(std::size_t)Button::X] = (_currentRawInput & XINPUT_GAMEPAD_X) != 0;
    _currentButtonState[(std::size_t)Button::Y] = (_currentRawInput & XINPUT_GAMEPAD_Y) != 0;

}

void XboxController::SetMotorSpeed(int controller_number, const Motor& motor, unsigned short value) {
    XINPUT_VIBRATION vibration;
    std::memset(&vibration, 0, sizeof(vibration));
    switch(motor) {
        case Motor::LEFT:
            vibration.wLeftMotorSpeed = value;
            break;
        case Motor::RIGHT:
            vibration.wRightMotorSpeed = value;
            break;
        case Motor::BOTH:
            vibration.wLeftMotorSpeed = value;
            vibration.wRightMotorSpeed = value;
        default:
            /* DO NOTHING */;
    }
    DWORD errorStatus = ::XInputSetState(controller_number, &vibration);
    if(errorStatus == ERROR_SUCCESS) {
        return;
    } else if(errorStatus == ERROR_DEVICE_NOT_CONNECTED) {
        _previousActiveState = _currentActiveState;
        _currentActiveState[(std::size_t)ActiveState::CONNECTED] = false;
        return;
    } else {
        return;
    }
}

bool XboxController::DidMotorStateChange() const {
    return _previousActiveState[(std::size_t)ActiveState::MOTOR] ^ _currentActiveState[(std::size_t)ActiveState::MOTOR];
}
