#include "Engine/Math/IntVector3.hpp"

#include <cmath>

#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"

const IntVector3 IntVector3::ZERO(0, 0, 0);
const IntVector3 IntVector3::ONE(1, 1, 1);
const IntVector3 IntVector3::X_AXIS(1, 0, 0);
const IntVector3 IntVector3::Y_AXIS(0, 1, 0);
const IntVector3 IntVector3::Z_AXIS(0, 0, 1);
const IntVector3 IntVector3::XY_AXIS(1, 1, 0);
const IntVector3 IntVector3::XZ_AXIS(1, 0, 1);
const IntVector3 IntVector3::YX_AXIS(1, 1, 0);
const IntVector3 IntVector3::YZ_AXIS(0, 1, 1);
const IntVector3 IntVector3::ZX_AXIS(1, 0, 1);
const IntVector3 IntVector3::ZY_AXIS(0, 1, 1);
const IntVector3 IntVector3::XYZ_AXIS(1, 1, 1);

IntVector3::IntVector3(int initialX, int initialY, int initialZ)
    : x(initialX)
    , y(initialY)
    , z(initialZ)
{
    /* DO NOTHING */
}

IntVector3::IntVector3(const IntVector2& iv2, int initialZ)
    : x(iv2.x)
    , y(iv2.y)
    , z(initialZ) {
    /* DO NOTHING */
}

IntVector3::IntVector3(const Vector2& v2, int initialZ)
    : x(static_cast<int>(std::floor(v2.x)))
    , y(static_cast<int>(std::floor(v2.y)))
    , z(initialZ) {
    /* DO NOTHING */
}

IntVector3::IntVector3(const Vector3& v3)
    : x(static_cast<int>(std::floor(v3.x)))
    , y(static_cast<int>(std::floor(v3.y)))
    , z(static_cast<int>(std::floor(v3.z)))
{
    /* DO NOTHING */
}

void IntVector3::SetXYZ(int newX, int newY, int newZ) {
    x = newX;
    y = newY;
    z = newZ;
}

void IntVector3::GetXYZ(int& out_x, int& out_y, int& out_z) {
    out_x = x;
    out_y = y;
    out_z = z;
}

bool IntVector3::operator!=(const IntVector3& rhs) {
    return !(*this == rhs);
}

bool IntVector3::operator==(const IntVector3& rhs) {
    return x == rhs.x && y == rhs.y && z == rhs.z;
}
