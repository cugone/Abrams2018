#include "Engine/Math/Plane3.hpp"

Plane3::Plane3(const Vector3& normal, float distance_from_origin)
    : normal(normal)
    , dist(distance_from_origin)
{
    /* DO NOTHING */
}
