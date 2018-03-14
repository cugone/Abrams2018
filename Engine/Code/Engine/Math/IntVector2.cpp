#include "Engine/Math/IntVector2.hpp"

#include <cmath>

#include "Engine/Math/Vector2.hpp"

const IntVector2 IntVector2::ZERO(0, 0);
const IntVector2 IntVector2::ONE(1, 1);
const IntVector2 IntVector2::X_AXIS(1, 0);
const IntVector2 IntVector2::Y_AXIS(0, 1);
const IntVector2 IntVector2::XY_AXIS(1, 1);
const IntVector2 IntVector2::YX_AXIS(1, 1);

IntVector2::IntVector2(int initialX, int initialY)
    : x(initialX)
    , y(initialY)
{
    /* DO NOTHING */
}

IntVector2::IntVector2(const Vector2& v2)
    : x(static_cast<int>(std::floor(v2.x)))
    , y(static_cast<int>(std::floor(v2.y)))
{
    /* DO NOTHING */
}

void IntVector2::SetXY(int newX, int newY) {
    x = newX;
    y = newY;
}

void IntVector2::GetXY(int& out_x, int& out_y) {
    out_x = x;
    out_y = y;
}

bool IntVector2::operator!=(const IntVector2& rhs) {
    return !(*this == rhs);
}

bool IntVector2::operator==(const IntVector2& rhs) {
    return x == rhs.x && y == rhs.y;
}
