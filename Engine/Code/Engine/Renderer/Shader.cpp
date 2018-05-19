#include "Engine/Renderer/Shader.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Renderer/BlendState.hpp"
#include "Engine/Renderer/DepthStencilState.hpp"
#include "Engine/Renderer/RasterState.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Sampler.hpp"

#include <filesystem>
#include <sstream>

Shader::Shader(Renderer* renderer, ShaderProgram* shaderProgram /*= nullptr*/, DepthStencilState* depthStencil /*= nullptr*/, RasterState* rasterState /*= nullptr*/, BlendState* blendState /*= nullptr*/)
    : _renderer(renderer)
    , _shader_program(shaderProgram)
    , _depth_stencil_state(depthStencil)
    , _raster_state(rasterState)
    , _blend_state(blendState) {
    std::size_t count = renderer->GetShaderCount();
    std::ostringstream ss;
    ss << '_' << count;
    _name += ss.str();
}

Shader::Shader(Renderer* renderer, const XMLElement& element)
    : _renderer(renderer)
{
    std::size_t count = renderer->GetShaderCount();
    std::ostringstream ss;
    ss << '_' << count;
    _name += ss.str();

    LoadFromXml(renderer, element);

}

Shader::~Shader() {
    _renderer = nullptr;
    if(_sampler) {
        delete _sampler;
        _sampler = nullptr;
    }

    if(_raster_state) {
        delete _raster_state;
        _raster_state = nullptr;
    }

    if(_blend_state) {
        delete _blend_state;
        _blend_state = nullptr;
    }

    if(_depth_stencil_state) {
        delete _depth_stencil_state;
        _depth_stencil_state = nullptr;
    }
}

const std::string& Shader::GetName() const {
    return _name;
}

ShaderProgram* Shader::GetShaderProgram() const {
    return _shader_program;
}

RasterState* Shader::GetRasterState() const {
    return _raster_state;
}

DepthStencilState* Shader::GetDepthStencilState() const {
    return _depth_stencil_state;
}

BlendState* Shader::GetBlendState() const {
    return _blend_state;
}

Sampler* Shader::GetSampler() const {
    return _sampler;
}

void Shader::SetName(const std::string& name) {
    _name = name;
}

void Shader::SetShaderProgram(ShaderProgram* sp) {
    _shader_program = sp;
}

void Shader::SetRasterState(RasterState* rs) {
    _raster_state = rs;
}

void Shader::SetDepthStencilState(DepthStencilState* ds) {
    _depth_stencil_state = ds;
}

void Shader::SetBlendState(BlendState* bs) {
    _blend_state = bs;
}

void Shader::SetSampler(Sampler* sampler) {
    _sampler = sampler;
}

bool Shader::LoadFromXml(Renderer* renderer, const XMLElement& element) {
    if(renderer == nullptr) {
        return false;
    }

    namespace FS = std::experimental::filesystem;

    DataUtils::ValidateXmlElement(element, "shader", "shaderprogram", "name", "depth,stencil,blends,raster,sampler");

    _name = DataUtils::ParseXmlAttribute(element, std::string("name"), "UNNAMED_SHADER");

    auto xml_SP = element.FirstChildElement("shaderprogram");
    DataUtils::ValidateXmlElement(*xml_SP, "shaderprogram", "", "src");

    std::string sp_src = DataUtils::ParseXmlAttribute(*xml_SP, "src", "");
    if(sp_src.empty()) {
        ERROR_AND_DIE("shaderprogram element has empty src attribute.");
    }

    FS::path p(sp_src);
    auto program = _renderer->GetShaderProgram(p.string());
    if(program == nullptr) {
        ERROR_AND_DIE("ShaderProgram referenced in Shader file does not already exist.");
    }
    _shader_program = program;

    _depth_stencil_state = new DepthStencilState(_renderer->GetDevice(), element);

    _blend_state = new BlendState(_renderer->GetDevice(), element);

    _raster_state = new RasterState(_renderer->GetDevice(), element);

    _sampler = new Sampler(_renderer->GetDevice(), element);

    return true;

}
