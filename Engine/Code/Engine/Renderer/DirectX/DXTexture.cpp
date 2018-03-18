#include "Engine/Renderer/DirectX/DXTexture.hpp"

#include "Engine/Renderer/DirectX/DX11.hpp"
#include "Engine/RHI/RHIDevice.hpp"

DXTexture::DXTexture() noexcept {
    /* DO NOTHING */
}

DXTexture::DXTexture(DXTexture&& r_other) noexcept
: Texture(std::move(r_other)) {

    _dx_rtv = r_other._dx_rtv;
    _dx_srv = r_other._dx_srv;
    _dx_dsv = r_other._dx_dsv;
    _dx_uav = r_other._dx_uav;

    r_other._dx_rtv = nullptr;
    r_other._dx_srv = nullptr;
    r_other._dx_dsv = nullptr;
    r_other._dx_uav = nullptr;

}

DXTexture& DXTexture::operator=(DXTexture&& r_rhs) noexcept {

    Texture::operator=(std::move(r_rhs));

    _dx_rtv = r_rhs._dx_rtv;
    _dx_srv = r_rhs._dx_srv;
    _dx_dsv = r_rhs._dx_dsv;
    _dx_uav = r_rhs._dx_uav;

    r_rhs._dx_rtv = nullptr;
    r_rhs._dx_srv = nullptr;
    r_rhs._dx_dsv = nullptr;
    r_rhs._dx_uav = nullptr;

    return *this;
}

DXTexture::~DXTexture() noexcept {
    if(_dx_rtv) {
        _dx_rtv->Release();
        _dx_rtv = nullptr;
    }
    if(_dx_dsv) {
        _dx_dsv->Release();
        _dx_dsv = nullptr;
    }

    if(_dx_srv) {
        _dx_srv->Release();
        _dx_srv = nullptr;
    }

    if(_dx_uav) {
        _dx_uav->Release();
        _dx_uav = nullptr;
    }
}

bool DXTexture::IsRenderTarget() const noexcept {
    return _dx_rtv != nullptr;
}

ID3D11RenderTargetView* DXTexture::GetRenderTargetView() noexcept {
    return _dx_rtv;
}

ID3D11ShaderResourceView* DXTexture::GetShaderResourceView() noexcept {
    return _dx_srv;
}

ID3D11DepthStencilView* DXTexture::GetDepthStencilView() noexcept {
    return _dx_dsv;
}

ID3D11UnorderedAccessView* DXTexture::GetUnorderedAccessView() noexcept {
    return _dx_uav;
}
