#include "Engine/Renderer/Camera.hpp"

void Camera::SetupView(float fovVerticalDegrees, float aspectRatio /*= MathUtils::M_16_BY_9_RATIO*/, float nearDistance /*= 0.01f*/, float farDistance /*= 1000.0f*/, const Vector3& worldUp /*= Vector3::Y_AXIS*/) {
    _fovVerticalDegrees = fovVerticalDegrees;
    _aspectRatio = aspectRatio;
    _nearDistance = nearDistance;
    _farDistance = farDistance;
    _worldUp = worldUp.GetNormalize();
}

float Camera::GetAspectRatio() const {
    return _aspectRatio;
}
