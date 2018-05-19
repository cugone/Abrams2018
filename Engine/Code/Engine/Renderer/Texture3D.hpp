#pragma once

#include "Engine/Renderer/Texture.hpp"

class RHIDevice;

struct ID3D11Texture3D;

class Texture3D : public Texture {
public:
    Texture3D() = default;
    Texture3D(RHIDevice* device, ID3D11Texture3D* dxTexture);
    Texture3D(Texture3D&& r_other) noexcept;
    Texture3D(const Texture3D& other) noexcept = delete;
    Texture3D& operator=(const Texture3D& rhs) noexcept = delete;
    Texture3D& operator=(Texture3D&& rhs) noexcept;

    virtual void SetDebugName([[maybe_unused]] const std::string& name) noexcept;

    virtual ~Texture3D();

protected:
private:
    ID3D11Texture3D* _dx_tex = nullptr;
};