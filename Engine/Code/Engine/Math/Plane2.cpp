#include "Engine/Math/Plane2.hpp"

Plane2::Plane2(const Vector2& normal, float distance_from_origin)
    : normal(normal.GetNormalize())
    , dist(distance_from_origin)
{
    /* DO NOTHING */
}
