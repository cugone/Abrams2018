#include "Engine/Renderer/Shader.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Renderer/BlendState.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Renderer/DepthStencilState.hpp"
#include "Engine/Renderer/RasterState.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Sampler.hpp"

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

const std::vector<ConstantBuffer*>& Shader::GetConstantBuffers() const {
    return _cbuffers;
}

void Shader::SetConstantBuffers(const std::vector<ConstantBuffer*> cbuffers) {
    for(auto& b : _cbuffers) {
        delete b;
        b = nullptr;
    }
    _cbuffers.clear();
    for(auto& b : cbuffers) {
        _cbuffers.push_back(b);
    }
}

void Shader::AppendScalarToConstantBufferBuffer(std::vector<void *>& b_t) {
    b_t.push_back(int32_t{});
}

void Shader::AppendUnsignedScalarToConstantBufferBuffer(std::vector<void *>& b_t) {
    b_t.push_back(uint32_t{});
}

void Shader::AppendVector1ToConstantBufferBuffer(std::vector<void *>& b_t) {
    AppendScalarToConstantBufferBuffer(b_t);
}

void Shader::AppendVector2ToConstantBufferBuffer(std::vector<void *>& b_t) {
    AppendScalarToConstantBufferBuffer(b_t);
    AppendScalarToConstantBufferBuffer(b_t);
}

void Shader::AppendVector3ToConstantBufferBuffer(std::vector<void *>& b_t) {
    AppendScalarToConstantBufferBuffer(b_t);
    AppendScalarToConstantBufferBuffer(b_t);
    AppendScalarToConstantBufferBuffer(b_t);
}

void Shader::AppendVector4ToConstantBufferBuffer(std::vector<void *>& b_t) {
    AppendScalarToConstantBufferBuffer(b_t);
    AppendScalarToConstantBufferBuffer(b_t);
    AppendScalarToConstantBufferBuffer(b_t);
    AppendScalarToConstantBufferBuffer(b_t);
}

void Shader::AppendUnsignedVector1ToConstantBufferBuffer(std::vector<void *>& b_t) {
    AppendUnsignedScalarToConstantBufferBuffer(b_t);
}

void Shader::AppendUnsignedVector2ToConstantBufferBuffer(std::vector<void *>& b_t) {
    AppendUnsignedScalarToConstantBufferBuffer(b_t);
    AppendUnsignedScalarToConstantBufferBuffer(b_t);
}

void Shader::AppendUnsignedVector3ToConstantBufferBuffer(std::vector<void *>& b_t) {
    AppendUnsignedScalarToConstantBufferBuffer(b_t);
    AppendUnsignedScalarToConstantBufferBuffer(b_t);
    AppendUnsignedScalarToConstantBufferBuffer(b_t);
}

void Shader::AppendUnsignedVector4ToConstantBufferBuffer(std::vector<void *>& b_t) {
    AppendUnsignedScalarToConstantBufferBuffer(b_t);
    AppendUnsignedScalarToConstantBufferBuffer(b_t);
    AppendUnsignedScalarToConstantBufferBuffer(b_t);
    AppendUnsignedScalarToConstantBufferBuffer(b_t);
}

void Shader::AppendMatrix1ToConstantBufferBuffer(std::vector<void *>& b_t) {
    AppendVector1ToConstantBufferBuffer(b_t);
}

void Shader::AppendMatrix2ToConstantBufferBuffer(std::vector<void *>& b_t) {
    AppendVector2ToConstantBufferBuffer(b_t);
    AppendVector2ToConstantBufferBuffer(b_t);
}

void Shader::AppendMatrix3ToConstantBufferBuffer(std::vector<void *>& b_t) {
    AppendVector3ToConstantBufferBuffer(b_t);
    AppendVector3ToConstantBufferBuffer(b_t);
    AppendVector3ToConstantBufferBuffer(b_t);
}

void Shader::AppendMatrix4ToConstantBufferBuffer(std::vector<void *>& b_t) {
    AppendVector4ToConstantBufferBuffer(b_t);
    AppendVector4ToConstantBufferBuffer(b_t);
    AppendVector4ToConstantBufferBuffer(b_t);
    AppendVector4ToConstantBufferBuffer(b_t);
}

void Shader::AppendUnsignedMatrix1ToConstantBufferBuffer(std::vector<void *>& b_t) {
    AppendUnsignedVector1ToConstantBufferBuffer(b_t);
}

void Shader::AppendUnsignedMatrix2ToConstantBufferBuffer(std::vector<void *>& b_t) {
    AppendUnsignedVector2ToConstantBufferBuffer(b_t);
    AppendUnsignedVector2ToConstantBufferBuffer(b_t);
}

void Shader::AppendUnsignedMatrix3ToConstantBufferBuffer(std::vector<void *>& b_t) {
    AppendUnsignedVector3ToConstantBufferBuffer(b_t);
    AppendUnsignedVector3ToConstantBufferBuffer(b_t);
    AppendUnsignedVector3ToConstantBufferBuffer(b_t);
}

void Shader::AppendUnsignedMatrix4ToConstantBufferBuffer(std::vector<void *>& b_t) {
    AppendUnsignedVector4ToConstantBufferBuffer(b_t);
    AppendUnsignedVector4ToConstantBufferBuffer(b_t);
    AppendUnsignedVector4ToConstantBufferBuffer(b_t);
    AppendUnsignedVector4ToConstantBufferBuffer(b_t);
}

bool Shader::LoadFromXml(Renderer* renderer, const XMLElement& element) {
    namespace FS = std::filesystem;

    if(renderer == nullptr) {
        return false;
    }

    DataUtils::ValidateXmlElement(element, "shader", "shaderprogram", "name", "depth,stencil,blends,raster,sampler,cbuffers");

    _name = DataUtils::ParseXmlAttribute(element, std::string("name"), _name);

    auto xml_SP = element.FirstChildElement("shaderprogram");
    DataUtils::ValidateXmlElement(*xml_SP, "shaderprogram", "", "src");

    std::string sp_src = DataUtils::ParseXmlAttribute(*xml_SP, "src", "");
    if(sp_src.empty()) {
        ERROR_AND_DIE("shaderprogram element has empty src attribute.");
    }

    FS::path p(sp_src);
    p.make_preferred();
    if((_shader_program = _renderer->GetShaderProgram(p.string())) == nullptr) {
        ERROR_AND_DIE("ShaderProgram referenced in Shader file does not already exist.");
    }
    _depth_stencil_state = new DepthStencilState(_renderer->GetDevice(), element);
    _blend_state = new BlendState(_renderer->GetDevice(), element);

    if(auto xml_raster = element.FirstChildElement("raster")) {
        std::string rs_src = DataUtils::ParseXmlAttribute(*xml_raster, "src", "");
        if(auto found_raster = _renderer->GetRasterState(rs_src)) {
            _raster_state = found_raster;
        } else {
            CreateAndRegisterNewRasterFromXml(element);
        }
    } else {
        CreateAndRegisterNewRasterFromXml(element);
    }
    if(auto xml_sampler = element.FirstChildElement("sampler")) {
        std::string s_src = DataUtils::ParseXmlAttribute(*xml_sampler, "src", "");
        auto found_sampler = _renderer->GetSampler(s_src);
        if(!found_sampler) {
            CreateAndRegisterNewSamplerFromXml(element);
        } else {
            _sampler = found_sampler;
        }
    } else {
        CreateAndRegisterNewSamplerFromXml(element);
    }
    if(auto xml_cbuffers = element.FirstChildElement("cbuffers")) {
        CreateConstantBuffersFromXml(element);
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

void Shader::CreateConstantBuffersFromXml(const XMLElement& element) {
    std::string none{};
    std::string optCE{"int,uint,dword,float,"
                      "int1,int2,int3,int4,"
                      "uint1,uint2,uint3,uint4,"
                      "dword1,dword2,dword3,dword4,"
                      "float1,float2,float3,float4,"
                      "int1x1,int2x2,int3x3,int4x4,"
                      "uint1x1,uint2x2,uint3x3,uint4x4,"
                      "dword1x1,dword2x2,dword3x3,dword4x4,"
                      "float1x1,float2x2,float3x3,float4x4,"};

    auto cb = [this, &none, &optCE](const XMLElement& element) {
        DataUtils::ValidateXmlElement(element, "cbuffer", none, none, optCE, none);
        std::vector<void*> b_t{};
        BuildBufferFromXml(element, b_t);
        ConstantBuffer::buffer_t buffer = b_t.data();
        auto buffer_size = sizeof(buffer);
        _cbuffers.push_back(_renderer->CreateConstantBuffer(buffer, buffer_size));
    };
    DataUtils::IterateAllChildElements(element, "cbuffer", cb);
}

void Shader::BuildBufferFromXml(const XMLElement& element, std::vector<void *>& b_t) {
    {
        auto signed_scalar_count = 0u;
        signed_scalar_count += DataUtils::GetChildElementCount(element, "int");
        signed_scalar_count += DataUtils::GetChildElementCount(element, "float");
        for(unsigned int i = 0; i < signed_scalar_count; ++i) {
            AppendScalarToConstantBufferBuffer(b_t);
        }
    }
    {
        auto unsigned_scalar_count = 0u;
        unsigned_scalar_count += DataUtils::GetChildElementCount(element, "uint");
        unsigned_scalar_count += DataUtils::GetChildElementCount(element, "dword");
        for(unsigned int i = 0; i < unsigned_scalar_count; ++i) {
            AppendUnsignedScalarToConstantBufferBuffer(b_t);
        }
    }
    {
        auto signed_vector1_count = 0u;
        signed_vector1_count += DataUtils::GetChildElementCount(element, "int1");
        signed_vector1_count += DataUtils::GetChildElementCount(element, "float1");
        for(unsigned int i = 0; i < signed_vector1_count; ++i) {
            AppendVector1ToConstantBufferBuffer(b_t);
        }
    }
    {
        auto unsigned_vector1_count = 0u;
        unsigned_vector1_count += DataUtils::GetChildElementCount(element, "uint1");
        unsigned_vector1_count += DataUtils::GetChildElementCount(element, "dword1");
        for(unsigned int i = 0; i < unsigned_vector1_count; ++i) {
            AppendUnsignedVector1ToConstantBufferBuffer(b_t);
        }
    }
    {
        auto signed_vector2_count = 0u;
        signed_vector2_count += DataUtils::GetChildElementCount(element, "int2");
        signed_vector2_count += DataUtils::GetChildElementCount(element, "float2");
        for(unsigned int i = 0; i < signed_vector2_count; ++i) {
            AppendVector2ToConstantBufferBuffer(b_t);
        }
    }
    {
        auto unsigned_vector2_count = 0u;
        unsigned_vector2_count += DataUtils::GetChildElementCount(element, "uint2");
        unsigned_vector2_count += DataUtils::GetChildElementCount(element, "dword2");
        for(unsigned int i = 0; i < unsigned_vector2_count; ++i) {
            AppendUnsignedVector2ToConstantBufferBuffer(b_t);
        }
    }
    {
        auto signed_vector3_count = 0u;
        signed_vector3_count += DataUtils::GetChildElementCount(element, "int3");
        signed_vector3_count += DataUtils::GetChildElementCount(element, "float3");
        for(unsigned int i = 0; i < signed_vector3_count; ++i) {
            AppendVector3ToConstantBufferBuffer(b_t);
        }
    }
    {
        auto unsigned_vector3_count = 0u;
        unsigned_vector3_count += DataUtils::GetChildElementCount(element, "uint3");
        unsigned_vector3_count += DataUtils::GetChildElementCount(element, "dword3");
        for(unsigned int i = 0; i < unsigned_vector3_count; ++i) {
            AppendUnsignedVector3ToConstantBufferBuffer(b_t);
        }
    }
    {
        auto signed_vector4_count = 0u;
        signed_vector4_count += DataUtils::GetChildElementCount(element, "int4");
        signed_vector4_count += DataUtils::GetChildElementCount(element, "float4");
        for(unsigned int i = 0; i < signed_vector4_count; ++i) {
            AppendVector4ToConstantBufferBuffer(b_t);
        }
    }
    {
        auto unsigned_vector4_count = 0u;
        unsigned_vector4_count += DataUtils::GetChildElementCount(element, "uint4");
        unsigned_vector4_count += DataUtils::GetChildElementCount(element, "dword4");
        for(unsigned int i = 0; i < unsigned_vector4_count; ++i) {
            AppendUnsignedVector4ToConstantBufferBuffer(b_t);
        }
    }
    {
        auto signed_matrix1_count = 0u;
        signed_matrix1_count += DataUtils::GetChildElementCount(element, "int1x1");
        signed_matrix1_count += DataUtils::GetChildElementCount(element, "float1x1");
        for(unsigned int i = 0; i < signed_matrix1_count; ++i) {
            AppendMatrix1ToConstantBufferBuffer(b_t);
        }
    }
    {
        auto unsigned_matrix1_count = 0u;
        unsigned_matrix1_count += DataUtils::GetChildElementCount(element, "uint1x1");
        unsigned_matrix1_count += DataUtils::GetChildElementCount(element, "dword1x1");
        for(unsigned int i = 0; i < unsigned_matrix1_count; ++i) {
            AppendUnsignedMatrix1ToConstantBufferBuffer(b_t);
        }
    }
    {
        auto signed_matrix2_count = 0u;
        signed_matrix2_count += DataUtils::GetChildElementCount(element, "int2x2");
        signed_matrix2_count += DataUtils::GetChildElementCount(element, "float2x2");
        for(unsigned int i = 0; i < signed_matrix2_count; ++i) {
            AppendMatrix2ToConstantBufferBuffer(b_t);
        }
    }
    {
        auto unsigned_matrix2_count = 0u;
        unsigned_matrix2_count += DataUtils::GetChildElementCount(element, "uint2x2");
        unsigned_matrix2_count += DataUtils::GetChildElementCount(element, "dword2x2");
        for(unsigned int i = 0; i < unsigned_matrix2_count; ++i) {
            AppendUnsignedMatrix2ToConstantBufferBuffer(b_t);
        }
    }
    {
        auto signed_matrix3_count = 0u;
        signed_matrix3_count += DataUtils::GetChildElementCount(element, "int3x3");
        signed_matrix3_count += DataUtils::GetChildElementCount(element, "float3x3");
        for(unsigned int i = 0; i < signed_matrix3_count; ++i) {
            AppendMatrix3ToConstantBufferBuffer(b_t);
        }
    }
    {
        auto unsigned_matrix3_count = 0u;
        unsigned_matrix3_count += DataUtils::GetChildElementCount(element, "uint3x3");
        unsigned_matrix3_count += DataUtils::GetChildElementCount(element, "dword3x3");
        for(unsigned int i = 0; i < unsigned_matrix3_count; ++i) {
            AppendUnsignedMatrix3ToConstantBufferBuffer(b_t);
        }
    }
    {
        auto signed_matrix4_count = 0u;
        signed_matrix4_count += DataUtils::GetChildElementCount(element, "int4x4");
        signed_matrix4_count += DataUtils::GetChildElementCount(element, "float4x4");
        for(unsigned int i = 0; i < signed_matrix4_count; ++i) {
            AppendMatrix4ToConstantBufferBuffer(b_t);
        }
    }
    {
        auto unsigned_matrix4_count = 0u;
        unsigned_matrix4_count += DataUtils::GetChildElementCount(element, "uint4x4");
        unsigned_matrix4_count += DataUtils::GetChildElementCount(element, "dword4x4");
        for(unsigned int i = 0; i < unsigned_matrix4_count; ++i) {
            AppendUnsignedMatrix4ToConstantBufferBuffer(b_t);
        }
    }
}
