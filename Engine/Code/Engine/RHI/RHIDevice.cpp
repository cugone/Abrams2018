#include "Engine/RHI/RHIDevice.hpp"

#include "Engine/Core/BuildConfig.cpp"

#include "Engine/Core/EngineBase.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"

#include "Engine/RHI/RHIOutput.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"

#include "Engine/Renderer/InputLayout.hpp"
#include "Engine/Renderer/DepthStencilState.hpp"
#include "Engine/Renderer/Window.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"

#include <sstream>

RHIDevice::RHIDevice()
: _immediate_context(nullptr)
{
    /* DO NOTHING */
}
RHIDevice::~RHIDevice() {
    _immediate_context = nullptr;
    if(_dx_device) {
        _dx_device->Release();
        _dx_device = nullptr;
    }
}

RHIDeviceContext* RHIDevice::GetImmediateContext() const {
    return _immediate_context;
}

RHIOutput* RHIDevice::CreateOutput(Window* window, const RHIOutputMode& outputMode /*= RHIOutputMode::WINDOWED*/) {
    if(!window) {
        ERROR_AND_DIE("RHIDevice: Invalid Window!");
    }
    window->SetDisplayMode(outputMode);
    return CreateOutputFromWindow(window);
}

RHIOutput* RHIDevice::CreateOutput(const IntVector2& clientSize, const IntVector2& clientPosition /*= IntVector2::ZERO*/, const RHIOutputMode& outputMode /*= RHIOutputMode::WINDOWED*/) {
    Window* window = new Window;
    window->SetDimensionsAndPosition(clientPosition, clientSize);
    window->SetDisplayMode(outputMode);
    return CreateOutputFromWindow(window);
}

D3D_FEATURE_LEVEL RHIDevice::GetFeatureLevel() const {
    return _dx_highestSupportedFeatureLevel;
}

ID3D11Device* RHIDevice::GetDxDevice() {
    return _dx_device;
}

bool RHIDevice::IsAllowTearingSupported() const {
    return _allow_tearing_supported;
}

VertexBuffer* RHIDevice::CreateVertexBuffer(const VertexBuffer::buffer_t& vbo, const BufferUsage& usage, const BufferBindUsage& bindusage) {
    auto vb = new VertexBuffer(this, vbo, usage, bindusage);
    return vb;
}

IndexBuffer* RHIDevice::CreateIndexBuffer(const IndexBuffer::buffer_t& ibo, const BufferUsage& usage, const BufferBindUsage& bindusage) {
    auto ib = new IndexBuffer(this, ibo, usage, bindusage);
    return ib;

}

InputLayout* RHIDevice::CreateInputLayout() {
    auto il = new InputLayout(this);
    return il;
}

StructuredBuffer* RHIDevice::CreateStructuredBuffer(const StructuredBuffer::buffer_t& buffer, std::size_t element_size, std::size_t element_count, const BufferUsage& usage, const BufferBindUsage& bindUsage) {
    auto sb = new StructuredBuffer(this, buffer, element_size, element_count, usage, bindUsage);
    return sb;
}

ConstantBuffer* RHIDevice::CreateConstantBuffer(const ConstantBuffer::buffer_t& buffer, std::size_t buffer_size, const BufferUsage& usage, const BufferBindUsage& bindUsage) {
    auto cb = new ConstantBuffer(this, buffer, buffer_size, usage, bindUsage);
    return cb;
}

RHIOutput* RHIDevice::CreateOutputFromWindow(Window*& window) {

    if(window == nullptr) {
        ERROR_AND_DIE("RHIDevice: Invalid Window!");
    }

    window->Open();

    ID3D11Device* dx_device = nullptr;
    ID3D11DeviceContext* dx_context = nullptr;
    D3D_FEATURE_LEVEL supported_feature_level = {};

    unsigned int device_flags = 0U;
#ifdef RENDER_DEBUG
    device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    DXGI_SWAP_CHAIN_DESC swap_chain_desc = {};
    swap_chain_desc.BufferCount = 1;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.OutputWindow = window->GetWindowHandle();
    swap_chain_desc.Windowed = TRUE;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.SampleDesc.Quality = 0;
    swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_desc.BufferDesc.Width = window->GetDimensions().x;
    swap_chain_desc.BufferDesc.Height = window->GetDimensions().y;

    IDXGISwapChain* dxgi_swapChain = nullptr;
    HRESULT hr_swapchain = ::D3D11CreateDeviceAndSwapChain(nullptr
                                                          ,D3D_DRIVER_TYPE_HARDWARE
                                                          , nullptr
                                                          ,device_flags
                                                          ,nullptr
                                                          ,0
                                                          ,D3D11_SDK_VERSION
                                                          ,&swap_chain_desc
                                                          ,&dxgi_swapChain
                                                          ,&dx_device
                                                          ,&supported_feature_level
                                                          ,&dx_context
                                                         );

    _dx_highestSupportedFeatureLevel = supported_feature_level;
    _immediate_context = new RHIDeviceContext(this, dx_context);

    bool swapchain_create_failed = FAILED(hr_swapchain);
    if(swapchain_create_failed) {
        delete _immediate_context;
        _immediate_context = nullptr;
        delete window;
        window = nullptr;
        ERROR_AND_DIE("Could not create swap chain.");
    }
    _dx_device = dx_device;

    RHIOutput* rhi_output = new RHIOutput(this, window, dxgi_swapChain);
    return rhi_output;

}

bool RHIDevice::QueryForAllowTearingSupport(IDXGIFactory5* dxgi_factory) {
    BOOL allow_tearing = {};
    HRESULT hr_cfs = dxgi_factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allow_tearing, sizeof(allow_tearing));
    bool cfs_call_succeeded = SUCCEEDED(hr_cfs);
    if(cfs_call_succeeded) {
        return allow_tearing == TRUE;
    }
    return false;
}

void RHIDevice::GetPrimaryDisplayModeDescriptions(IDXGIAdapter4* dxgi_adapter, std::vector<DXGI_MODE_DESC1>& descriptions) {
    descriptions.clear();
    descriptions.shrink_to_fit();
    IDXGIOutput1* primary_output = nullptr;
    std::vector<IDXGIOutput1*> outputs;
    {
        unsigned int i = 0;
        IDXGIOutput1* cur_output = nullptr;
        while(dxgi_adapter->EnumOutputs(i, (IDXGIOutput**)(&cur_output)) != DXGI_ERROR_NOT_FOUND) {
            outputs.push_back(cur_output);
            ++i;
        }
        primary_output = outputs[0];
    }
    
    unsigned int display_count = 0;
    unsigned int display_mode_flags = DXGI_ENUM_MODES_SCALING | DXGI_ENUM_MODES_INTERLACED | DXGI_ENUM_MODES_STEREO | DXGI_ENUM_MODES_DISABLED_STEREO;
    primary_output->GetDisplayModeList1(DXGI_FORMAT_R8G8B8A8_UNORM, display_mode_flags, &display_count, nullptr);
    if(display_count == 0) {
        return;
    }

    descriptions.resize(display_count);
    primary_output->GetDisplayModeList1(DXGI_FORMAT_R8G8B8A8_UNORM, display_mode_flags, &display_count, descriptions.data());

    for(auto& output : outputs) {
        output->Release();
        output = nullptr;
    }
    outputs.clear();
    outputs.shrink_to_fit();
}

DXGI_MODE_DESC1 RHIDevice::GetDisplayModeMatchingDimensions(const std::vector<DXGI_MODE_DESC1>& descriptions, unsigned int w, unsigned int h) {
    for(const auto& desc : descriptions) {
        if(desc.Width == w && desc.Height == h) {
            return desc;
        }
    }
    return descriptions.back();
}

InputLayout* RHIDevice::CreateInputLayoutFromByteCode(ID3DBlob* bytecode) {
    ID3D11ShaderReflection* vertexReflection = nullptr;
    bool success = SUCCEEDED(::D3DReflect(bytecode->GetBufferPointer(), bytecode->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&vertexReflection));
    if(!success) {
        return nullptr;
    }
    InputLayout* il = new InputLayout(this);
    PopulateInputLayoutUsingReflection(vertexReflection, il);
    il->CreateInputLayout(bytecode->GetBufferPointer(), bytecode->GetBufferSize());
    return il;
}

void RHIDevice::PopulateInputLayoutUsingReflection(ID3D11ShaderReflection* vertexReflection, InputLayout* il) {
    D3D11_SHADER_DESC desc{};
    vertexReflection->GetDesc(&desc);
    unsigned int input_count = desc.InputParameters;
    unsigned int last_input_slot = 16;
    for(auto i = 0u; i < input_count; ++i) {
        D3D11_SIGNATURE_PARAMETER_DESC input_desc{};
        vertexReflection->GetInputParameterDesc(i, &input_desc);
        il->AddElement(CreateInputElementFromSignature(input_desc, last_input_slot));
    }
}

D3D11_INPUT_ELEMENT_DESC RHIDevice::CreateInputElementFromSignature(D3D11_SIGNATURE_PARAMETER_DESC& input_desc, unsigned int& last_input_slot) {
    D3D11_INPUT_ELEMENT_DESC elem{};
    //TODO: Meta file required in the future!
    elem.InputSlot = 0;
    elem.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    elem.InstanceDataStepRate = 0;
    //-----
    elem.SemanticName = input_desc.SemanticName;
    elem.SemanticIndex = input_desc.SemanticIndex;
    elem.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    last_input_slot = elem.InputSlot;

    constexpr auto r_mask = D3D_COMPONENT_MASK_X;
    constexpr auto rg_mask = D3D_COMPONENT_MASK_X | D3D_COMPONENT_MASK_Y;
    constexpr auto rgb_mask = D3D_COMPONENT_MASK_X | D3D_COMPONENT_MASK_Y | D3D_COMPONENT_MASK_Z;
    constexpr auto rgba_mask = D3D_COMPONENT_MASK_X | D3D_COMPONENT_MASK_Y | D3D_COMPONENT_MASK_Z | D3D_COMPONENT_MASK_W;
    auto one_channel_uint = ImageFormatToDxgiFormat(ImageFormat::R32_UInt);
    auto one_channel_sint = ImageFormatToDxgiFormat(ImageFormat::R32_SInt);
    auto one_channel_float = ImageFormatToDxgiFormat(ImageFormat::R32_Float);
    auto two_channel_uint = ImageFormatToDxgiFormat(ImageFormat::R32G32_UInt);
    auto two_channel_sint = ImageFormatToDxgiFormat(ImageFormat::R32G32_SInt);
    auto two_channel_float = ImageFormatToDxgiFormat(ImageFormat::R32G32_Float);
    auto three_channel_uint = ImageFormatToDxgiFormat(ImageFormat::R32G32B32_UInt);
    auto three_channel_sint = ImageFormatToDxgiFormat(ImageFormat::R32G32B32_SInt);
    auto three_channel_float = ImageFormatToDxgiFormat(ImageFormat::R32G32B32_Float);
    auto four_channel_uint = ImageFormatToDxgiFormat(ImageFormat::R32G32B32A32_UInt);
    auto four_channel_sint = ImageFormatToDxgiFormat(ImageFormat::R32G32B32A32_SInt);
    auto four_channel_float = ImageFormatToDxgiFormat(ImageFormat::R32G32B32A32_Float);
    if(input_desc.Mask == r_mask) {
        switch(input_desc.ComponentType) {
            case D3D_REGISTER_COMPONENT_UINT32:  elem.Format = one_channel_uint; break;
            case D3D_REGISTER_COMPONENT_SINT32:  elem.Format = one_channel_sint; break;
            case D3D_REGISTER_COMPONENT_FLOAT32: elem.Format = one_channel_float; break;
        }
    } else if(input_desc.Mask <= rg_mask) {
        switch(input_desc.ComponentType) {
            case D3D_REGISTER_COMPONENT_UINT32:  elem.Format = two_channel_uint; break;
            case D3D_REGISTER_COMPONENT_SINT32:  elem.Format = two_channel_sint; break;
            case D3D_REGISTER_COMPONENT_FLOAT32: elem.Format = two_channel_float; break;
        }
    } else if(input_desc.Mask <= rgb_mask) {
        switch(input_desc.ComponentType) {
            case D3D_REGISTER_COMPONENT_UINT32:  elem.Format = three_channel_uint; break;
            case D3D_REGISTER_COMPONENT_SINT32:  elem.Format = three_channel_sint; break;
            case D3D_REGISTER_COMPONENT_FLOAT32: elem.Format = three_channel_float; break;
        }
    } else if(input_desc.Mask <= rgba_mask) {
        switch(input_desc.ComponentType) {
            case D3D_REGISTER_COMPONENT_UINT32:  elem.Format = four_channel_uint; break;
            case D3D_REGISTER_COMPONENT_SINT32:  elem.Format = four_channel_sint; break;
            case D3D_REGISTER_COMPONENT_FLOAT32: elem.Format = four_channel_float; break;
        }
    }
    return elem;
}

ShaderProgram* RHIDevice::CreateShaderProgramFromHlslString(const std::string& name, const std::string& hlslString, const std::string& entryPointList, InputLayout* inputLayout, const PipelineStage& target) {
    bool uses_vs_stage = static_cast<unsigned char>(target & PipelineStage::Vs) != 0;
    bool uses_hs_stage = static_cast<unsigned char>(target & PipelineStage::Hs) != 0;
    bool uses_ds_stage = static_cast<unsigned char>(target & PipelineStage::Ds) != 0;
    bool uses_gs_stage = static_cast<unsigned char>(target & PipelineStage::Gs) != 0;
    bool uses_ps_stage = static_cast<unsigned char>(target & PipelineStage::Ps) != 0;
    bool uses_cs_stage = static_cast<unsigned char>(target & PipelineStage::Cs) != 0;

    auto entrypoints = StringUtils::Split(entryPointList, ',', false);

    enum class EntrypointIndexes : unsigned char {
        Vs,
        Hs,
        Ds,
        Gs,
        Ps,
        Cs,
    };

    ShaderProgramDesc desc{};
    desc.device = this;
    desc.name = name;
    if(uses_vs_stage) {
        std::string stage = ":VS";
        ID3DBlob * vs_bytecode = CompileShader(name + stage, hlslString.data(), hlslString.size(), entrypoints[static_cast<std::size_t>(EntrypointIndexes::Vs)], PipelineStage::Vs);
        ID3D11VertexShader* vs = nullptr;
        _dx_device->CreateVertexShader(vs_bytecode->GetBufferPointer(), vs_bytecode->GetBufferSize(), nullptr, &vs);
        if(inputLayout) {
            inputLayout->CreateInputLayout(vs_bytecode->GetBufferPointer(), vs_bytecode->GetBufferSize());
        } else {
            inputLayout = CreateInputLayoutFromByteCode(vs_bytecode);
        }
        desc.vs = vs;
        desc.vs_bytecode = vs_bytecode;
        desc.input_layout = inputLayout;
    }

    if(uses_ps_stage) {
        std::string stage = ":PS";
        ID3DBlob* ps_bytecode = CompileShader(name + stage, hlslString.data(), hlslString.size(), entrypoints[static_cast<std::size_t>(EntrypointIndexes::Ps)], PipelineStage::Ps);
        ID3D11PixelShader* ps = nullptr;
        _dx_device->CreatePixelShader(ps_bytecode->GetBufferPointer(), ps_bytecode->GetBufferSize(), nullptr, &ps);
        desc.ps = ps;
        desc.ps_bytecode = ps_bytecode;
    }

    if(uses_hs_stage) {
        std::string stage = ":HS";
        ID3DBlob* hs_bytecode = CompileShader(name + stage, hlslString.data(), hlslString.size(), entrypoints[static_cast<std::size_t>(EntrypointIndexes::Hs)], PipelineStage::Hs);
        ID3D11HullShader* hs = nullptr;
        _dx_device->CreateHullShader(hs_bytecode->GetBufferPointer(), hs_bytecode->GetBufferSize(), nullptr, &hs);
        desc.hs = hs;
        desc.hs_bytecode = hs_bytecode;
    }

    if(uses_ds_stage) {
        std::string stage = ":DS";
        ID3DBlob* ds_bytecode = CompileShader(name + stage, hlslString.data(), hlslString.size(), entrypoints[static_cast<std::size_t>(EntrypointIndexes::Ds)], PipelineStage::Ds);
        ID3D11DomainShader* ds = nullptr;
        _dx_device->CreateDomainShader(ds_bytecode->GetBufferPointer(), ds_bytecode->GetBufferSize(), nullptr, &ds);
    }

    if(uses_gs_stage) {
        std::string stage = ":GS";
        ID3DBlob* gs_bytecode = CompileShader(name + stage, hlslString.data(), hlslString.size(), entrypoints[static_cast<std::size_t>(EntrypointIndexes::Gs)], PipelineStage::Gs);
        ID3D11GeometryShader* gs = nullptr;
        _dx_device->CreateGeometryShader(gs_bytecode->GetBufferPointer(), gs_bytecode->GetBufferSize(), nullptr, &gs);
    }

    if(uses_cs_stage) {
        std::string stage = ":CS";
        ID3DBlob* cs_bytecode = CompileShader(name + stage, hlslString.data(), hlslString.size(), entrypoints[static_cast<std::size_t>(EntrypointIndexes::Cs)], PipelineStage::Cs);
        ID3D11ComputeShader* cs = nullptr;
        _dx_device->CreateComputeShader(cs_bytecode->GetBufferPointer(), cs_bytecode->GetBufferSize(), nullptr, &cs);
    }
    ShaderProgram* new_sp = new ShaderProgram(std::move(desc));
    return new_sp;
}

ShaderProgram* RHIDevice::CreateShaderProgramFromHlslFile(const std::string& filepath, const std::string& entryPoint, const PipelineStage& target) {
    bool retry_requested = false;
    do {
        std::string source{};
        if(FileUtils::ReadBufferFromFile(source, filepath)) {
            ShaderProgram* sp = CreateShaderProgramFromHlslString(filepath, source, entryPoint, nullptr, target);
            if(sp) {
                return sp;
            }
            std::ostringstream ss;
            ss << "Shader program " << filepath << " failed to compile.\nSee Output window for errors.\nPress Retry to recompile.";
            retry_requested = IDRETRY == ::MessageBoxA(nullptr, ss.str().c_str(), "ShaderProgram Compiler Error", MB_ICONERROR | MB_RETRYCANCEL);
        }
    } while(retry_requested);
    ERROR_AND_DIE("Unrecoverable error. Cannot continue with malformed shader file.");
}

ID3DBlob* RHIDevice::CompileShader(const std::string& name, const void*  sourceCode, std::size_t sourceCodeSize, const std::string& entryPoint, const PipelineStage& target) {
    unsigned int compile_options = 0;
#ifdef RENDER_DEBUG
    compile_options |= D3DCOMPILE_DEBUG;
    compile_options |= D3DCOMPILE_SKIP_OPTIMIZATION;
    compile_options |= D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;
    compile_options |= D3DCOMPILE_WARNINGS_ARE_ERRORS;
#endif
#ifdef FINAL_BUILD
    compile_options |= D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;
    compile_options |= D3DCOMPILE_SKIP_VALIDATION;
    compile_options |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif
    ID3DBlob* code_blob = nullptr;
    ID3DBlob* errors = nullptr;
    std::string target_string = {};
    switch(target) {
        case PipelineStage::Vs:
            target_string = "vs_5_0";
            break;
        case PipelineStage::Hs:
            target_string = "hs_5_0";
            break;
        case PipelineStage::Ds:
            target_string = "ds_5_0";
            break;
        case PipelineStage::Gs:
            target_string = "gs_5_0";
            break;
        case PipelineStage::Ps:
            target_string = "ps_5_0";
            break;
        case PipelineStage::Cs:
            target_string = "cs_5_0";
            break;
        case PipelineStage::None:
        case PipelineStage::All:
        default:
            DebuggerPrintf("Failed to compile [%s]. Invalid PipelineStage parameter.\n", name.c_str());
            return nullptr;
    }
    HRESULT compile_hr = ::D3DCompile(  sourceCode
                                      , sourceCodeSize
                                      , name.c_str()
                                      , nullptr
                                      , D3D_COMPILE_STANDARD_FILE_INCLUDE
                                      , entryPoint.c_str()
                                      , target_string.c_str()
                                      , compile_options
                                      , 0
                                      , &code_blob
                                      , &errors);
    if(FAILED(compile_hr) || (errors != nullptr)) {
        if(errors != nullptr) {
            char* error_string = reinterpret_cast<char*>(errors->GetBufferPointer());
            DebuggerPrintf("Failed to compile [%s].  Compiler gave the following output;\n%s",
                           name.c_str(),
                           error_string);
            errors->Release();
            errors = nullptr;
        }
    }
    return code_blob;
}
