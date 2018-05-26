#include "Engine/Renderer/Camera.hpp"

void Camera::SetupView(float fov_VerticalDegrees, float aspect_Ratio /*= MathUtils::M_16_BY_9_RATIO*/, float near_Distance /*= 0.01f*/, float far_Distance /*= 1000.0f*/, const Vector3& world_Up /*= Vector3::Y_AXIS*/) {
    this->fovVerticalDegrees = fov_VerticalDegrees;
    this->aspectRatio = aspect_Ratio;
    this->nearDistance = near_Distance;
    this->farDistance = far_Distance;
    this->worldUp = world_Up.GetNormalize();
}
