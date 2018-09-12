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
    view_projection_matrix = projection_matrix * view_matrix;
    inv_view_projection_matrix = Matrix4::CalculateInverse(view_projection_matrix);
}

void Camera3D::CalcProjectionMatrix() {
    projection_matrix = Matrix4::CreateDXPerspectiveProjection(fov_vertical_degrees, aspect_ratio, near_distance, far_distance);
    inv_projection_matrix = Matrix4::CalculateInverse(projection_matrix);
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
    view_matrix = rotation_matrix * view_matrix;
    inv_view_matrix = Matrix4::CalculateInverse(view_matrix);
}

void Camera3D::CalcRotationMatrix() {
    float c_x_theta = MathUtils::CosDegrees(rotationPitch);
    float s_x_theta = MathUtils::SinDegrees(rotationPitch);
    Matrix4 Rx;
    Rx.SetIBasis(Vector4(1.0f, 0.0f, 0.0f, 0.0f));
    Rx.SetJBasis(Vector4(0.0f, c_x_theta, s_x_theta, 0.0f));
    Rx.SetKBasis(Vector4(0.0f, -s_x_theta, c_x_theta, 0.0f));

    float c_y_theta = MathUtils::CosDegrees(rotationYaw);
    float s_y_theta = MathUtils::SinDegrees(rotationYaw);
    Matrix4 Ry;
    Ry.SetIBasis(Vector4(c_y_theta, 0.0f, -s_y_theta, 0.0f));
    Ry.SetJBasis(Vector4(0.0f, 1.0f, 0.0f, 0.0f));
    Ry.SetKBasis(Vector4(s_y_theta, 0.0f, c_y_theta, 0.0f));

    float c_z_theta = MathUtils::CosDegrees(rotationRoll);
    float s_z_theta = MathUtils::SinDegrees(rotationRoll);
    Matrix4 Rz;
    Rz.SetIBasis(Vector4(c_z_theta, s_z_theta, 0.0f, 0.0f));
    Rz.SetJBasis(Vector4(-s_z_theta, c_z_theta, 0.0f, 0.0f));
    Rz.SetKBasis(Vector4(0.0f, 0.0f, 1.0f, 0.0f));

    rotation_matrix = Rz * Rx * Ry;
}

void Camera3D::Update(float deltaSeconds) {
    trauma -= trauma_recovery_rate * deltaSeconds;
    CalcRotationMatrix();
}

const Vector3& Camera3D::GetPosition() const {
    return position;
}

void Camera3D::SetPosition(const Vector3& newPosition) {
    position = newPosition;
}

void Camera3D::SetPosition(float x, float y, float z) {
    SetPosition(Vector3{ x, y, z });
}

void Camera3D::SetPosition(const Vector2& newPosition) {
    SetPosition(newPosition.x, newPosition.y, 0.0f);
}

void Camera3D::SetPosition(float x, float y) {
    SetPosition(Vector2{ x, y });
}

void Camera3D::Translate(const Vector3& displacement) {
    position += displacement;
}

void Camera3D::Translate(float x, float y, float z) {
    Translate(Vector3{ x, y, z });
}

void Camera3D::Translate(const Vector2& displacement) {
    Translate(displacement.x, displacement.y, 0.0f);
}

void Camera3D::Translate(float x, float y) {
    Translate(Vector2{ x, y });
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

const Matrix4& Camera3D::GetInverseViewMatrix() const {
    return inv_view_matrix;
}

const Matrix4& Camera3D::GetInverseProjectionMatrix() const {
    return inv_projection_matrix;
}

const Matrix4& Camera3D::GetInverseViewProjectionMatrix() const {
    return inv_view_projection_matrix;
}

void Camera3D::SetEulerAngles(const Vector3& eulerAnglesDegrees) {
    rotationPitch = eulerAnglesDegrees.x;
    rotationYaw = eulerAnglesDegrees.y;
    rotationRoll = eulerAnglesDegrees.z;
}

void Camera3D::SetForwardFromTarget(const Vector3& lookAtPosition) {
    look = (lookAtPosition - position).GetNormalize();
}

Vector3 Camera3D::GetRight() const {
    return Vector3(rotation_matrix.GetIBasis());
}

Vector3 Camera3D::GetUp() const {
    return Vector3(rotation_matrix.GetJBasis());
}

Vector3 Camera3D::GetForward() const {
    return Vector3(rotation_matrix.GetKBasis());
}
