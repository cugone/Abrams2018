#pragma once

#include "Engine/Renderer/Texture.hpp"

class RHIDevice;

struct ID3D11Texture2D;

class Texture2D : public Texture {
public:
    Texture2D(RHIDevice* device, ID3D11Texture2D* dxTexture);
    Texture2D(Texture2D&& r_other) noexcept;
    Texture2D(const Texture2D& other) noexcept = delete;
    Texture2D& operator=(const Texture2D& rhs) noexcept = delete;
    Texture2D& operator=(Texture2D&& rhs) noexcept;

    virtual void SetDebugName([[maybe_unused]] const std::string& name) noexcept;

    virtual ~Texture2D();

protected:
private:
    ID3D11Texture2D* _dx_tex = nullptr;
    void SetDeviceAndTexture(RHIDevice* device, ID3D11Texture2D* texture) noexcept;
};