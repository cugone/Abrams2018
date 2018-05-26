#include "Engine/Renderer/Camera.hpp"

void Camera::SetupView(float fovVerticalDegrees, float aspectRatio /*= MathUtils::M_16_BY_9_RATIO*/, float nearDistance /*= 0.01f*/, float farDistance /*= 1000.0f*/, const Vector3& worldUp /*= Vector3::Y_AXIS*/) {
    this->fovVerticalDegrees = fovVerticalDegrees;
    this->aspectRatio = aspectRatio;
    this->nearDistance = nearDistance;
    this->farDistance = farDistance;
    this->worldUp = worldUp.GetNormalize();
}
