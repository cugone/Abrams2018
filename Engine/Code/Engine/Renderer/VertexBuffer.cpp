#include "Engine/Renderer/VertexBuffer.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"


VertexBuffer::VertexBuffer(RHIDevice* owner, const buffer_t& buffer, const BufferUsage& usage, const BufferBindUsage& bindUsage)
    : ArrayBuffer<Vertex3D>() {
    D3D11_BUFFER_DESC buffer_desc = {};
    buffer_desc.Usage = BufferUsageToD3DUsage(usage);
    buffer_desc.BindFlags = BufferBindUsageToD3DBindFlags(bindUsage);
    buffer_desc.CPUAccessFlags = CPUAccessFlagFromUsage(usage);
    buffer_desc.StructureByteStride = sizeof(arraybuffer_t);
    buffer_desc.ByteWidth = sizeof(arraybuffer_t) * static_cast<unsigned int>(buffer.size());
    //MiscFlags are unused.

    D3D11_SUBRESOURCE_DATA init_data = {};
    init_data.pSysMem = buffer.data();

    _dx_buffer = nullptr;
    HRESULT hr = owner->GetDxDevice()->CreateBuffer(&buffer_desc, &init_data, &_dx_buffer);
    bool succeeded = SUCCEEDED(hr);
    if(!succeeded) {
        ERROR_AND_DIE("VertexBuffer failed to create.");
    }
}

VertexBuffer::~VertexBuffer() {
    if(IsValid()) {
        _dx_buffer->Release();
        _dx_buffer = nullptr;
    }
}

void VertexBuffer::Update(RHIDeviceContext* context, const buffer_t& buffer) {
    D3D11_MAPPED_SUBRESOURCE resource = {};
    auto dx_context = context->GetDxContext();
    HRESULT hr = dx_context->Map(_dx_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0U, &resource);
    bool succeeded = SUCCEEDED(hr);
    if(succeeded) {
        std::memcpy(resource.pData, buffer.data(), sizeof(arraybuffer_t) * buffer.size());
        dx_context->Unmap(_dx_buffer, 0);
    }
}
