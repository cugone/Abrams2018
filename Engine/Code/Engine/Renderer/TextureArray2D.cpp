#include "Engine/Renderer/TextureArray2D.hpp"


#include "Engine/Core/BuildConfig.cpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Renderer/DirectX/DX11.hpp"

#include "Engine/RHI/RHIDevice.hpp"

TextureArray2D::TextureArray2D(RHIDevice* device, ID3D11Texture2D* dxTexture)
    : Texture(device)
    , _dx_tex(dxTexture)
{
    SetDeviceAndTexture(device, _dx_tex);
}

TextureArray2D::TextureArray2D()
    : Texture()
{
    _isArray = true;
}

void TextureArray2D::SetDebugName([[maybe_unused]] const std::string& name) noexcept {
#ifdef RENDER_DEBUG
    _dx_tex->SetPrivateData(WKPDID_D3DDebugObjectName, name.size(), name.data());
#endif
}

TextureArray2D::~TextureArray2D() {
    _device = nullptr;
    _dx_tex->Release();
    _dx_tex = nullptr;
}

TextureArray2D::TextureArray2D(TextureArray2D&& r_other) noexcept
    : Texture(std::move(r_other))
    , _dx_tex(std::move(r_other._dx_tex))
{
    r_other._dx_tex = nullptr;
}

TextureArray2D& TextureArray2D::operator=(TextureArray2D&& rhs) noexcept {
    Texture::operator=(std::move(rhs));
    _dx_tex = std::move(rhs._dx_tex);
    rhs._dx_tex = nullptr;
    return *this;
}

void TextureArray2D::SetDeviceAndTexture(RHIDevice* device, ID3D11Texture2D* texture) noexcept {

    _device = device;
    _dx_tex = texture;

    D3D11_TEXTURE2D_DESC t_desc;
    _dx_tex->GetDesc(&t_desc);
    auto depth = t_desc.ArraySize;
    _dimensions = IntVector3(t_desc.Width, t_desc.Height, depth);
    _isArray = true;

    bool success = true;
    if(t_desc.BindFlags & D3D11_BIND_RENDER_TARGET) {
        success &= SUCCEEDED(_device->GetDxDevice()->CreateRenderTargetView(_dx_tex, nullptr, &_rtv)) == true;
    }

    if(t_desc.BindFlags & D3D11_BIND_SHADER_RESOURCE) {
        success &= SUCCEEDED(_device->GetDxDevice()->CreateShaderResourceView(_dx_tex, nullptr, &_srv)) == true;
    }

    if(t_desc.BindFlags & D3D11_BIND_DEPTH_STENCIL) {
        success &= SUCCEEDED(_device->GetDxDevice()->CreateDepthStencilView(_dx_tex, nullptr, &_dsv));
    }

    if(t_desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS) {
        D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
        desc.Texture2D.MipSlice = 0;
        desc.Format = t_desc.Format;

        success &= SUCCEEDED(_device->GetDxDevice()->CreateUnorderedAccessView(_dx_tex, &desc, &_uav));
    }
    ASSERT_OR_DIE(success, "Set device and texture failed.");
}