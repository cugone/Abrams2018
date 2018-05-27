#include "Engine/Renderer/Camera3D.hpp"

void Camera3D::SetupView(float fovVerticalDegrees, float aspectRatio /*= MathUtils::M_16_BY_9_RATIO*/, float nearDistance /*= 0.01f*/, float farDistance /*= 1.0f*/, const Vector3& worldUp /*= Vector3::Y_AXIS*/) {
    fov_vertical_degrees = fovVerticalDegrees;
    aspect_ratio = aspectRatio;
    near_distance = nearDistance;
    far_distance = farDistance;
    world_up = worldUp.GetNormalize();
    near_view_height = 2.0f * near_distance * std::tan(0.5f * fov_vertical_degrees);
    far_view_height = 2.0f * far_distance * std::tan(0.5f * fov_vertical_degrees);
    CalcViewMatrix();
    CalcProjectionMatrix();
    CalcViewProjectionMatrix();
}

void Camera3D::CalcViewProjectionMatrix() {
    view_projection_matrix = view_matrix * projection_matrix;
}

void Camera3D::CalcProjectionMatrix() {
    projection_matrix = Matrix4::CreateDXPerspectiveProjection(fov_vertical_degrees, aspect_ratio, near_distance, far_distance);
}

void Camera3D::CalcViewMatrix() {

    look.Normalize();
    up = MathUtils::CrossProduct(look, right).GetNormalize();
    right = MathUtils::CrossProduct(up, look);

    Matrix4 vT = Matrix4::CreateTranslationMatrix(-position);
    Matrix4 vQ;
    vQ.SetIBasis(Vector4{ right, 0.0f });
    vQ.SetJBasis(Vector4{ up, 0.0f });
    vQ.SetKBasis(Vector4{ look, 0.0f });
    view_matrix = vT * vQ;
}

void Camera3D::Update(float deltaSeconds) {
    trauma -= trauma_recovery_rate * deltaSeconds;
    CalcViewMatrix();
}

const Vector3& Camera3D::GetPosition() const {
    return position;
}

void Camera3D::Translate(const Vector3& displacement) {
    position += displacement;
}

void Camera3D::Translate(float x, float y, float z) {
    Translate(Vector3{ x, y, z });
}

void Camera3D::Translate2D(const Vector2& displacement) {
    Translate(displacement.x, displacement.y, 0.0f);
}

void Camera3D::Translate2D(float x, float y) {
    Translate2D(Vector2{ x, y });
}

float Camera3D::GetFovYDegrees() const {
    return fov_vertical_degrees;
}

float Camera3D::CalcFovXDegrees() const {
    float half_width = 0.5f * CalcNearViewWidth();
    return 2.0f * std::atan(half_width / near_distance);
}

float Camera3D::CalcNearViewWidth() const {
    return aspect_ratio * near_view_height;
}

float Camera3D::CalcNearViewHeight() const {
    return near_view_height;
}

float Camera3D::CalcFarViewWidth() const {
    return aspect_ratio * far_view_height;
}

float Camera3D::CalcFarViewHeight() const {
    return far_view_height;
}

float Camera3D::GetAspectRatio() const {
    return aspect_ratio;
}

float Camera3D::GetInverseAspectRatio() const {
    return 1.0f / aspect_ratio;
}

float Camera3D::GetNearDistance() const {
    return near_distance;
}

float Camera3D::GetFarDistance() const {
    return far_distance;
}

const Matrix4& Camera3D::GetViewMatrix() const {
    return view_matrix;
}

const Matrix4& Camera3D::GetProjectionMatrix() const {
    return projection_matrix;
}

const Matrix4& Camera3D::GetViewProjectionMatrix() const {
    return view_projection_matrix;
}
