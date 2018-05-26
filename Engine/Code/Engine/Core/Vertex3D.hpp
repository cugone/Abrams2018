#pragma once

#include "Engine/Core/Rgba.hpp"

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"

class InputLayout;

class Vertex3D {
public:

    Vertex3D(const Vector3& pos = Vector3::ZERO, const Rgba& color = Rgba::WHITE, const Vector2& tex_coords = Vector2::ZERO);
    Vertex3D(const Vertex3D& other) = default;
    Vertex3D(Vertex3D&& other) = default;
    Vertex3D& operator=(const Vertex3D& other) = default;
    Vertex3D& operator=(Vertex3D&& other) = default;
    Vector3 position = Vector3::ZERO;
    Rgba color = Rgba::WHITE;
    Vector2 texcoords = Vector2::ZERO;

protected:
private:
};

inline Vertex3D::Vertex3D(const Vector3& pos /*= Vector3::ZERO*/, const Rgba& color /*= Rgba::WHITE*/, const Vector2& tex_coords /*= Vector2::ZERO*/)
    : position(pos)
    , color(color)
    , texcoords(tex_coords)
{
    /* DO NOTHING */
}