#pragma once

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Matrix4.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"

class Camera2D {
public:
    Camera2D() = default;
    ~Camera2D() = default;

    void SetupView(float left, float right, float top, float bottom, float nearDistance = 0.0f, float farDistance = 1.0f, float aspectRatio = MathUtils::M_16_BY_9_RATIO);
    void SetupView(const Vector2& leftBottom, const Vector2& rightTop, const Vector2& nearFar = Vector2(0.0f, 1.0f), float aspectRatio = MathUtils::M_16_BY_9_RATIO);
    void Update(float deltaSeconds);

    const Vector2& GetPosition() const;
    void SetPosition(const Vector3& newPosition);
    void SetPosition(float x, float y, float z);
    void SetPosition(const Vector2& newPosition);
    void SetPosition(float x, float y);
    void Translate(const Vector3& displacement);
    void Translate(float x, float y, float z);
    void Translate(const Vector2& displacement);
    void Translate(float x, float y);
    void SetRotationDegrees(float newAngleDegrees);
    void ApplyRotationDegrees(float addAngleDegrees);
    void SetRotation(float newAngleRadians);
    void ApplyRotation(float addAngleRadians);
    float GetAspectRatio() const;
    float GetInverseAspectRatio() const;
    float GetNearDistance() const;
    float GetFarDistance() const;

    const Matrix4& GetViewMatrix() const;
    const Matrix4& GetProjectionMatrix() const;
    const Matrix4& GetViewProjectionMatrix() const;

    float trauma = 0.0f;
    float trauma_recovery_rate = 1.0f;
protected:
private:
    void CalcViewMatrix();
    void CalcViewProjectionMatrix();
    void CalcProjectionMatrix();

    float left_view = -1.0f;
    float right_view = 1.0f;
    float top_view = -1.0f;
    float bottom_view = 1.0f;
    float aspect_ratio = MathUtils::M_16_BY_9_RATIO;
    float near_distance = 0.01f;
    float far_distance = 1.0f;
    float orientation_degrees = 0.0f;
    Vector2 position = Vector2::ZERO;
    Matrix4 view_matrix = Matrix4::GetIdentity();
    Matrix4 projection_matrix = Matrix4::GetIdentity();
    Matrix4 view_projection_matrix = Matrix4::GetIdentity();

};