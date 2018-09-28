#pragma once

#include "Engine/Core/DataUtils.hpp"

#include <string>

class ShaderProgram;
class RasterState;
class DepthStencilState;
class BlendState;
class Renderer;
class Sampler;
class ConstantBuffer;

class Shader {
public:
    Shader(Renderer* renderer, ShaderProgram* shaderProgram = nullptr, DepthStencilState* depthStencil = nullptr, RasterState* rasterState = nullptr, BlendState* blendState = nullptr, Sampler* sampler = nullptr);
    Shader(Renderer* renderer, const XMLElement& element);
    ~Shader();

    const std::string& GetName() const;
    ShaderProgram* GetShaderProgram() const;
    RasterState* GetRasterState() const;
    DepthStencilState* GetDepthStencilState() const;
    BlendState* GetBlendState() const;
    Sampler* GetSampler() const;
    const std::vector<ConstantBuffer*>& GetConstantBuffers() const;

    void SetName(const std::string& name);
    void SetShaderProgram(ShaderProgram* sp);
    void SetRasterState(RasterState* rs);
    void SetDepthStencilState(DepthStencilState* ds);
    void SetBlendState(BlendState* bs);
    void SetSampler(Sampler* sampler);
    void SetConstantBuffers(const std::vector<ConstantBuffer*> cbuffers);
protected:
private:
    bool LoadFromXml(Renderer* renderer, const XMLElement& element);

    void CreateAndRegisterNewSamplerFromXml(const XMLElement& element);
    void CreateAndRegisterNewRasterFromXml(const XMLElement& element);
    void CreateConstantBuffersFromXml(const XMLElement& element);

    void BuildBufferFromXml(const XMLElement& element, std::vector<void *>& b_t);

    void AppendScalarToConstantBufferBuffer(std::vector<void *>& b_t);
    void AppendUnsignedScalarToConstantBufferBuffer(std::vector<void *>& b_t);

    void AppendVector1ToConstantBufferBuffer(std::vector<void *>& b_t);
    void AppendVector2ToConstantBufferBuffer(std::vector<void *>& b_t);
    void AppendVector3ToConstantBufferBuffer(std::vector<void *>& b_t);
    void AppendVector4ToConstantBufferBuffer(std::vector<void *>& b_t);
    void AppendMatrix1ToConstantBufferBuffer(std::vector<void *>& b_t);
    void AppendMatrix2ToConstantBufferBuffer(std::vector<void *>& b_t);
    void AppendMatrix3ToConstantBufferBuffer(std::vector<void *>& b_t);
    void AppendMatrix4ToConstantBufferBuffer(std::vector<void *>& b_t);

    void AppendUnsignedVector1ToConstantBufferBuffer(std::vector<void *>& b_t);
    void AppendUnsignedVector2ToConstantBufferBuffer(std::vector<void *>& b_t);
    void AppendUnsignedVector3ToConstantBufferBuffer(std::vector<void *>& b_t);
    void AppendUnsignedVector4ToConstantBufferBuffer(std::vector<void *>& b_t);
    void AppendUnsignedMatrix1ToConstantBufferBuffer(std::vector<void *>& b_t);
    void AppendUnsignedMatrix2ToConstantBufferBuffer(std::vector<void *>& b_t);
    void AppendUnsignedMatrix3ToConstantBufferBuffer(std::vector<void *>& b_t);
    void AppendUnsignedMatrix4ToConstantBufferBuffer(std::vector<void *>& b_t);

    std::string _name = "SHADER";
    Renderer* _renderer = nullptr;
    ShaderProgram* _shader_program = nullptr;
    DepthStencilState* _depth_stencil_state = nullptr;
    RasterState* _raster_state = nullptr;
    BlendState* _blend_state = nullptr;
    Sampler* _sampler = nullptr;
    std::vector<ConstantBuffer*> _cbuffers{};
    bool _raster_from_db = false;
    bool _sampler_from_db = false;
};