#pragma once

#include "Engine/Renderer/DirectX/DXTexture.hpp"

class DXDevice;

class DXTexture1D : public DXTexture {
public:
    DXTexture1D() = default;
    DXTexture1D(DXDevice* device, ID3D11Texture1D* texture);
    DXTexture1D(const DXTexture1D& other) noexcept = delete;
    DXTexture1D(DXTexture1D&& r_other) noexcept;
    DXTexture1D& operator=(const DXTexture1D& rhs) noexcept = delete;
    DXTexture1D& operator=(DXTexture1D&& r_rhs) noexcept;
    virtual ~DXTexture1D();

    void SetDeviceAndTexture(DXDevice* device, ID3D11Texture1D* texture);
    virtual bool IsValid() const noexcept override;

    virtual void SetDebugName([[maybe_unused]] char const* name) noexcept override;

protected:
private:
    ID3D11Texture1D* _dx_resource = nullptr;
};
