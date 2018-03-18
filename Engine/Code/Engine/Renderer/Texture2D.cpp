#include "Engine/Renderer/Texture2D.hpp"

#include "Engine/Renderer/DirectX/DXDevice.hpp"
#include "Engine/Renderer/OpenGL/OGLDevice.hpp"

Texture2D::Texture2D(RHIDevice* device)
    : Texture(device)
{
    /* DO NOTHING */
}

Texture2D::~Texture2D() {
    _device = nullptr;
}

Texture2D::Texture2D(Texture2D&& r_other) noexcept
    : Texture(std::move(r_other))
{
    /* DO NOTHING */
}

Texture2D& Texture2D::operator=(Texture2D&& rhs) noexcept {
    Texture::operator=(std::move(rhs));
    return *this;
}
