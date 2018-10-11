#include "Engine/Renderer/Shader.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

#include "Engine/Renderer/BlendState.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Renderer/DepthStencilState.hpp"
#include "Engine/Renderer/RasterState.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"

#include "Engine/RHI/RHIDevice.hpp"

#include <algorithm>
#include <filesystem>
#include <sstream>

Shader::Shader(Renderer* renderer, ShaderProgram* shaderProgram /*= nullptr*/, DepthStencilState* depthStencil /*= nullptr*/, RasterState* rasterState /*= nullptr*/, BlendState* blendState /*= nullptr*/, Sampler* sampler /*= nullptr*/)
: _renderer(renderer)
, _shader_program(shaderProgram)
, _depth_stencil_state(depthStencil)
, _raster_state(rasterState)
, _blend_state(blendState)
, _sampler(sampler)
{
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
    _sampler = nullptr;
    _raster_state = nullptr;
    for(auto& cbuffer : _cbuffers) {
        delete cbuffer;
        cbuffer = nullptr;
    }
    _cbuffers.clear();
    _cbuffers.shrink_to_fit();

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

const std::vector<ConstantBuffer*>& Shader::GetConstantBuffers() const {
    return _cbuffers;
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

void Shader::SetConstantBuffers(const std::vector<ConstantBuffer*> cbuffers) {
    _cbuffers = cbuffers;
}

bool Shader::LoadFromXml(Renderer* renderer, const XMLElement& element) {
    namespace FS = std::filesystem;

    if(renderer == nullptr) {
        return false;
    }

    DataUtils::ValidateXmlElement(element, "shader", "shaderprogram", "name", "depth,stencil,blends,raster,sampler,cbuffers");

    _name = DataUtils::ParseXmlAttribute(element, std::string("name"), _name);

    auto xml_SP = element.FirstChildElement("shaderprogram");
    DataUtils::ValidateXmlElement(*xml_SP, "shaderprogram", "", "src", "pipelinestages");

    std::string sp_src = DataUtils::ParseXmlAttribute(*xml_SP, "src", "");
    if(sp_src.empty()) {
        ERROR_AND_DIE("shaderprogram element has empty src attribute.");
    }

    FS::path p(sp_src);
    p.make_preferred();
    if(nullptr == (_shader_program = _renderer->GetShaderProgram(p.string()))) {
        if(StringUtils::StartsWith(p.string(), "__")) {
            std::ostringstream ss;
            ss << "Intrinsic ShaderProgram referenced in Shader file \"" << _name << "\" does not already exist.";
            ERROR_AND_DIE(ss.str().c_str());
        }
        bool is_hlsl = p.has_extension() && StringUtils::ToLowerCase(p.extension().string()) == ".hlsl";
        if(is_hlsl) {
            if(auto xml_pipelinestages = xml_SP->FirstChildElement("pipelinestages")) {
                
                DataUtils::ValidateXmlElement(*xml_pipelinestages, "pipelinestages", "vertex,hull,domain,geometry,pixel,compute", "");
                PipelineStage targets = PipelineStage::None;
                std::string entrypointList{};

                auto stage_cb = [this, &p, &targets, &entrypointList](const XMLElement& elem) {
                    auto name = std::string{ elem.Name() ? elem.Name() : "" };
                    PipelineStage target = PipelineStage::None;
                    if(name == "vertex") {
                        auto entrypoint = DataUtils::ParseXmlAttribute(elem, "entrypoint", "");
                        target = entrypoint.empty() ? PipelineStage::None : PipelineStage::Vs;
                        entrypoint += ",";
                        entrypointList += entrypoint;
                    } else if(name == "hull") {
                        auto entrypoint = DataUtils::ParseXmlAttribute(elem, "entrypoint", "");
                        target = entrypoint.empty() ? PipelineStage::None : PipelineStage::Hs;
                        entrypoint += ",";
                        entrypointList += entrypoint;
                    } else if(name == "domain") {
                        auto entrypoint = DataUtils::ParseXmlAttribute(elem, "entrypoint", "");
                        target = entrypoint.empty() ? PipelineStage::None : PipelineStage::Ds;
                        entrypoint += ",";
                        entrypointList += entrypoint;
                    } else if(name == "geometry") {
                        auto entrypoint = DataUtils::ParseXmlAttribute(elem, "entrypoint", "");
                        target = entrypoint.empty() ? PipelineStage::None : PipelineStage::Gs;
                        entrypoint += ",";
                        entrypointList += entrypoint;
                    } else if(name == "pixel") {
                        auto entrypoint = DataUtils::ParseXmlAttribute(elem, "entrypoint", "");
                        target = entrypoint.empty() ? PipelineStage::None : PipelineStage::Ps;
                        entrypoint += ",";
                        entrypointList += entrypoint;
                    } else if(name == "compute") {
                        auto entrypoint = DataUtils::ParseXmlAttribute(elem, "entrypoint", "");
                        target = entrypoint.empty() ? PipelineStage::None : PipelineStage::Cs;
                        entrypoint += ",";
                        entrypointList += entrypoint;
                    }
                    targets |= target;
                };
                DataUtils::IterateAllChildElements(*xml_pipelinestages, "", stage_cb);
                _renderer->CreateAndRegisterShaderProgramFromHlslFile(p.string(), entrypointList, targets);
                _shader_program = _renderer->GetShaderProgram(p.string());
            }
        }
    }
    _cbuffers = _renderer->CreateConstantBuffersFromShaderProgram(_shader_program);
    _depth_stencil_state = new DepthStencilState(_renderer->GetDevice(), element);
    _blend_state = new BlendState(_renderer->GetDevice(), element);

    _raster_state = _renderer->GetRasterState("__default");
    if(auto xml_raster = element.FirstChildElement("raster")) {
        std::string rs_src = DataUtils::ParseXmlAttribute(*xml_raster, "src", "");
        if(auto found_raster = _renderer->GetRasterState(rs_src)) {
            _raster_state = found_raster;
        } else {
            CreateAndRegisterNewRasterFromXml(element);
        }
    }

    _sampler = _renderer->GetSampler("__default");
    if(auto xml_sampler = element.FirstChildElement("sampler")) {
        std::string s_src = DataUtils::ParseXmlAttribute(*xml_sampler, "src", "");
        if(auto found_sampler = _renderer->GetSampler(s_src)) {
            _sampler = found_sampler;
        } else {
            CreateAndRegisterNewSamplerFromXml(element);
        }
    }
    return true;
}

void Shader::CreateAndRegisterNewSamplerFromXml(const XMLElement& element) {
    _sampler = new Sampler(_renderer->GetDevice(), element);
    std::string ns = _name + "_sampler";
    _renderer->RegisterSampler(ns, _sampler);
}

void Shader::CreateAndRegisterNewRasterFromXml(const XMLElement& element) {
    _raster_state = new RasterState(_renderer->GetDevice(), element);
    std::string nr = _name + "_raster";
    _renderer->RegisterRasterState(nr, _raster_state);
}
