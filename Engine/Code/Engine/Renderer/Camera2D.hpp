#pragma once

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Matrix4.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"

class Camera2D {
public:

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

    Matrix4 view_matrix = Matrix4::GetIdentity();
    Matrix4 projection_matrix = Matrix4::GetIdentity();
    Matrix4 view_projection_matrix = Matrix4::GetIdentity();
    Vector2 leftBottom_view = Vector2{ -1.0f, 1.0f };
    Vector2 rightTop_view = Vector2{ 1.0f, -1.0f };
    Vector2 nearFar_distance = Vector2{ 0.0f, 1.0f };
    Vector2 position = Vector2::ZERO;
    float aspect_ratio = MathUtils::M_16_BY_9_RATIO;
    float orientation_degrees = 0.0f;

};