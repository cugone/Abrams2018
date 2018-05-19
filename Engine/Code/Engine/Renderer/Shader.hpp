#pragma once

#include "Engine/Core/DataUtils.hpp"

#include <string>

class ShaderProgram;
class RasterState;
class DepthStencilState;
class BlendState;
class Renderer;
class Sampler;

class Shader {
public:
    Shader(Renderer* renderer, ShaderProgram* shaderProgram = nullptr, DepthStencilState* depthStencil = nullptr, RasterState* rasterState = nullptr, BlendState* blendState = nullptr);
    Shader(Renderer* renderer, const XMLElement& element);
    ~Shader();

    const std::string& GetName() const;
    ShaderProgram* GetShaderProgram() const;
    RasterState* GetRasterState() const;
    DepthStencilState* GetDepthStencilState() const;
    BlendState* GetBlendState() const;
    Sampler* GetSampler() const;

    void SetName(const std::string& name);
    void SetShaderProgram(ShaderProgram* sp);
    void SetRasterState(RasterState* rs);
    void SetDepthStencilState(DepthStencilState* ds);
    void SetBlendState(BlendState* bs);
    void SetSampler(Sampler* sampler);

protected:
private:
    bool LoadFromXml(Renderer* renderer, const XMLElement& element);
    std::string _name = "SHADER";
    Renderer* _renderer = nullptr;
    ShaderProgram* _shader_program = nullptr;
    DepthStencilState* _depth_stencil_state = nullptr;
    RasterState* _raster_state = nullptr;
    BlendState* _blend_state = nullptr;
    Sampler* _sampler = nullptr;
};