#pragma once

#include "Engine/Renderer/DirectX/DXTexture.hpp"

class DXDevice;

class DXTexture3D : public DXTexture {
public:
    DXTexture3D() = default;
    DXTexture3D(DXDevice* device, ID3D11Texture3D* texture);
    DXTexture3D(const DXTexture3D& other) noexcept = delete;
    DXTexture3D(DXTexture3D&& r_other) noexcept;
    DXTexture3D& operator=(const DXTexture3D& rhs) noexcept = delete;
    DXTexture3D& operator=(DXTexture3D&& r_rhs) noexcept;
    virtual ~DXTexture3D();

    void SetDeviceAndTexture(DXDevice* device, ID3D11Texture3D* texture);
    virtual bool IsValid() const noexcept override;

    virtual void SetDebugName([[maybe_unused]] char const* name) noexcept override;

protected:
private:
    ID3D11Texture3D * _dx_resource = nullptr;
};