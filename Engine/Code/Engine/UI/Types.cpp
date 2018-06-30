#include "Engine/UI/Types.hpp"

namespace UI {

Ratio::Ratio(const Vector2& newValue /*= Vector2::ZERO*/) {
    value = MathUtils::Clamp(newValue, Vector2::ZERO, Vector2::ONE);
}

Ratio& Ratio::operator=(const Ratio& rhs) {
    value = MathUtils::Clamp(rhs.value, Vector2::ZERO, Vector2::ONE);
    return *this;
}

Ratio& Ratio::operator=(Ratio&& rhs) {
    value = std::move(rhs.value);
    value = MathUtils::Clamp(rhs.value, Vector2::ZERO, Vector2::ONE);
    return *this;
}

Ratio::Ratio(Ratio&& rhs) {
    value = MathUtils::Clamp(rhs.value, Vector2::ZERO, Vector2::ONE);
    rhs.value = Vector2::ZERO;
}

Ratio::Ratio(const Ratio& rhs) {
    value = MathUtils::Clamp(rhs.value, Vector2::ZERO, Vector2::ONE);
}

const Vector2& Ratio::GetValue() const {
    return value;
}

void Ratio::SetValue(const Vector2& newValue) {
    value = MathUtils::Clamp(newValue, Vector2::ZERO, Vector2::ONE);
}

} //End UI