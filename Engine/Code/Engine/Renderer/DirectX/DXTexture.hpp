#pragma once

#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/IntVector3.hpp"

class RHIDevice;
struct ID3D11Texture1D;
struct ID3D11Texture2D;
struct ID3D11Texture3D;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11DepthStencilView;
struct ID3D11UnorderedAccessView;

class DXTexture : public Texture {
public:
    DXTexture() noexcept;
    DXTexture(const DXTexture& other) noexcept = delete;
    DXTexture(DXTexture&& r_other) noexcept;
    DXTexture& operator=(const DXTexture& rhs) noexcept = delete;
    DXTexture& operator=(DXTexture&& r_rhs) noexcept;
    virtual ~DXTexture() = 0;

    virtual bool IsValid() const noexcept = 0;
    bool IsRenderTarget() const noexcept;

    virtual void SetDebugName(char const* name) noexcept = 0;

    ID3D11RenderTargetView* GetRenderTargetView() noexcept;
    ID3D11ShaderResourceView* GetShaderResourceView() noexcept;
    ID3D11DepthStencilView* GetDepthStencilView() noexcept;
    ID3D11UnorderedAccessView* GetUnorderedAccessView() noexcept;

protected:

    ID3D11RenderTargetView* _dx_rtv = nullptr;
    ID3D11ShaderResourceView* _dx_srv = nullptr;
    ID3D11DepthStencilView* _dx_dsv = nullptr;
    ID3D11UnorderedAccessView* _dx_uav = nullptr;

private:

};