#include "Engine/Renderer/Texture.hpp"

Texture::Texture(RHIDevice* device) noexcept
    : _device(device)
{
    /* DO NOTHING */
}

Texture::Texture(Texture&& r_other) noexcept
: _device(r_other._device)
,_dimensions(r_other._dimensions)
,_isLoaded(r_other._isLoaded)
{
    r_other._device = nullptr;
    r_other._dimensions = IntVector3::ZERO;
    r_other._isLoaded = false;
}

Texture& Texture::operator=(Texture&& rhs) noexcept {
    _device = rhs._device;
    _dimensions = rhs._dimensions;
    _isLoaded = rhs._isLoaded;

    rhs._device = nullptr;
    rhs._dimensions = IntVector3::ZERO;
    rhs._isLoaded = false;

    return *this;
}

Texture::~Texture() noexcept {
    _device = nullptr;
}

const IntVector3& Texture::GetDimensions() const noexcept {
    return _dimensions;
}

void Texture::IsLoaded(bool is_loaded) noexcept {
    _isLoaded = is_loaded;
}

bool Texture::IsLoaded() const noexcept {
    return _isLoaded;
}

