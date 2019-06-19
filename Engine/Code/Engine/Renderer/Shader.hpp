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

enum class PipelineStage : uint8_t;

class Shader {
public:
    Shader(Renderer* renderer, ShaderProgram* shaderProgram = nullptr, DepthStencilState* depthStencil = nullptr, RasterState* rasterState = nullptr, BlendState* blendState = nullptr, Sampler* sampler = nullptr) noexcept;
    Shader(Renderer* renderer, const XMLElement& element) noexcept;
    ~Shader() noexcept;

    const std::string& GetName() const noexcept;
    ShaderProgram* GetShaderProgram() const noexcept;
    RasterState* GetRasterState() const noexcept;
    DepthStencilState* GetDepthStencilState() const noexcept;
    BlendState* GetBlendState() const noexcept;
    Sampler* GetSampler() const noexcept;
    const std::vector<ConstantBuffer*>& GetConstantBuffers() const noexcept;

    void SetName(const std::string& name) noexcept;
    void SetShaderProgram(ShaderProgram* sp) noexcept;
    void SetRasterState(RasterState* rs) noexcept;
    void SetDepthStencilState(DepthStencilState* ds) noexcept;
    void SetBlendState(BlendState* bs) noexcept;
    void SetSampler(Sampler* sampler) noexcept;
    void SetConstantBuffers(const std::vector<ConstantBuffer*>& cbuffers) noexcept;
protected:
private:
    bool LoadFromXml(Renderer* renderer, const XMLElement& element) noexcept;

    PipelineStage ParseTargets(const XMLElement& element) noexcept;
    std::string ParseEntrypointList(const XMLElement& element) noexcept;

    void ValidatePipelineStages(const PipelineStage& targets) noexcept;

    void CreateAndRegisterNewSamplerFromXml(const XMLElement& element) noexcept;
    void CreateAndRegisterNewRasterFromXml(const XMLElement& element) noexcept;

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