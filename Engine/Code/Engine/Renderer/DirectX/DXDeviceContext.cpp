#include "Engine/Renderer/DirectX/DXDeviceContext.hpp"

#include "Engine/Core/Rgba.hpp"

#include "Engine/Renderer/DirectX/DX11.hpp"

#include "Engine/Renderer/Texture2D.hpp"
#include "Engine/Renderer/DirectX/DXTexture2D.hpp"

DXDeviceContext::DXDeviceContext(RHIDevice* parentDevice /*= nullptr*/, ID3D11DeviceContext* dx_context /*= nullptr*/)
 : RHIDeviceContext(parentDevice)
, _dx_context(dx_context) {
    /* DO NOTHING */
}

DXDeviceContext::~DXDeviceContext() {
    UnbindAllShaderResources();
    _dx_context->Release();
    _dx_context = nullptr;
}

void DXDeviceContext::ClearState() {
    _dx_context->ClearState();
}
void DXDeviceContext::Flush() {
    _dx_context->Flush();
}

void DXDeviceContext::ClearColorTarget(Texture2D* output, const Rgba& color) {
    if(!output) {
        return;
    }
    DXTexture2D* dx_output = dynamic_cast<DXTexture2D*>(output);
    auto rtv = dx_output->GetRenderTargetView();
    float colorAsFloats[4];
    color.GetAsFloats(colorAsFloats[0], colorAsFloats[1], colorAsFloats[2], colorAsFloats[3]);
    _dx_context->ClearRenderTargetView(rtv, colorAsFloats);
}

void DXDeviceContext::ClearDepthStencilTarget(Texture2D* output
                                               , bool depth /*= true*/
                                               , bool stencil /*= true*/
                                               , float depthValue /*= 1.0f*/
                                               , unsigned char stencilValue /*= 0*/) {
    if(!output) {
        return;
    }
    DXTexture2D* dx_output = dynamic_cast<DXTexture2D*>(output);
    auto dsv = dx_output->GetDepthStencilView();
    if(!dsv) {
        return;
    }
    D3D11_CLEAR_FLAG clearflag;
    if(depth) {
        clearflag = (D3D11_CLEAR_FLAG)(clearflag | D3D11_CLEAR_DEPTH);
    }
    if(stencil) {
        clearflag = (D3D11_CLEAR_FLAG)(clearflag | D3D11_CLEAR_STENCIL);
    }
    _dx_context->ClearDepthStencilView(dsv, clearflag, depthValue, stencilValue);
}

ID3D11DeviceContext* DXDeviceContext::GetDxContext() const {
    return _dx_context;
}

void DXDeviceContext::UnbindAllShaderResources() {
    ID3D11ShaderResourceView* nosrvs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };
    _dx_context->VSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, nosrvs);
    _dx_context->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, nosrvs);
    _dx_context->CSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, nosrvs);
}
