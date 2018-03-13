#pragma once

class Vector2;

class Vector3 {
public:

    static const Vector3 ONE;
    static const Vector3 ZERO;
    static const Vector3 X_AXIS;
    static const Vector3 Y_AXIS;
    static const Vector3 Z_AXIS;
    static const Vector3 XY_AXIS;
    static const Vector3 XZ_AXIS;
    static const Vector3 YZ_AXIS;


    Vector3() = default;
    Vector3(const Vector3& rhs) = default;
    Vector3(Vector3&& rhs) = default;
    Vector3& operator=(const Vector3& rhs) = default;
    Vector3& operator=(Vector3&& rhs) = default;
    ~Vector3() = default;

    explicit Vector3(float initialX, float initialY, float initialZ);
    explicit Vector3(const Vector2& vec2);
    explicit Vector3(const Vector2& xy, float initialZ);

    Vector3 operator+(const Vector3& rhs) const;
    Vector3& operator+=(const Vector3& rhs);

    Vector3 operator-() const;
    Vector3 operator-(const Vector3& rhs) const;
    Vector3& operator-=(const Vector3& rhs);

    friend Vector3 operator*(float lhs, const Vector3& rhs);
    Vector3 operator*(float scalar) const;
    Vector3& operator*=(float scalar);
    Vector3 operator*(const Vector3& rhs) const;
    Vector3& operator*=(const Vector3& rhs);

    Vector3 operator/(float scalar) const;
    Vector3 operator/=(float scalar);
    Vector3 operator/(const Vector3& rhs) const;
    Vector3 operator/=(const Vector3& rhs);

    bool operator==(const Vector3& rhs) const;
    bool operator!=(const Vector3& rhs) const;

    void GetXYZ(float& outX, float& outY, float& outZ) const;
    const float* GetAsFloatArray() const;

    float CalcLength() const;
    float CalcLengthSquared() const;
    
    float Normalize();
    Vector3 GetNormalize() const;

    void SetXYZ(float newX, float newY, float newZ);

    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

protected:
private:
};