#pragma once

#include <vector>

#include "Engine/Core/Vertex3D.hpp"

#include "Engine/Renderer/DirectX/DX11.hpp"

class Texture;
class Rgba;
class RHIDevice;
class VertexBuffer;
class IndexBuffer;
class StructuredBuffer;
class ConstantBuffer;
class Sampler;
class Shader;
class ShaderProgram;
class DepthStencilState;
class RasterState;
class BlendState;
class Material;

enum class BufferType : unsigned int;
enum class PrimitiveType : unsigned int;

class RHIDeviceContext {
public:
    RHIDeviceContext(const RHIDevice* parentDevice, ID3D11DeviceContext* deviceContext);
    ~RHIDeviceContext();

    void ClearState();
    void Flush();

    void ClearColorTarget(Texture* output, const Rgba& color);
    void ClearDepthStencilTarget(Texture* output, bool depth = true, bool stencil = true, float depthValue = 1.0f, unsigned char stencilValue = 0);

    void SetMaterial(Material* material);
    void SetTexture(unsigned int index, Texture* texture);
    void SetVertexBuffer(unsigned int startIndex, VertexBuffer* buffer);
    void SetIndexBuffer(IndexBuffer* buffer);
    void SetConstantBuffer(unsigned int index, ConstantBuffer* buffer);
    void SetStructuredBuffer(unsigned int index, StructuredBuffer* buffer);

    void Draw(std::size_t vertexCount, std::size_t startVertex = 0);
    void DrawIndexed(std::size_t vertexCount, std::size_t startVertex = 0, std::size_t baseVertexLocation = 0);

    const RHIDevice* GetParentDevice() const;
    ID3D11DeviceContext* GetDxContext();

    void UnbindAllShaderResources();
    void UnbindAllCustomConstantBuffers();

private:
    void UnbindAllConstantBuffers();

    void SetShader(Shader* shader);
    void SetShaderProgram(ShaderProgram* shaderProgram = nullptr);
    void SetDepthStencilState(DepthStencilState* depthStencilState = nullptr);
    void SetRasterState(RasterState* rasterState = nullptr);
    void SetBlendState(BlendState* blendState = nullptr);
    void SetSampler(Sampler* sampler = nullptr);

    static constexpr unsigned int STRUCTURED_BUFFER_OFFSET = 64;

    const RHIDevice* _device = nullptr;
    ID3D11DeviceContext* _dx_context = nullptr;

    friend class Renderer;
};
