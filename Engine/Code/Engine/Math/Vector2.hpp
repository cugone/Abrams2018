#pragma once

class Vector2 {
public:

    static Vector2 ONE;
    static Vector2 ZERO;
    static Vector2 X_AXIS;
    static Vector2 Y_AXIS;

    Vector2();
    Vector2(const Vector2& rhs) = default;
    Vector2(Vector2&& rhs) = default;
    Vector2& operator=(const Vector2& rhs) = default;
    Vector2& operator=(Vector2&& rhs) = default;
    ~Vector2() = default;

    explicit Vector2(float initialX, float initialY);

    Vector2 operator+(const Vector2& rhs);
    Vector2& operator+=(const Vector2& rhs);

    Vector2 operator-(const Vector2& rhs);
    Vector2& operator-=(const Vector2& rhs);

    Vector2 operator*(float scalar);
    Vector2 operator*=(float scalar);
    Vector2 operator*(const Vector2& rhs);
    Vector2 operator*=(const Vector2& rhs);

    Vector2 operator/(const Vector2& rhs);
    Vector2 operator/=(const Vector2& rhs);

    float x;
    float y;

protected:
private:
};