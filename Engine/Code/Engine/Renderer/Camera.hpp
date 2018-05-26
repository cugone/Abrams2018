#pragma once

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vector3.hpp"

class Camera {
public:
    Camera() = default;
    ~Camera() = default;

    void SetupView(float fovVerticalDegrees, float aspectRatio = MathUtils::M_16_BY_9_RATIO, float nearDistance = 0.01f, float farDistance = 1000.0f, const Vector3& worldUp = Vector3::Y_AXIS);
    
    float trauma = 0.0f;
    float traumaRecoveryRate = 1.0f;
    float GetAspectRatio() const;
protected:
private:
    float _aspectRatio = MathUtils::M_16_BY_9_RATIO;
    Vector3 _position = Vector3::ZERO;
    float _fovVerticalDegrees = 60.0f;
    float _nearDistance = 0.01f;
    float _farDistance = 1000.0f;

    Vector3 _worldUp = Vector3::Y_AXIS;
};