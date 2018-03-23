#include "Engine/Renderer/DirectX/DX11Renderer.hpp"

#include <filesystem>

#include "Engine/Core/Image.hpp"

#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/RHI/RHIInstance.hpp"
#include "Engine/Renderer/DirectX/DXInstance.hpp"
#include "Engine/Renderer/DirectX/DXDevice.hpp"
#include "Engine/Renderer/DirectX/DXDeviceContext.hpp"
#include "Engine/Renderer/DirectX/DXOutput.hpp"
#include "Engine/Renderer/DirectX/DXDepthStencilState.hpp"

#include "Engine/Renderer/Texture2D.hpp"
#include "Engine/Renderer/DepthStencilState.hpp"
#include "Engine/Renderer/DirectX/DXTexture1D.hpp"
#include "Engine/Renderer/DirectX/DXTexture2D.hpp"
#include "Engine/Renderer/DirectX/DXTexture3D.hpp"

DX11Renderer::DX11Renderer(unsigned int width, unsigned int height)
    : Renderer(width, height)
{
    /* DO NOTHING */
}

DX11Renderer::~DX11Renderer() {
    _dx_rhiinstance = nullptr;
    _dx_rhioutput   = nullptr;
    _dx_rhidevice   = nullptr;
    _dx_rhicontext  = nullptr;
}

void DX11Renderer::Initialize() {
    _rhi_instance = RHIInstance::CreateInstance(RHIInstanceType::DIRECTX);
    _rhi_output = _rhi_instance->CreateOutput(_window_dimensions, IntVector2::ZERO, _current_outputMode);
    _rhi_device = _rhi_output->GetParentDevice();
    _rhi_context = _rhi_device->GetImmediateContext();

    _dx_rhiinstance = dynamic_cast<DXInstance*>(_rhi_instance);
    _dx_rhioutput = dynamic_cast<DXOutput*>(_rhi_output);
    _dx_rhidevice = dynamic_cast<DXDevice*>(_rhi_device);
    _dx_rhicontext = dynamic_cast<DXDeviceContext*>(_rhi_context);

    _default_depthstencil = CreateDepthStencil(_dx_rhidevice, _window_dimensions);
    SetRenderTarget(nullptr, _default_depthstencil);

}

void DX11Renderer::SetRenderTarget(Texture* color_target /*= nullptr*/, Texture* depthstencil_target /*= nullptr*/) {
    if(color_target != nullptr) {
        _current_target = color_target;
    } else {
        _current_target = _rhi_output->GetBackBuffer();
    }

    ID3D11DepthStencilView* dsv = nullptr;
    if(depthstencil_target != nullptr) {
        _current_depthstencil = depthstencil_target;
    } else {
        _current_depthstencil = _default_depthstencil;
    }
    auto dx_depthstencil = dynamic_cast<DXTexture2D*>(_current_depthstencil);
    dsv = dx_depthstencil->GetDepthStencilView();

    auto dx_target = dynamic_cast<DXTexture2D*>(_current_target);
    ID3D11RenderTargetView* rtv = dx_target->GetRenderTargetView();

    _dx_rhicontext->GetDxContext()->OMSetRenderTargets(1, &rtv, dsv);
}

void DX11Renderer::SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
    D3D11_VIEWPORT viewport;
    memset(&viewport, 0, sizeof(viewport));

    viewport.TopLeftX = static_cast<float>(x);
    viewport.TopLeftY = static_cast<float>(y);
    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    _dx_rhicontext->GetDxContext()->RSSetViewports(1, &viewport);
}

void DX11Renderer::SetViewportAsPercent(float /*x*/, float /*y*/, float /*w*/, float /*h*/) {
    /* DO NOTHING */
}

void DX11Renderer::ClearColor(const Rgba& color) {
    _dx_rhicontext->ClearColorTarget(_current_target, color);
}

void DX11Renderer::ClearTargetColor(Texture* target, const Rgba& color) {
    _dx_rhicontext->ClearColorTarget(target, color);
}

void DX11Renderer::ClearDepthStencilBuffer() {
    _dx_rhicontext->ClearDepthStencilTarget(_current_depthstencil);
}

void DX11Renderer::Present(bool vsync /*= false*/) {
    _dx_rhioutput->Present(vsync);
}

Texture* DX11Renderer::CreateOrGetTexture(const std::wstring& filepath, const IntVector3& dimensions) {
    namespace FS = std::experimental::filesystem;
    FS::path p(filepath);
    auto texture_iter = _textures.find(p.wstring());
    if(texture_iter == _textures.end()) {
        return CreateTexture(filepath, dimensions);
    } else {
        return GetTexture(filepath);
    }
}

Texture* DX11Renderer::CreateTexture(const std::wstring& filepath,
                                     const IntVector3& dimensions /*= IntVector3::XY_AXIS*/,
                                     const BufferUsage& bufferUsage /*= BufferUsage::STATIC*/,
                                     const BufferBindUsage& bindUsage /*= BufferBindUsage::SHADER_RESOURCE*/,
                                     const ImageFormat& imageFormat /*= ImageFormat::R8G8B8A8_UNORM*/) {
    if(dimensions.y == 0 && dimensions.z == 0) {
        return Create1DTexture(filepath, bufferUsage, bindUsage, imageFormat);
    }
    if(dimensions.z == 0) {
        return Create2DTexture(filepath, bufferUsage, bindUsage, imageFormat);
    }
    return Create3DTexture(filepath, dimensions, bufferUsage, bindUsage, imageFormat);
}

void DX11Renderer::SetVSync(bool vsync_setting) {
    _vsync_setting = vsync_setting;
}

void DX11Renderer::BeginFrame() {
    /* DO NOTHING */
}

void DX11Renderer::Update(float /*deltaSeconds*/) {
    /* DO NOTHING */
}

void DX11Renderer::Render() const {
    /* DO NOTHING */
}

void DX11Renderer::EndFrame() {
    Present(_vsync_setting);
}

void DX11Renderer::SetTexture(Texture* texture, unsigned int registerIndex /*= 0*/) {
    if(_current_target == texture) {
        return;
    }
    if(texture == nullptr) {
        texture = GetTexture(L"__default");
    }
    _current_target = texture;
    _dx_rhicontext->SetTexture(registerIndex, _current_target);
}

Texture* DX11Renderer::CreateDepthStencil(RHIDevice* owner, const IntVector2& dimensions) {

    DXDevice* dx_owner = dynamic_cast<DXDevice*>(owner);

    ID3D11Texture2D* dx_resource = nullptr;

    D3D11_TEXTURE2D_DESC descDepth;
    descDepth.Width = dimensions.x;
    descDepth.Height = dimensions.y;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    HRESULT texture_hr = dx_owner->GetDxDevice()->CreateTexture2D(&descDepth, nullptr, &dx_resource);
    bool texture_creation_succeeded = SUCCEEDED(texture_hr);
    if(texture_creation_succeeded) {
        if(_current_depthstencil) {
            delete _current_depthstencil;
            _current_depthstencil = nullptr;
        }
        if(_current_depthstencil_state) {
            delete _current_depthstencil_state;
            _current_depthstencil_state = nullptr;
        }
        _current_depthstencil = new DXTexture2D(dx_owner, dx_resource);
        _current_depthstencil_state = new DXDepthStencilState(dx_owner);
        return _current_depthstencil;
    }
    return nullptr;
}

Texture* DX11Renderer::Create1DTexture(const std::wstring& filepath, const BufferUsage& bufferUsage, const BufferBindUsage& bindUsage, const ImageFormat& imageFormat) {
    namespace FS = std::experimental::filesystem;
    FS::path p(filepath);
    if(!FS::exists(p)) {
        return GetTexture(L"__invalid");
    }
    Image img = Image(p.wstring());

    D3D11_TEXTURE1D_DESC tex_desc;
    memset(&tex_desc, 0, sizeof(tex_desc));

    tex_desc.Width = img.GetDimensions().x;     // width... 
    tex_desc.MipLevels = 1;    // setting to 0 means there's a full chain (or can generate a full chain) - we're immutable, so not allowed
    tex_desc.ArraySize = 1;    // only one texture (
    tex_desc.Usage = BufferUsageToD3DUsage(bufferUsage);            // data is set at creation time and won't change
    tex_desc.Format = ImageFormatToDxgiFormat(imageFormat);      // R8G8B8A8 texture
    tex_desc.BindFlags = BufferBindUsageToD3DBindFlags(bindUsage);   // we're going to be using this texture as a shader resource
    tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(bufferUsage);                      // Determines how I can access this resource CPU side (IMMUTABLE, So none)
    tex_desc.MiscFlags = 0;                            // Extra Flags, of note is;
        
    // Setup Initial Data
    D3D11_SUBRESOURCE_DATA subresource_data;
    memset(&subresource_data, 0, sizeof(subresource_data));

    auto width = img.GetDimensions().x;
    auto height = img.GetDimensions().y;
    subresource_data.pSysMem = img.GetData();
    subresource_data.SysMemPitch = width * sizeof(unsigned int); // pitch is byte size of a single row)
    subresource_data.SysMemSlicePitch = width * height * sizeof(unsigned int);
    //Force specific usages for unordered access
    if((bindUsage & BufferBindUsage::UNORDERED_ACCESS) == BufferBindUsage::UNORDERED_ACCESS) {
        tex_desc.Usage = BufferUsageToD3DUsage(BufferUsage::GPU);
        tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(BufferUsage::STAGING);
    }
    ID3D11Texture1D* dx_tex = nullptr;

    bool isImmutable = bufferUsage == BufferUsage::STATIC;
    bool mustUseInitialData = isImmutable;

    HRESULT hr = _dx_rhidevice->GetDxDevice()->CreateTexture1D(&tex_desc, (mustUseInitialData ? &subresource_data : nullptr), &dx_tex);
    bool succeeded = SUCCEEDED(hr);
    if(succeeded) {
        auto* tex = new DXTexture1D(_dx_rhidevice, dx_tex);
        tex->SetDebugName(p.string().c_str());
        tex->IsLoaded(true);
        if(RegisterTexture(p.wstring(), tex)) {
            return tex;
        } else {
            delete tex;
            return nullptr;
        }
    } else {
        return nullptr;
    }
}

Texture* DX11Renderer::Create2DTexture(const std::wstring& filepath, const BufferUsage& bufferUsage, const BufferBindUsage& bindUsage, const ImageFormat& imageFormat) {
    namespace FS = std::experimental::filesystem;
    FS::path p(filepath);
    if(!FS::exists(p)) {
        return GetTexture(L"__invalid");
    }
    Image img = Image(p.wstring());

    D3D11_TEXTURE2D_DESC tex_desc;
    memset(&tex_desc, 0, sizeof(tex_desc));

    tex_desc.Width = img.GetDimensions().x;     // width... 
    tex_desc.Height = img.GetDimensions().y;    // ...and height of image in pixels.
    tex_desc.MipLevels = 1;    // setting to 0 means there's a full chain (or can generate a full chain) - we're immutable, so not allowed
    tex_desc.ArraySize = 1;    // only one texture (
    tex_desc.Usage = BufferUsageToD3DUsage(bufferUsage);            // data is set at creation time and won't change
    tex_desc.Format = ImageFormatToDxgiFormat(imageFormat);      // R8G8B8A8 texture
    tex_desc.BindFlags = BufferBindUsageToD3DBindFlags(bindUsage);   // we're going to be using this texture as a shader resource
                                                                     //Make every texture a shader resource
    tex_desc.BindFlags |= BufferBindUsageToD3DBindFlags(BufferBindUsage::SHADER_RESOURCE);
    tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(bufferUsage);                      // Determines how I can access this resource CPU side (IMMUTABLE, So none)
    tex_desc.MiscFlags = 0;                            // Extra Flags, of note is;
                                                       // D3D11_RESOURCE_MISC_GENERATE_MIPS - if we want to use this to be able to generate mips (not compatible with IMMUTABLE)

                                                       // If Multisampling - set this up - we're not multisampling, so 1 and 0
                                                       // (MSAA as far as I know only makes sense for Render Targets, not shader resource textures)
    tex_desc.SampleDesc.Count = 1;
    tex_desc.SampleDesc.Quality = 0;

    // Setup Initial Data
    D3D11_SUBRESOURCE_DATA subresource_data;
    memset(&subresource_data, 0, sizeof(subresource_data));

    auto width = img.GetDimensions().x;
    auto height = img.GetDimensions().y;
    subresource_data.pSysMem = img.GetData();
    subresource_data.SysMemPitch = width * sizeof(unsigned int); // pitch is byte size of a single row)
    subresource_data.SysMemSlicePitch = width * height * sizeof(unsigned int);
    //Force specific usages for unordered access
    if((bindUsage & BufferBindUsage::UNORDERED_ACCESS) == BufferBindUsage::UNORDERED_ACCESS) {
        tex_desc.Usage = BufferUsageToD3DUsage(BufferUsage::GPU);
        tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(BufferUsage::STAGING);
    }
    ID3D11Texture2D* dx_tex = nullptr;

    //If IMMUTABLE or not multi-sampled, must use initial data.
    bool isMultiSampled = tex_desc.SampleDesc.Count != 1 || tex_desc.SampleDesc.Quality != 0;
    bool isImmutable = bufferUsage == BufferUsage::STATIC;
    bool mustUseInitialData = isImmutable || !isMultiSampled;

    HRESULT hr = _dx_rhidevice->GetDxDevice()->CreateTexture2D(&tex_desc, (mustUseInitialData ? &subresource_data : nullptr), &dx_tex);
    bool succeeded = SUCCEEDED(hr);
    if(succeeded) {
        auto* tex = new DXTexture2D(_dx_rhidevice, dx_tex);
        tex->SetDebugName(p.string().c_str());
        tex->IsLoaded(true);
        if(RegisterTexture(p.wstring(), tex)) {
            return tex;
        } else {
            delete tex;
            return nullptr;
        }
    } else {
        return nullptr;
    }
}

Texture* DX11Renderer::Create3DTexture(const std::wstring& filepath, const IntVector3& dimensions, const BufferUsage& bufferUsage, const BufferBindUsage& bindUsage, const ImageFormat& imageFormat) {
    namespace FS = std::experimental::filesystem;
    FS::path p(filepath);
    if(!FS::exists(p)) {
        return GetTexture(L"__invalid");
    }

    D3D11_TEXTURE3D_DESC tex_desc;
    memset(&tex_desc, 0, sizeof(tex_desc));

    tex_desc.Width = dimensions.x;     // width... 
    tex_desc.Height = dimensions.y;    // ...and height of image in pixels.
    tex_desc.Depth = dimensions.z;
    tex_desc.MipLevels = 1;    // setting to 0 means there's a full chain (or can generate a full chain) - we're immutable, so not allowed
    tex_desc.Usage = BufferUsageToD3DUsage(bufferUsage);            // data is set at creation time and won't change
    tex_desc.Format = ImageFormatToDxgiFormat(imageFormat);      // R8G8B8A8 texture
    tex_desc.BindFlags = BufferBindUsageToD3DBindFlags(bindUsage);   // we're going to be using this texture as a shader resource
                                                                     //Make every texture a shader resource
    tex_desc.BindFlags |= BufferBindUsageToD3DBindFlags(BufferBindUsage::SHADER_RESOURCE);
    tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(bufferUsage);                      // Determines how I can access this resource CPU side (IMMUTABLE, So none)
    tex_desc.MiscFlags = 0;                            // Extra Flags, of note is;
                                                       // D3D11_RESOURCE_MISC_GENERATE_MIPS - if we want to use this to be able to generate mips (not compatible with IMMUTABLE)

                                                       // If Multisampling - set this up - we're not multisampling, so 1 and 0
                                                       // (MSAA as far as I know only makes sense for Render Targets, not shader resource textures)

    // Setup Initial Data
    D3D11_SUBRESOURCE_DATA subresource_data;
    memset(&subresource_data, 0, sizeof(subresource_data));

    std::vector<unsigned char> data;
    FileUtils::ReadBufferFromFile(data, filepath);
    auto width = dimensions.x;
    auto height = dimensions.y;
    subresource_data.pSysMem = data.data();
    subresource_data.SysMemPitch = width * sizeof(unsigned int); // pitch is byte size of a single row)
    subresource_data.SysMemSlicePitch = width * height * sizeof(unsigned int);
    //Force specific usages for unordered access
    if((bindUsage & BufferBindUsage::UNORDERED_ACCESS) == BufferBindUsage::UNORDERED_ACCESS) {
        tex_desc.Usage = BufferUsageToD3DUsage(BufferUsage::GPU);
        tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(BufferUsage::STAGING);
    }
    ID3D11Texture3D* dx_tex = nullptr;

    //If IMMUTABLE or not multi-sampled, must use initial data.
    bool isMultiSampled = false;
    bool isImmutable = bufferUsage == BufferUsage::STATIC;
    bool mustUseInitialData = isImmutable || !isMultiSampled;

    HRESULT hr = _dx_rhidevice->GetDxDevice()->CreateTexture3D(&tex_desc, (mustUseInitialData ? &subresource_data : nullptr), &dx_tex);
    bool succeeded = SUCCEEDED(hr);
    if(succeeded) {
        auto* tex = new DXTexture3D(_dx_rhidevice, dx_tex);
        tex->SetDebugName(p.string().c_str());
        tex->IsLoaded(true);
        if(RegisterTexture(p.wstring(), tex)) {
            return tex;
        } else {
            delete tex;
            return nullptr;
        }
    } else {
        return nullptr;
    }
}
