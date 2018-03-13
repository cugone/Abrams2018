#include "Engine/Math/Vector3.hpp"

#include <cmath>

#include "Engine/Math/MathUtils.hpp"

#include "Engine/Math/Vector2.hpp"

const Vector3 Vector3::ZERO(0.0f, 0.0f, 0.0f);
const Vector3 Vector3::X_AXIS(1.0f, 0.0f, 0.0f);
const Vector3 Vector3::Y_AXIS(0.0f, 1.0f, 0.0f);
const Vector3 Vector3::Z_AXIS(0.0f, 0.0f, 1.0f);
const Vector3 Vector3::XY_AXIS(1.0f, 1.0f, 0.0f);
const Vector3 Vector3::XZ_AXIS(1.0f, 0.0f, 1.0f);
const Vector3 Vector3::YZ_AXIS(0.0f, 1.0f, 1.0f);
const Vector3 Vector3::ONE(1.0f, 1.0f, 1.0f);

Vector3::Vector3() {
    /* DO NOTHING */
}

Vector3::Vector3(float initialX, float initialY, float initialZ)
: x(initialX)
, y(initialY)
, z(initialZ)
{
    /* DO NOTHING */
}

Vector3::Vector3(const Vector2& xy, float initialZ)
    : x(xy.x)
    , y(xy.y)
    , z(initialZ)
{
    /* DO NOTHING */
}

Vector3::Vector3(const Vector2& vec2)
    : x(vec2.x)
    , y(vec2.y)
{
    /* DO NOTHING */
}

Vector3 Vector3::operator+(const Vector3& rhs) const {
    return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
}

Vector3& Vector3::operator+=(const Vector3& rhs) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
}

Vector3 Vector3::operator-(const Vector3& rhs) const {
    return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
}

Vector3& Vector3::operator-=(const Vector3& rhs) {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
}

Vector3 Vector3::operator-() const {
    return Vector3(-x, -y, -z);
}

Vector3 Vector3::operator*(const Vector3& rhs) const {
    return Vector3(x * rhs.x, y * rhs.y, z * rhs.z);
}

Vector3 operator*(float lhs, const Vector3& rhs) {
    return Vector3(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
}

Vector3 Vector3::operator*(float scalar) const {
    return Vector3(x * scalar, y * scalar, z * scalar);
}

Vector3& Vector3::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

Vector3& Vector3::operator*=(const Vector3& rhs) {
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
    return *this;
}

Vector3 Vector3::operator/(float scalar) const {
    return Vector3(x / scalar, y / scalar, z / scalar);
}

Vector3 Vector3::operator/=(float scalar) {
    x /= scalar;
    y /= scalar;
    z /= scalar;
    return *this;
}

Vector3 Vector3::operator/(const Vector3& rhs) const {
    return Vector3(x / rhs.x, y / rhs.y, z / rhs.z);
}

Vector3 Vector3::operator/=(const Vector3& rhs) {
    x /= rhs.x;
    y /= rhs.y;
    return *this;
}

bool Vector3::operator==(const Vector3& rhs) const {
    return x == rhs.x && y == rhs.y;
}

bool Vector3::operator!=(const Vector3& rhs) const {
    return !(*this == rhs);
}

void Vector3::GetXYZ(float& outX, float& outY, float& outZ) const {
    outX = x;
    outY = y;
    outZ = z;
}

const float* Vector3::GetAsFloatArray() const {
    return &x;
}

float Vector3::CalcLength() const {
    return std::sqrt(CalcLengthSquared());
}

float Vector3::CalcLengthSquared() const {
    return x * x + y * y + z * z;
}

float Vector3::Normalize() {
    float length = CalcLength();
    if(length > 0.0f) {
        float inv_length = 1.0f / length;
        x *= inv_length;
        y *= inv_length;
        z *= inv_length;
        return length;
    }
    return 0.0f;
}

Vector3 Vector3::GetNormalize() const {
    float length = CalcLength();
    if(length > 0.0f) {
        float inv_length = 1.0f / length;
        return Vector3(x * inv_length, y * inv_length, z * inv_length);
    }
    return Vector3::ZERO;
}

void Vector3::SetXYZ(float newX, float newY, float newZ) {
    x = newX;
    y = newY;
    z = newZ;
}
