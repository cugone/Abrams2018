#include "Engine/Renderer/Camera2D.hpp"

void Camera2D::SetupView(float left, float right, float top, float bottom, float nearDistance /*= 0.0f*/, float farDistance /*= 1.0f*/, float aspectRatio /*= MathUtils::M_16_BY_9_RATIO*/) {
    SetupView(Vector2(left, bottom), Vector2(right, top), Vector2(nearDistance, farDistance), aspectRatio);
}

void Camera2D::SetupView(const Vector2& leftBottom, const Vector2& rightTop, const Vector2& nearFar /*= Vector2(0.0f, 1.0f)*/, float aspectRatio /*= MathUtils::M_16_BY_9_RATIO*/) {
    left_view = leftBottom.x;
    right_view = rightTop.x;
    bottom_view = leftBottom.y;
    top_view = rightTop.y;
    aspect_ratio = aspectRatio;
    near_distance = nearFar.x;
    far_distance = nearFar.y;
    CalcViewMatrix();
    CalcProjectionMatrix();
    CalcViewProjectionMatrix();
}

void Camera2D::CalcViewProjectionMatrix() {
    view_projection_matrix = projection_matrix * view_matrix;
}

void Camera2D::CalcProjectionMatrix() {
    projection_matrix = Matrix4::CreateDXOrthographicProjection(left_view, right_view, bottom_view, top_view, near_distance, far_distance);
}

void Camera2D::CalcViewMatrix() {
    Matrix4 vT = Matrix4::CreateTranslationMatrix(-position);
    Matrix4 vR = Matrix4::Create2DRotationDegreesMatrix(orientation_degrees);
    view_matrix = vT * vR;
}

void Camera2D::Update(float deltaSeconds) {
    trauma -= trauma_recovery_rate * deltaSeconds;
}

const Vector2& Camera2D::GetPosition() const {
    return position;
}

void Camera2D::SetPosition(const Vector3& newPosition) {
    SetPosition(Vector2(newPosition));
}

void Camera2D::SetPosition(float x, float y, float z) {
    SetPosition(Vector3{x, y, z});
}

void Camera2D::SetPosition(const Vector2& newPosition) {
    position = newPosition;
}

void Camera2D::SetPosition(float x, float y) {
    SetPosition(Vector2{x, y});
}

void Camera2D::Translate(const Vector3& displacement) {
    Translate(displacement.x, displacement.y);
}

void Camera2D::Translate(float x, float y, float z) {
    Translate(Vector3{ x, y, z });
}

void Camera2D::Translate(const Vector2& displacement) {
    position += displacement;
}

void Camera2D::Translate(float x, float y) {
    Translate(Vector2{ x, y });
}

void Camera2D::SetRotationDegrees(float newAngleDegrees) {
    orientation_degrees = newAngleDegrees;
}

void Camera2D::ApplyRotationDegrees(float addAngleDegrees) {
    orientation_degrees += addAngleDegrees;
}

void Camera2D::SetRotation(float newAngleRadians) {
    orientation_degrees = MathUtils::ConvertRadiansToDegrees(newAngleRadians);
}

void Camera2D::ApplyRotation(float addAngleRadians) {
    orientation_degrees += MathUtils::ConvertRadiansToDegrees(addAngleRadians);
}

float Camera2D::GetAspectRatio() const {
    return aspect_ratio;
}

float Camera2D::GetInverseAspectRatio() const {
    return 1.0f / aspect_ratio;
}

float Camera2D::GetNearDistance() const {
    return near_distance;
}

float Camera2D::GetFarDistance() const {
    return far_distance;
}

const Matrix4& Camera2D::GetViewMatrix() const {
    return view_matrix;
}

const Matrix4& Camera2D::GetProjectionMatrix() const {
    return projection_matrix;
}

const Matrix4& Camera2D::GetViewProjectionMatrix() const {
    return view_projection_matrix;
}
