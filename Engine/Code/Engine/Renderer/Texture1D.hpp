#pragma once

#include "Engine/Renderer/Texture.hpp"

class RHIDevice;

struct ID3D11Texture1D;

class Texture1D : public Texture {
public:
    Texture1D() = default;
    Texture1D(RHIDevice* device, ID3D11Texture1D* dxTexture);
    Texture1D(Texture1D&& r_other) noexcept;
    Texture1D(const Texture1D& other) noexcept = delete;
    Texture1D& operator=(const Texture1D& rhs) noexcept = delete;
    Texture1D& operator=(Texture1D&& rhs) noexcept;

    virtual void SetDebugName([[maybe_unused]] const std::string& name) noexcept;

    virtual ~Texture1D();

protected:
private:
    ID3D11Texture1D* _dx_tex = nullptr;
};