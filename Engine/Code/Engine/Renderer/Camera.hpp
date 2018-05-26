#pragma once

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vector3.hpp"

class Camera {
public:
    Camera() = default;
    ~Camera() = default;

    void SetupView(float fov_VerticalDegrees, float aspect_Ratio = MathUtils::M_16_BY_9_RATIO, float near_Distance = 0.01f, float far_Distance = 1000.0f, const Vector3& world_Up = Vector3::Y_AXIS);
    
    float trauma = 0.0f;
    float traumaRecoveryRate = 1.0f;
    float aspectRatio = MathUtils::M_16_BY_9_RATIO;
    float fovVerticalDegrees = 60.0f;
    float nearDistance = 0.01f;
    float farDistance = 1000.0f;
    Vector3 position = Vector3::ZERO;
    Vector3 worldUp = Vector3::Y_AXIS;
protected:
private:

};