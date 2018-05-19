#include "Engine/Renderer/ConstantBuffer.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"


ConstantBuffer::ConstantBuffer(RHIDevice* owner, const buffer_t& buffer, std::size_t buffer_size, const BufferUsage& usage, const BufferBindUsage& bindUsage)
    : Buffer<void*>()
    , _buffer_size(buffer_size)
{
    D3D11_BUFFER_DESC buffer_desc = {};
    buffer_desc.Usage = BufferUsageToD3DUsage(usage);
    buffer_desc.BindFlags = BufferBindUsageToD3DBindFlags(bindUsage);
    buffer_desc.CPUAccessFlags = CPUAccessFlagFromUsage(usage);
    buffer_desc.StructureByteStride = 0;
    buffer_desc.ByteWidth = _buffer_size;
    //MiscFlags are unused.

    D3D11_SUBRESOURCE_DATA init_data = {};
    init_data.pSysMem = buffer;

    _dx_buffer = nullptr;
    HRESULT hr = owner->GetDxDevice()->CreateBuffer(&buffer_desc, &init_data, &_dx_buffer);
    bool succeeded = SUCCEEDED(hr);
    if(!succeeded) {
        ERROR_AND_DIE("ConstantBuffer failed to create.");
    }
}

ConstantBuffer::~ConstantBuffer() {
    if(IsValid()) {
        _dx_buffer->Release();
        _dx_buffer = nullptr;
    }
}

void ConstantBuffer::Update(RHIDeviceContext* context, const buffer_t& buffer) {
    D3D11_MAPPED_SUBRESOURCE resource = {};
    auto dx_context = context->GetDxContext();
    HRESULT hr = dx_context->Map(_dx_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0U, &resource);
    bool succeeded = SUCCEEDED(hr);
    if(succeeded) {
        std::memcpy(resource.pData, buffer, _buffer_size);
        dx_context->Unmap(_dx_buffer, 0);
    }
}
