#include "Engine/RHI/RHIDeviceContext.hpp"

#include "Engine/Core/Rgba.hpp"

#include "Engine/Renderer/Texture.hpp"

#include "Engine/Renderer/BlendState.hpp"
#include "Engine/Renderer/Buffer.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Renderer/DepthStencilState.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/InputLayout.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/Renderer/StructuredBuffer.hpp"
#include "Engine/Renderer/RasterState.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"

RHIDeviceContext::RHIDeviceContext(RHIDevice* parentDevice, ID3D11DeviceContext* deviceContext)
    : _device(parentDevice)
    , _dx_context(deviceContext)
{
    /* DO NOTHING */
}

RHIDeviceContext::~RHIDeviceContext() {
    _device = nullptr;
    _dx_context->Release();
    _dx_context = nullptr;
}

void RHIDeviceContext::ClearState() {
    _dx_context->ClearState();
}

void RHIDeviceContext::Flush() {
    _dx_context->Flush();
}

void RHIDeviceContext::ClearColorTarget(Texture* output, const Rgba& color) {
    float color_as_floats[4];
    color.GetAsFloats(color_as_floats[0], color_as_floats[1], color_as_floats[2], color_as_floats[3]);
    _dx_context->ClearRenderTargetView(output->GetRenderTargetView(), color_as_floats);
}

void RHIDeviceContext::ClearDepthStencilTarget(Texture* output, bool depth /*= true */, bool stencil /*= true */, float depthValue /*= 1.0f */, unsigned char stencilValue /*= 0*/) {
    unsigned int clear_flag = 0;
    if(depth) {
        clear_flag |= D3D11_CLEAR_DEPTH;
    }
    if(stencil) {
        clear_flag |= D3D11_CLEAR_STENCIL;
    }
    _dx_context->ClearDepthStencilView(output->GetDepthStencilView(), clear_flag, depthValue, stencilValue);
}

void RHIDeviceContext::SetMaterial(Material* material) {
    if(material == nullptr) {
        return;
    }
    SetShader(material->GetShader());
    auto tex_count = material->GetTextureCount();
    for(unsigned int i = 0; i < tex_count; ++i) {
        SetTexture(i, material->GetTexture(i));
    }
}

void RHIDeviceContext::SetTexture(unsigned int index, Texture* texture) {
    if(texture) {
        ID3D11ShaderResourceView* srvs = { texture->GetShaderResourceView() };
        _dx_context->VSSetShaderResources(index, 1, &srvs);
        _dx_context->PSSetShaderResources(index, 1, &srvs);
        _dx_context->DSSetShaderResources(index, 1, &srvs);
        _dx_context->HSSetShaderResources(index, 1, &srvs);
        _dx_context->GSSetShaderResources(index, 1, &srvs);
        _dx_context->CSSetShaderResources(index, 1, &srvs);
    } else {
        ID3D11ShaderResourceView* no_srvs = { nullptr };
        _dx_context->VSSetShaderResources(index, 1, &no_srvs);
        _dx_context->PSSetShaderResources(index, 1, &no_srvs);
        _dx_context->DSSetShaderResources(index, 1, &no_srvs);
        _dx_context->HSSetShaderResources(index, 1, &no_srvs);
        _dx_context->GSSetShaderResources(index, 1, &no_srvs);
        _dx_context->CSSetShaderResources(index, 1, &no_srvs);
    }
}

void RHIDeviceContext::SetVertexBuffer(unsigned int startIndex, VertexBuffer* buffer) {
    ID3D11Buffer* const dx_buffer = buffer->GetDxBuffer();
    ID3D11Buffer* nobuffer[1] = { nullptr };
    unsigned int stride = sizeof(VertexBuffer::buffer_t);
    unsigned int offsets = 0;
    if(buffer) {
        _dx_context->IASetVertexBuffers(startIndex, 1, &dx_buffer, &stride, &offsets);
    } else {
        _dx_context->IASetVertexBuffers(startIndex, 1, nobuffer, &stride, &offsets);
    }
}

void RHIDeviceContext::SetIndexBuffer(IndexBuffer* buffer) {
    ID3D11Buffer * const dx_buffer = buffer->GetDxBuffer();
    if(buffer) {
        _dx_context->IASetIndexBuffer(dx_buffer, DXGI_FORMAT_R32_UINT, 0);
    } else {
        _dx_context->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
    }
}

void RHIDeviceContext::SetConstantBuffer(unsigned int index, ConstantBuffer* buffer) {
    ID3D11Buffer * const dx_buffer = buffer->GetDxBuffer();
    ID3D11Buffer* nobuffer[1] = { nullptr };
    if(buffer) {
        _dx_context->VSSetConstantBuffers(index, 1, &dx_buffer);
        _dx_context->PSSetConstantBuffers(index, 1, &dx_buffer);
        _dx_context->DSSetConstantBuffers(index, 1, &dx_buffer);
        _dx_context->HSSetConstantBuffers(index, 1, &dx_buffer);
        _dx_context->GSSetConstantBuffers(index, 1, &dx_buffer);
        _dx_context->CSSetConstantBuffers(index, 1, &dx_buffer);
    } else {
        _dx_context->VSSetConstantBuffers(index, 1, nobuffer);
        _dx_context->PSSetConstantBuffers(index, 1, nobuffer);
        _dx_context->DSSetConstantBuffers(index, 1, nobuffer);
        _dx_context->HSSetConstantBuffers(index, 1, nobuffer);
        _dx_context->GSSetConstantBuffers(index, 1, nobuffer);
        _dx_context->CSSetConstantBuffers(index, 1, nobuffer);
    }
}

void RHIDeviceContext::SetStructuredBuffer(unsigned int index, StructuredBuffer* buffer) {
    ID3D11ShaderResourceView* const dx_buffer = buffer->dx_srv;
    ID3D11ShaderResourceView* nobuffer[1] = { nullptr };
    if(buffer) {
        _dx_context->VSSetShaderResources(index + STRUCTURED_BUFFER_OFFSET, 1, &dx_buffer);
        _dx_context->PSSetShaderResources(index + STRUCTURED_BUFFER_OFFSET, 1, &dx_buffer);
        _dx_context->DSSetShaderResources(index + STRUCTURED_BUFFER_OFFSET, 1, &dx_buffer);
        _dx_context->HSSetShaderResources(index + STRUCTURED_BUFFER_OFFSET, 1, &dx_buffer);
        _dx_context->GSSetShaderResources(index + STRUCTURED_BUFFER_OFFSET, 1, &dx_buffer);
        _dx_context->CSSetShaderResources(index + STRUCTURED_BUFFER_OFFSET, 1, &dx_buffer);
    } else {
        _dx_context->VSSetShaderResources(index + STRUCTURED_BUFFER_OFFSET, 1, nobuffer);
        _dx_context->PSSetShaderResources(index + STRUCTURED_BUFFER_OFFSET, 1, nobuffer);
        _dx_context->DSSetShaderResources(index + STRUCTURED_BUFFER_OFFSET, 1, nobuffer);
        _dx_context->HSSetShaderResources(index + STRUCTURED_BUFFER_OFFSET, 1, nobuffer);
        _dx_context->GSSetShaderResources(index + STRUCTURED_BUFFER_OFFSET, 1, nobuffer);
        _dx_context->CSSetShaderResources(index + STRUCTURED_BUFFER_OFFSET, 1, nobuffer);
    }
}

void RHIDeviceContext::Draw(std::size_t vertexCount, std::size_t startVertex /*= 0*/) {
    _dx_context->Draw(static_cast<unsigned int>(vertexCount), static_cast<unsigned int>(startVertex));
}

void RHIDeviceContext::DrawIndexed(std::size_t vertexCount, std::size_t startVertex /*= 0*/, std::size_t baseVertexLocation /*= 0*/) {
    _dx_context->DrawIndexed(static_cast<unsigned int>(vertexCount), static_cast<unsigned int>(startVertex), static_cast<int>(baseVertexLocation));
}

RHIDevice* RHIDeviceContext::GetParentDevice() const {
    return _device;
}

ID3D11DeviceContext* RHIDeviceContext::GetDxContext() {
    return _dx_context;
}

void RHIDeviceContext::SetShaderProgram(ShaderProgram* shaderProgram /*= nullptr*/) {
    if(shaderProgram == nullptr) {
        _dx_context->IASetInputLayout(nullptr);
        _dx_context->VSSetShader(nullptr, nullptr, 0);
        _dx_context->PSSetShader(nullptr, nullptr, 0);
        _dx_context->DSSetShader(nullptr, nullptr, 0);
        _dx_context->HSSetShader(nullptr, nullptr, 0);
        _dx_context->GSSetShader(nullptr, nullptr, 0);
        _dx_context->CSSetShader(nullptr, nullptr, 0);
    } else {
        _dx_context->IASetInputLayout(shaderProgram->GetInputLayout()->GetDxInputLayout());
        _dx_context->VSSetShader(shaderProgram->GetVS(), nullptr, 0);
        _dx_context->PSSetShader(shaderProgram->GetPS(), nullptr, 0);
        _dx_context->DSSetShader(shaderProgram->GetDS(), nullptr, 0);
        _dx_context->HSSetShader(shaderProgram->GetHS(), nullptr, 0);
        _dx_context->GSSetShader(shaderProgram->GetGS(), nullptr, 0);
        _dx_context->CSSetShader(shaderProgram->GetCS(), nullptr, 0);
    }
}

void RHIDeviceContext::SetDepthStencilState(DepthStencilState* depthStencilState /*= nullptr*/) {
    if(depthStencilState == nullptr) {
        _dx_context->OMSetDepthStencilState(nullptr, 0);
    } else {
        _dx_context->OMSetDepthStencilState(depthStencilState->GetDxDepthStencilState(), 0);
    }
}

void RHIDeviceContext::SetRasterState(RasterState* rasterState /*= nullptr*/) {
    if(rasterState == nullptr) {
        _dx_context->RSSetState(nullptr);
    } else {
        _dx_context->RSSetState(rasterState->GetDxRasterState());
    }
}

void RHIDeviceContext::SetBlendState(BlendState* blendState /*= nullptr*/) {
    unsigned int mask = 0xffffffff;
    float blend_factor[4] = {1, 1, 1, 1};
    if(blendState == nullptr) {
        _dx_context->OMSetBlendState(nullptr, blend_factor, mask);
    } else {
        _dx_context->OMSetBlendState(blendState->GetDxBlendState(), blend_factor, mask);
    }
}

void RHIDeviceContext::UnbindAllShaderResources() {
    ID3D11ShaderResourceView* no_srvs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };
    _dx_context->VSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, &no_srvs[0]);
    _dx_context->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, &no_srvs[0]);
    _dx_context->DSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, &no_srvs[0]);
    _dx_context->HSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, &no_srvs[0]);
    _dx_context->GSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, &no_srvs[0]);
    _dx_context->CSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, &no_srvs[0]);
}

void RHIDeviceContext::SetShader(Shader* shader) {
    if(shader == nullptr) {
        SetShaderProgram(nullptr);
        SetRasterState(nullptr);
        SetDepthStencilState(nullptr);
        SetBlendState(nullptr);
    } else {
        SetShaderProgram(shader->GetShaderProgram());
        SetRasterState(shader->GetRasterState());
        SetDepthStencilState(shader->GetDepthStencilState());
        SetBlendState(shader->GetBlendState());
    }
}
