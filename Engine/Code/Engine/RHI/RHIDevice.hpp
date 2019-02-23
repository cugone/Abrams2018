#pragma once

#include "Engine/RHI/RHITypes.hpp"

#include "Engine/Math/IntVector2.hpp"

#include "Engine/Renderer/DirectX/DX11.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/StructuredBuffer.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"

#include <set>
#include <vector>

class RHIDeviceContext;
class IntVector2;
class Window;
class RHIOutput;
class DepthStencilState;
class InputLayout;
class Vertex3D;
class ShaderProgram;

class RHIDevice {
public:
    RHIDevice() = default;
    ~RHIDevice();

    RHIDeviceContext* GetImmediateContext() const;

    RHIOutput* CreateOutput(Window* window, const RHIOutputMode& mode = RHIOutputMode::Windowed);
    RHIOutput* CreateOutput(const IntVector2& clientSize, const IntVector2& clientPosition = IntVector2::ZERO, const RHIOutputMode& outputMode = RHIOutputMode::Windowed);

    VertexBuffer* CreateVertexBuffer(const VertexBuffer::buffer_t& vbo, const BufferUsage& usage, const BufferBindUsage& bindusage) const;
    IndexBuffer* CreateIndexBuffer(const IndexBuffer::buffer_t& ibo, const BufferUsage& usage, const BufferBindUsage& bindusage) const;
    InputLayout* CreateInputLayout() const;

    StructuredBuffer* CreateStructuredBuffer(const StructuredBuffer::buffer_t& buffer, std::size_t element_size, std::size_t element_count, const BufferUsage& usage, const BufferBindUsage& bindUsage) const;
    ConstantBuffer* CreateConstantBuffer(const ConstantBuffer::buffer_t& buffer, std::size_t buffer_size, const BufferUsage& usage, const BufferBindUsage& bindUsage) const;


    D3D_FEATURE_LEVEL GetFeatureLevel() const;
    ID3D11Device5* GetDxDevice() const;
    bool IsAllowTearingSupported() const;

    ShaderProgram* CreateShaderProgramFromHlslString(const std::string& name, const std::string& hlslString, const std::string& entryPoint, InputLayout* inputLayout, const PipelineStage& target) const;
    ShaderProgram* CreateShaderProgramFromHlslFile(const std::string& filepath, const std::string& entryPoint, const PipelineStage& target) const;

    ID3DBlob* CompileShader(const std::string& name, const void*  sourceCode, std::size_t sourceCodeSize, const std::string& entryPoint, const PipelineStage& target) const;
    std::vector<ConstantBuffer*> CreateConstantBuffersFromByteCode(ID3DBlob* bytecode) const;

    std::set<DisplayDesc, DisplayDescGTComparator> displayModes{};

private:
    RHIOutput* CreateOutputFromWindow(Window*& window);

    std::vector<ConstantBuffer*> CreateConstantBuffersUsingReflection(ID3D11ShaderReflection& cbufferReflection) const;
    InputLayout* CreateInputLayoutFromByteCode(ID3DBlob* bytecode) const;

    std::vector<OutputInfo> GetOutputsFromAdapter(const AdapterInfo& a) const noexcept;
    void GetPrimaryDisplayModeDescriptions(const AdapterInfo& adapter, decltype(displayModes)& descriptions) const;
    void GetDisplayModeDescriptions(const AdapterInfo& adapter, const OutputInfo& output, decltype(displayModes)& descriptions) const;
    DisplayDesc GetDisplayModeMatchingDimensions(const std::vector<DisplayDesc>& descriptions, unsigned int w, unsigned int h);

    RHIDeviceContext* _immediate_context = nullptr;
    ID3D11Device5* _dx_device = nullptr;
    D3D_FEATURE_LEVEL _dx_highestSupportedFeatureLevel{};
    bool _allow_tearing_supported = false;
};