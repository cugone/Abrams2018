#include "Engine/Renderer/InputLayout.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Renderer/DirectX/DX11.hpp"

#include "Engine/RHI/RHIDevice.hpp"

InputLayout::InputLayout(RHIDevice* parent_device)
: _parent_device(parent_device)
{
    /* DO NOTHING */
}

InputLayout::~InputLayout() {

    _elements.clear();
    _elements.shrink_to_fit();

    _dx_input_layout->Release();
    _dx_input_layout = nullptr;
}

void InputLayout::AddElement(std::size_t memberByteOffset, const ImageFormat& format, const char* semantic, unsigned int inputSlot /*= 0*/, bool isVertexData /*= true*/, unsigned int instanceDataStepRate /*= 0*/) {
    D3D11_INPUT_ELEMENT_DESC e_desc = {};
    e_desc.Format = ImageFormatToDxgiFormat(format);
    e_desc.InputSlotClass = isVertexData ? D3D11_INPUT_PER_VERTEX_DATA : D3D11_INPUT_PER_INSTANCE_DATA;
    e_desc.InstanceDataStepRate = isVertexData ? 0 : instanceDataStepRate;
    e_desc.SemanticName = semantic;
    e_desc.SemanticIndex = 0;
    e_desc.InputSlot = inputSlot;
    e_desc.AlignedByteOffset = memberByteOffset;
    _elements.push_back(e_desc);
}

void InputLayout::AddElement(const D3D11_INPUT_ELEMENT_DESC& desc) {
    _elements.push_back(desc);
}

void InputLayout::CreateInputLayout(void* byte_code, std::size_t byte_code_length) {
    _elements.shrink_to_fit();
    if(_dx_input_layout) {
        _dx_input_layout->Release();
        _dx_input_layout = nullptr;
    }
    auto dx_device = _parent_device->GetDxDevice();
    HRESULT hr = dx_device->CreateInputLayout(_elements.data(), _elements.size(), byte_code, byte_code_length, &_dx_input_layout);
    bool succeeded = SUCCEEDED(hr);
    GUARANTEE_OR_DIE(succeeded, "Create Input Layout failed.");
}

ID3D11InputLayout* InputLayout::GetDxInputLayout() const {
    return _dx_input_layout;
}
