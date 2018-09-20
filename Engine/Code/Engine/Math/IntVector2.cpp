#include "Engine/Math/IntVector2.hpp"

#include "Engine/Core/StringUtils.hpp"

#include "Engine/Math/Vector2.hpp"

#include <cmath>
#include <sstream>

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

IntVector2::IntVector2(const std::string& value)
    : x(0)
    , y(0)
{
    if(value[0] == '[') {
        if(value.back() == ']') {
            std::string contents_str = value.substr(1, value.size() - 1);
            auto values = StringUtils::Split(contents_str);
            auto s = values.size();
            for(std::size_t i = 0; i < s; ++i) {
                switch(i) {
                    case 0: x = std::stoi(values[i]);
                    case 1: y = std::stoi(values[i]);
                    default: break;
                }
            }
        }
    }
}

IntVector2 IntVector2::operator-() const {
    return IntVector2(-x, -y);
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
