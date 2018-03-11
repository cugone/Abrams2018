#include "Engine/Math/Vector2.hpp"

Vector2 Vector2::ZERO(0.0f, 0.0f);
Vector2 Vector2::X_AXIS(1.0f, 0.0f);
Vector2 Vector2::Y_AXIS(0.0f, 1.0f);
Vector2 Vector2::ONE(1.0f, 1.0);

Vector2::Vector2() {
    /* DO NOTHING */
}

Vector2::Vector2(float initialX, float initialY)
: x(initialX)
, y(initialY)
{
    /* DO NOTHING */
}

Vector2 Vector2::operator+(const Vector2& rhs) {
    return Vector2(x + rhs.x, y + rhs.y);
}

Vector2& Vector2::operator+=(const Vector2& rhs) {
    x += rhs.x;
    y += rhs.y;
    return *this;
}

Vector2 Vector2::operator-(const Vector2& rhs) {
    return Vector2(x - rhs.x, y - rhs.y);
}

Vector2& Vector2::operator-=(const Vector2& rhs) {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

Vector2 Vector2::operator*(const Vector2& rhs) {
    return Vector2(x * rhs.x, y * rhs.y);
}

Vector2 Vector2::operator*(float scalar) {
    return Vector2(x * scalar, y * scalar);
}

Vector2 Vector2::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    return *this;
}

Vector2 Vector2::operator*=(const Vector2& rhs) {
    x *= rhs.x;
    y *= rhs.y;
    return *this;
}

Vector2 Vector2::operator/(const Vector2& rhs) {
    return Vector2(x / rhs.x, y / rhs.y);
}

Vector2 Vector2::operator/=(const Vector2& rhs) {
    x /= rhs.x;
    y /= rhs.y;
    return *this;
}
