#include "Engine/Renderer/Renderer.hpp"

#include "Engine/Core/BuildConfig.cpp"
#include "Engine/Core/DataUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Vertex3D.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vector2.hpp"

#include "Engine/RHI/RHIInstance.hpp"
#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"
#include "Engine/RHI/RHIOutput.hpp"

#include "Engine/Renderer/Camera3D.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Renderer/DepthStencilState.hpp"
#include "Engine/Renderer/InputLayout.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/RasterState.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/Renderer/StructuredBuffer.hpp"
#include "Engine/Renderer/Texture1D.hpp"
#include "Engine/Renderer/Texture2D.hpp"
#include "Engine/Renderer/Texture3D.hpp"
#include "Engine/Renderer/Window.hpp"

#include <filesystem>
#include <sstream>
#include <cstddef>

Renderer::Renderer(unsigned int width, unsigned int height)
    : _window_dimensions(width, height)
{
    /* DO NOTHING */
}

Renderer::~Renderer() {

    UnbindAllShaderResources();

    for(auto& texture : _textures) {
        delete texture.second;
        texture.second = nullptr;
    }
    _textures.clear();

    for(auto& sp : _shader_programs) {
        delete sp.second;
        sp.second = nullptr;
    }
    _shader_programs.clear();

    for(auto& material : _materials) {
        delete material.second;
        material.second = nullptr;
    }
    _materials.clear();

    for(auto& shader : _shaders) {
        delete shader.second;
        shader.second = nullptr;
    }
    _shaders.clear();

    for(auto& sampler : _samplers) {
        delete sampler.second;
        sampler.second = nullptr;
    }
    _samplers.clear();

    for(auto& raster : _rasters) {
        delete raster.second;
        raster.second = nullptr;
    }
    _rasters.clear();

    delete _temp_vbo;
    _temp_vbo = nullptr;

    delete _temp_ibo;
    _temp_ibo = nullptr;

    delete _default_depthstencil;
    _default_depthstencil = nullptr;

    delete _current_depthstencil_state;
    _current_depthstencil_state = nullptr;

    _current_raster_state = nullptr;
    _current_sampler = nullptr;
    _current_material = nullptr;

    delete _matrix_cb;
    _matrix_cb = nullptr;

    delete _time_cb;
    _time_cb = nullptr;

    delete _rhi_context;
    _rhi_context = nullptr;

    delete _rhi_device;
    _rhi_device = nullptr;

    delete _rhi_output;
    _rhi_output = nullptr;

    RHIInstance::DestroyInstance();
    _rhi_instance = nullptr;

}

void Renderer::Initialize() {
    _rhi_instance = RHIInstance::CreateInstance();
    _rhi_device = _rhi_instance->CreateDevice();
    _rhi_output = _rhi_device->CreateOutput(_window_dimensions);
    _rhi_context = _rhi_device->GetImmediateContext();

    _default_depthstencil = CreateDepthStencil(_rhi_device, _window_dimensions);

    {
    VertexBuffer::buffer_t default_vbo(1024);
    IndexBuffer::buffer_t default_ibo(1024);
    _temp_vbo = _rhi_device->CreateVertexBuffer(default_vbo, BufferUsage::DYNAMIC, BufferBindUsage::VERTEX_BUFFER);
    _temp_ibo = _rhi_device->CreateIndexBuffer(default_ibo, BufferUsage::DYNAMIC, BufferBindUsage::INDEX_BUFFER);
    _current_vbo_size = default_vbo.size();
    _current_ibo_size = default_ibo.size();
    }

    _matrix_cb = _rhi_device->CreateConstantBuffer(&_matrix_data, sizeof(_matrix_data), BufferUsage::DYNAMIC, BufferBindUsage::CONSTANT_BUFFER);
    //_time_cb = _rhi_device->CreateConstantBuffer(&_time_data, sizeof(_time_data), BufferUsage::DYNAMIC, BufferBindUsage::CONSTANT_BUFFER);

    CreateAndRegisterDefaultSamplers();
    CreateAndRegisterDefaultRasterStates();
    CreateAndRegisterDefaultTextures();
    CreateAndRegisterDefaultShaderPrograms();
    CreateAndRegisterDefaultShaders();
    CreateAndRegisterDefaultMaterials();

}

void Renderer::BeginFrame() {
    /* DO NOTHING */
}

void Renderer::Update(float /*deltaSeconds*/) {
    /* DO NOTHING */
}

void Renderer::Render() const {
    /* DO NOTHING */
}

void Renderer::EndFrame() {
    Present();
}

bool Renderer::RegisterTexture(const std::string& name, Texture* texture) {
    namespace FS = std::experimental::filesystem;
    FS::path p(name);
    auto found_texture = _textures.find(p.string());
    if(found_texture == _textures.end()) {
        _textures.insert_or_assign(name, texture);
        return true;
    } else {
        return false;
    }
}

Texture* Renderer::GetTexture(const std::string& nameOrFile) {
    namespace FS = std::experimental::filesystem;
    FS::path p(nameOrFile);
    auto found_iter = _textures.find(p.string());
    if(found_iter == _textures.end()) {
        return nullptr;
    }
    return _textures[p.string()];
}

void Renderer::DrawPoint(const Vertex3D& point) {
    std::vector<Vertex3D> vbo = { point };
    std::vector<unsigned int> ibo = { 0 };
    DrawIndexed(PrimitiveType::POINTS, vbo, ibo);
}

void Renderer::DrawPoint(const Vector3& point, const Rgba& color /*= Rgba::WHITE*/, const Vector2& tex_coords /*= Vector2::ZERO*/) {
    DrawPoint(Vertex3D(point, color, tex_coords));
}

void Renderer::Draw(const PrimitiveType& topology, const std::vector<Vertex3D>& vbo) {
    UpdateVbo(vbo);
    Draw(topology, _temp_vbo, vbo.size());
}

void Renderer::Draw(const PrimitiveType& topology, const std::vector<Vertex3D>& vbo, std::size_t vertex_count) {
    UpdateVbo(vbo);
    Draw(topology, _temp_vbo, vertex_count);
}

void Renderer::DrawIndexed(const PrimitiveType& topology, const std::vector<Vertex3D>& vbo, const std::vector<unsigned int>& ibo) {
    UpdateVbo(vbo);
    UpdateIbo(ibo);
    DrawIndexed(topology, _temp_vbo, _temp_ibo, ibo.size());
}

void Renderer::DrawIndexed(const PrimitiveType& topology, const std::vector<Vertex3D>& vbo, const std::vector<unsigned int>& ibo, std::size_t vertex_count) {
    UpdateVbo(vbo);
    UpdateIbo(ibo);
    DrawIndexed(topology, _temp_vbo, _temp_ibo, vertex_count);
}

void Renderer::Draw(const PrimitiveType& topology, VertexBuffer* vbo, std::size_t vertex_count) {

    D3D11_PRIMITIVE_TOPOLOGY d3d_prim = PrimitiveTypeToD3dTopology(topology);
    _rhi_context->GetDxContext()->IASetPrimitiveTopology(d3d_prim);
    unsigned int stride = sizeof(VertexBuffer::arraybuffer_t);
    unsigned int offsets = 0;
    ID3D11Buffer* dx_vbo_buffer = vbo->GetDxBuffer();
    _rhi_context->GetDxContext()->IASetVertexBuffers(0, 1, &dx_vbo_buffer, &stride, &offsets);
    _rhi_context->Draw(vertex_count);
}

void Renderer::DrawIndexed(const PrimitiveType& topology, VertexBuffer* vbo, IndexBuffer* ibo, std::size_t index_count) {
    D3D11_PRIMITIVE_TOPOLOGY d3d_prim = PrimitiveTypeToD3dTopology(topology);
    _rhi_context->GetDxContext()->IASetPrimitiveTopology(d3d_prim);
    unsigned int stride = sizeof(VertexBuffer::arraybuffer_t);
    unsigned int offsets = 0;
    ID3D11Buffer* dx_vbo_buffer = vbo->GetDxBuffer();
    ID3D11Buffer* dx_ibo_buffer = ibo->GetDxBuffer();
    _rhi_context->GetDxContext()->IASetVertexBuffers(0, 1, &dx_vbo_buffer, &stride, &offsets);
    _rhi_context->GetDxContext()->IASetIndexBuffer(dx_ibo_buffer, DXGI_FORMAT_R32_UINT, offsets);
    _rhi_context->DrawIndexed(index_count);
}

void Renderer::DrawPoint2D(float pointX, float pointY, const Rgba& color /*= Rgba::WHITE*/) {
    std::vector<Vertex3D> vbo;
    vbo.clear();
    vbo.reserve(1);
    vbo.push_back(Vertex3D(Vector3(pointX, pointY, 0.0f), color));
    std::vector<unsigned int> ibo;
    ibo.clear();
    ibo.reserve(1);
    ibo.push_back(0);
    DrawIndexed(PrimitiveType::POINTS, vbo, ibo);
}
void Renderer::DrawPoint2D(const Vector2& point, const Rgba& color /*= Rgba::WHITE*/) {
    DrawPoint2D(point.x, point.y, color);
}

void Renderer::DrawLine2D(float startX, float startY, float endX, float endY, const Rgba& color /*= Rgba::WHITE*/, float thickness /*= 0.0f*/) {
    bool use_thickness = thickness > 0.0f;
    if(!use_thickness) {
        Vertex3D start = Vertex3D(Vector3(Vector2(startX, startY), 0.0f), color);
        Vertex3D end = Vertex3D(Vector3(Vector2(endX, endY), 0.0f), color);
        std::vector<Vertex3D> vbo;
        vbo.clear();
        vbo.reserve(2);
        vbo.push_back(start);
        vbo.push_back(end);
        std::vector<unsigned int> ibo;
        ibo.clear();
        ibo.reserve(2);
        ibo.push_back(0);
        ibo.push_back(1);
        DrawIndexed(PrimitiveType::LINES, vbo, ibo);
        return;
    }
    Vector3 start = Vector3(Vector2(startX, startY), 0.0f);
    Vector3 end = Vector3(Vector2(endX, endY), 0.0f);
    Vector3 displacement = end - start;
    float length = displacement.CalcLength();
    if(length > 0.0f) {
        Vector3 direction = displacement.GetNormalize();
        Vector3 left_normal = Vector3(-direction.y, direction.x, 0.0f);
        Vector3 right_normal = Vector3(direction.y, -direction.x, 0.0f);
        Vector3 start_left = start + left_normal * thickness * 0.5f;
        Vector3 start_right = start + right_normal * thickness * 0.5f;
        Vector3 end_left = end + left_normal * thickness * 0.5f;
        Vector3 end_right = end + right_normal * thickness * 0.5f;
        std::vector<Vertex3D> vbo;
        vbo.clear();
        vbo.reserve(4);
        vbo.push_back(Vertex3D(start_right, color));
        vbo.push_back(Vertex3D(start_left, color));
        vbo.push_back(Vertex3D(end_left, color));
        vbo.push_back(Vertex3D(end_right, color));
        std::vector<unsigned int> ibo;
        ibo.clear();
        ibo.reserve(6);
        ibo.push_back(0);
        ibo.push_back(1);
        ibo.push_back(2);
        ibo.push_back(0);
        ibo.push_back(2);
        ibo.push_back(3);
        DrawIndexed(PrimitiveType::TRIANGLES, vbo, ibo);
    }
}

void Renderer::DrawLine2D(const Vector2& start, const Vector2& end, const Rgba& color /*= Rgba::WHITE*/, float thickness /*= 0.0f*/) {
    DrawLine2D(start.x, start.y, end.x, end.y, color, thickness);
}

void Renderer::DrawQuad2D(float left, float bottom, float right, float top, const Rgba& color /*= Rgba::WHITE*/) {
    Vector3 pos0 = Vector3(left, bottom, 0.0f);
    Vector3 pos1 = Vector3(left, top, 0.0f);
    Vector3 pos2 = Vector3(right, top, 0.0f);
    Vector3 pos3 = Vector3(right, bottom, 0.0f);
    Vector2 uv_lb = Vector2(0.0f, 0.0f);
    Vector2 uv_lt = Vector2(0.0f, 1.0f);
    Vector2 uv_rb = Vector2(1.0f, 0.0f);
    Vector2 uv_rt = Vector2(1.0f, 1.0f);
    std::vector<Vertex3D> vbo = {
     Vertex3D(pos0, color, uv_lb)
    ,Vertex3D(pos1, color, uv_lt)
    ,Vertex3D(pos2, color, uv_rt)
    ,Vertex3D(pos3, color, uv_rb)
    };
    std::vector<unsigned int> ibo = {
          0, 1, 2
        , 0, 2, 3
    };
    DrawIndexed(PrimitiveType::TRIANGLES, vbo, ibo);
}

void Renderer::DrawQuad2D(const Vector2& position, const Vector2& halfExtents /*= Vector2(0.5f, 0.5f)*/, const Rgba& color /*= Rgba::WHITE*/) {
    float left = position.x - halfExtents.x;
    float bottom = position.y + halfExtents.y;
    float right = position.x + halfExtents.x;
    float top = position.y - halfExtents.y;
    DrawQuad2D(left, bottom, right, top, color);
}

void Renderer::DrawCircle2D(float centerX, float centerY, float radius, const Rgba& color /*= Rgba::WHITE*/) {
    DrawPolygon2D(centerX, centerY, radius, 65, color);
}

void Renderer::DrawCircle2D(const Vector2& center, float radius, const Rgba& color /*= Rgba::WHITE*/) {
    DrawCircle2D(center.x, center.y, radius, color);
}

void Renderer::DrawPolygon2D(float centerX, float centerY, float radius, std::size_t numSides /*= 3*/, const Rgba& color /*= Rgba::WHITE*/) {
    float num_sides_as_float = static_cast<float>(numSides);
    std::vector<Vector3> verts;
    verts.reserve(numSides);
    float anglePerVertex = 360.0f / num_sides_as_float;
    for(float degrees = 0.0f; degrees < 360.0f; degrees += anglePerVertex) {
        float radians = MathUtils::ConvertDegreesToRadians(degrees);
        float pX = radius * std::cos(radians) + centerX;
        float pY = radius * std::sin(radians) + centerY;
        verts.push_back(Vector3(Vector2(pX, pY), 0.0f));
    }

    std::vector<Vertex3D> vbo;
    vbo.resize(verts.size());
    for(std::size_t i = 0; i < vbo.size(); ++i) {
        vbo[i] = Vertex3D(verts[i], color);
    }

    std::vector<unsigned int> ibo;
    ibo.resize(numSides + 1);
    for(std::size_t i = 0; i < ibo.size(); ++i) {
        ibo[i] = i % numSides;
    }
    DrawIndexed(PrimitiveType::LINESSTRIP, vbo, ibo);
}

void Renderer::DrawPolygon2D(const Vector2& center, float radius, std::size_t numSides /*= 3*/, const Rgba& color /*= Rgba::WHITE*/) {
    DrawPolygon2D(center.x, center.y, radius, numSides, color);
}

void Renderer::CreateAndRegisterDefaultShaderPrograms() {
    auto sp = CreateDefaultShaderProgram();
    RegisterShaderProgram(sp->GetName(), sp);

    auto unlit_sp = CreateDefaultUnlitShaderProgram();
    RegisterShaderProgram(unlit_sp->GetName(), unlit_sp);

}

ShaderProgram* Renderer::CreateDefaultShaderProgram() {
    std::string program =
R"(

cbuffer matrix_cb : register(b0) {
    float4x4 g_MODEL;
    float4x4 g_VIEW;
    float4x4 g_PROJECTION;
};


struct vs_in_t {
    float3 position : POSITION;
    float4 color : COLOR;
    float2 uv : UV;
};

struct ps_in_t {
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : UV;
};

SamplerState sSampler : register(s0);

Texture2D<float4> tImage    : register(t0);
Texture2D<float4> tNormal   : register(t1);
Texture2D<float4> tLighting : register(t2);
Texture2D<float4> tSpecular : register(t3);
Texture2D<float4> tOcclusion : register(t4);
Texture2D<float4> tEmissive : register(t5);

ps_in_t VertexFunction(vs_in_t input_vertex) {
    ps_in_t output;

    float4 local = float4(input_vertex.position, 1.0f);
    float4 world = mul(local, g_MODEL);
    float4 view = mul(world, g_VIEW);
    float4 clip = mul(view, g_PROJECTION);

    output.position = clip;
    output.color = input_vertex.color;
    output.uv = input_vertex.uv;

    return output;
}

float4 PixelFunction(ps_in_t input_pixel) : SV_Target0 {
    float4 albedo = tImage.Sample(sSampler, input_pixel.uv);
    return albedo * input_pixel.color;
}

)";
    InputLayout* il = _rhi_device->CreateInputLayout();
    auto pos_offset = offsetof(Vertex3D, position);
    auto color_offset = offsetof(Vertex3D, color);
    auto uv_offset = offsetof(Vertex3D, texcoords);
    il->AddElement(pos_offset, ImageFormat::R32G32B32_FLOAT, "POSITION");
    il->AddElement(color_offset, ImageFormat::R8G8B8A8_UNORM, "COLOR");
    il->AddElement(uv_offset, ImageFormat::R32G32_FLOAT, "UV");
    auto vs_bytecode = _rhi_device->CompileShader("__defaultVS", program.data(), program.size(), "VertexFunction", PipelineStage::VS);
    ID3D11VertexShader* vs = nullptr;
    _rhi_device->GetDxDevice()->CreateVertexShader(vs_bytecode->GetBufferPointer(), vs_bytecode->GetBufferSize(), nullptr, &vs);
    il->CreateInputLayout(vs_bytecode->GetBufferPointer(), vs_bytecode->GetBufferSize());
    auto ps_bytecode = _rhi_device->CompileShader("__defaultPS", program.data(), program.size(), "PixelFunction", PipelineStage::PS);
    ID3D11PixelShader* ps = nullptr;
    _rhi_device->GetDxDevice()->CreatePixelShader(ps_bytecode->GetBufferPointer(), ps_bytecode->GetBufferSize(), nullptr, &ps);
    ShaderProgram* shader = new ShaderProgram("__default", _rhi_device, vs, ps, vs_bytecode, ps_bytecode, il);
    return shader;
}

ShaderProgram* Renderer::CreateDefaultUnlitShaderProgram() {
    std::string program =
        R"(

cbuffer matrix_cb : register(b0) {
    float4x4 g_MODEL;
    float4x4 g_VIEW;
    float4x4 g_PROJECTION;
};


struct vs_in_t {
    float3 position : POSITION;
    float4 color : COLOR;
    float2 uv : UV;
};

struct ps_in_t {
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : UV;
};

SamplerState sSampler : register(s0);

Texture2D<float4> tImage    : register(t0);
Texture2D<float4> tNormal   : register(t1);
Texture2D<float4> tLighting : register(t2);
Texture2D<float4> tSpecular : register(t3);
Texture2D<float4> tOcclusion : register(t4);
Texture2D<float4> tEmissive : register(t5);


ps_in_t VertexFunction(vs_in_t input_vertex) {
    ps_in_t output;

    float4 local = float4(input_vertex.position, 1.0f);
    float4 world = mul(local, g_MODEL);
    float4 view = mul(world, g_VIEW);
    float4 clip = mul(view, g_PROJECTION);

    output.position = clip;
    output.color = input_vertex.color;
    output.uv = input_vertex.uv;

    return output;
}

float4 PixelFunction(ps_in_t input_pixel) : SV_Target0 {
    float4 albedo = tImage.Sample(sSampler, input_pixel.uv);
    return albedo * input_pixel.color;
}

)";

    InputLayout* il = _rhi_device->CreateInputLayout();
    auto pos_offset = offsetof(Vertex3D, position);
    auto color_offset = offsetof(Vertex3D, color);
    auto uv_offset = offsetof(Vertex3D, texcoords);
    il->AddElement(pos_offset, ImageFormat::R32G32B32_FLOAT, "POSITION");
    il->AddElement(color_offset, ImageFormat::R8G8B8A8_UNORM, "COLOR");
    il->AddElement(uv_offset, ImageFormat::R32G32_FLOAT, "UV");
    auto vs_bytecode = _rhi_device->CompileShader("__unlitVS", program.data(), program.size(), "VertexFunction", PipelineStage::VS);
    ID3D11VertexShader* vs = nullptr;
    _rhi_device->GetDxDevice()->CreateVertexShader(vs_bytecode->GetBufferPointer(), vs_bytecode->GetBufferSize(), nullptr, &vs);
    il->CreateInputLayout(vs_bytecode->GetBufferPointer(), vs_bytecode->GetBufferSize());
    auto ps_bytecode = _rhi_device->CompileShader("__unlitPS", program.data(), program.size(), "PixelFunction", PipelineStage::PS);
    ID3D11PixelShader* ps = nullptr;
    _rhi_device->GetDxDevice()->CreatePixelShader(ps_bytecode->GetBufferPointer(), ps_bytecode->GetBufferSize(), nullptr, &ps);
    ShaderProgram* shader = new ShaderProgram("__unlit", _rhi_device, vs, ps, vs_bytecode, ps_bytecode, il);
    return shader;

}

void Renderer::CreateAndRegisterDefaultMaterials() {
    auto default_mat = CreateDefaultMaterial();
    RegisterMaterial(default_mat->GetName(), default_mat);

    auto unlit_mat = CreateDefaultUnlitMaterial();
    RegisterMaterial(unlit_mat->GetName(), unlit_mat);

}

Material* Renderer::CreateDefaultMaterial() {
    std::string material =
R"(
<material name="__default">
    <shader src="__default" />
</material>
)";

    tinyxml2::XMLDocument doc;
    auto parse_result = doc.Parse(material.c_str(), material.size());
    if(parse_result != tinyxml2::XML_SUCCESS) {
        return nullptr;
    }
    return new Material(this, *doc.RootElement());

}

Material* Renderer::CreateDefaultUnlitMaterial() {
    std::string material =
        R"(
<material name="__unlit">
    <shader src="__unlit" />
</material>
)";

    tinyxml2::XMLDocument doc;
    auto parse_result = doc.Parse(material.c_str(), material.size());
    if(parse_result != tinyxml2::XML_SUCCESS) {
        return nullptr;
    }
    return new Material(this, *doc.RootElement());

}

void Renderer::CreateAndRegisterDefaultSamplers() {
    auto default_sampler = CreateDefaultSampler();
    RegisterSampler("__default", default_sampler);
}

Sampler* Renderer::CreateDefaultSampler() {
    return new Sampler(_rhi_device, SamplerDesc{});
}

void Renderer::CreateAndRegisterDefaultRasterStates() {
    RasterState* wireframe_raster = CreateWireframeRaster();
    RegisterRasterState("__wireframe", wireframe_raster);

    RasterState* solid_raster = CreateSolidRaster();
    RegisterRasterState("__solid", solid_raster);

    RasterState* wireframenc_raster = CreateWireframeNoCullingRaster();
    RegisterRasterState("__wireframenc", wireframenc_raster);

    RasterState* solidnc_raster = CreateSolidNoCullingRaster();
    RegisterRasterState("__solidnc", solidnc_raster);

    RasterState* wireframefc_raster = CreateWireframeFrontCullingRaster();
    RegisterRasterState("__wireframefc", wireframefc_raster);

    RasterState* solidfc_raster = CreateSolidFrontCullingRaster();
    RegisterRasterState("__solidfc", solidfc_raster);

}

RasterState* Renderer::CreateWireframeRaster() {
    RasterState* state = new RasterState(_rhi_device, FillMode::WIREFRAME, CullMode::BACK, true);
    return state;
}

RasterState* Renderer::CreateSolidRaster() {
    RasterState* state = new RasterState(_rhi_device, FillMode::SOLID, CullMode::BACK, true);
    return state;
}

RasterState* Renderer::CreateWireframeNoCullingRaster() {
    RasterState* state = new RasterState(_rhi_device, FillMode::WIREFRAME, CullMode::NONE, true);
    return state;
}

RasterState* Renderer::CreateSolidNoCullingRaster() {
    RasterState* state = new RasterState(_rhi_device, FillMode::SOLID, CullMode::NONE, true);
    return state;
}

RasterState* Renderer::CreateWireframeFrontCullingRaster() {
    RasterState* state = new RasterState(_rhi_device, FillMode::WIREFRAME, CullMode::FRONT, true);
    return state;
}

RasterState* Renderer::CreateSolidFrontCullingRaster() {
    RasterState* state = new RasterState(_rhi_device, FillMode::SOLID, CullMode::FRONT, true);
    return state;
}

void Renderer::UnbindAllShaderResources() {
    _rhi_context->UnbindAllShaderResources();
}

RasterState* Renderer::GetRasterState(const std::string& name) {
    auto found_iter = _rasters.find(name);
    if(found_iter == _rasters.end()) {
        return nullptr;
    }
    return found_iter->second;
}

void Renderer::CreateAndRegisterSamplerFromSamplerDescription(const std::string& name, const SamplerDesc& desc) {
    Sampler* sampler = new Sampler(_rhi_device, desc);
    RegisterSampler(name, sampler);
}

Sampler* Renderer::GetSampler(const std::string& name) {
    auto found_iter = _samplers.find(name);
    if(found_iter == _samplers.end()) {
        return nullptr;
    }
    return found_iter->second;
}

void Renderer::SetSampler(Sampler* sampler) {
    if(sampler == _current_sampler) {
        return;
    }
    _current_sampler = sampler;
    _rhi_context->SetSampler(sampler);
}

void Renderer::RegisterRasterState(const std::string& name, RasterState* raster) {
    if(raster == nullptr) {
        return;
    }
    auto found_iter = _rasters.find(name);
    if(found_iter != _rasters.end()) {
        delete found_iter->second;
        found_iter->second = nullptr;
    }
    _rasters.insert_or_assign(name, raster);
}

void Renderer::RegisterSampler(const std::string& name, Sampler* sampler) {
    if(sampler == nullptr) {
        return;
    }
    auto found_iter = _samplers.find(name);
    if(found_iter != _samplers.end()) {
        delete found_iter->second;
        found_iter->second = nullptr;
    }
    _samplers.insert_or_assign(name, sampler);
}

void Renderer::RegisterShader(const std::string& name, Shader* shader) {
    if(shader == nullptr) {
        return;
    }
    auto found_iter = _materials.find(name);
    if(found_iter != _materials.end()) {
        delete found_iter->second;
        found_iter->second = nullptr;
    }
    _shaders.insert_or_assign(name, shader);
}

void Renderer::CreateAndRegisterDefaultTextures() {
    auto default_texture = CreateDefaultTexture();
    RegisterTexture("__default", default_texture);

    auto invalid_texture = CreateInvalidTexture();
    RegisterTexture("__invalid", invalid_texture);

    auto diffuse_texture = CreateDefaultDiffuseTexture();
    RegisterTexture("__diffuse", diffuse_texture);

    auto normal_texture = CreateDefaultNormalTexture();
    RegisterTexture("__normal", normal_texture);

    auto specular_texture = CreateDefaultSpecularTexture();
    RegisterTexture("__specular", specular_texture);

    auto occlusion_texture = CreateDefaultOcclusionTexture();
    RegisterTexture("__occlusion", occlusion_texture);

    auto emissive_texture = CreateDefaultEmissiveTexture();
    RegisterTexture("__emissive", emissive_texture);

}

Texture* Renderer::CreateDefaultTexture() {
    std::vector<Rgba> data = {
        Rgba::WHITE
    };
    return Create2DTextureFromMemory(data, 1, 1);
}

Texture* Renderer::CreateInvalidTexture() {
    std::vector<Rgba> data = {
        Rgba::MAGENTA, Rgba::BLACK,
        Rgba::BLACK,   Rgba::MAGENTA,
    };
    return Create2DTextureFromMemory(data, 2, 2);
}

Texture* Renderer::CreateDefaultDiffuseTexture() {
    std::vector<Rgba> data = {
        Rgba::WHITE
    };
    return Create2DTextureFromMemory(data, 1, 1);
}

Texture* Renderer::CreateDefaultNormalTexture() {
    std::vector<Rgba> data = {
        Rgba::NORMAL_Z
    };
    return Create2DTextureFromMemory(data, 1, 1);
}

Texture* Renderer::CreateDefaultLightingTexture() {
    std::vector<Rgba> data = {
        Rgba::WHITE
    };
    return Create2DTextureFromMemory(data, 1, 1);
}

Texture* Renderer::CreateDefaultSpecularTexture() {
    std::vector<Rgba> data = {
        Rgba::BLACK
    };
    return Create2DTextureFromMemory(data, 1, 1);
}

Texture* Renderer::CreateDefaultOcclusionTexture() {
    std::vector<Rgba> data = {
        Rgba::WHITE
    };
    return Create2DTextureFromMemory(data, 1, 1);
}

Texture* Renderer::CreateDefaultEmissiveTexture() {
    std::vector<Rgba> data = {
        Rgba::BLACK
    };
    return Create2DTextureFromMemory(data, 1, 1);
}

void Renderer::CreateAndRegisterDefaultShaders() {
    auto default_shader = CreateDefaultShader();
    RegisterShader(default_shader->GetName(), default_shader);

    auto default_unlit = CreateDefaultUnlitShader();
    RegisterShader(default_unlit->GetName(), default_unlit);
}

Shader* Renderer::CreateDefaultShader() {
    std::string shader =
R"(
<shader name="__default">
    <shaderprogram src="__default" />
    <raster src="__solid" />
    <sampler src="__default" />
    <blends>
        <blend enable="true">
            <color src="src_alpha" dest="inv_src_alpha" op="add" />
        </blend>
    </blends>
</shader>
)";
    tinyxml2::XMLDocument doc;
    auto parse_result = doc.Parse(shader.c_str(), shader.size());
    if(parse_result != tinyxml2::XML_SUCCESS) {
        return nullptr;
    }

    return new Shader(this, *doc.RootElement());
}

Shader* Renderer::CreateDefaultUnlitShader() {
    std::string shader =
        R"(
<shader name="__unlit">
    <shaderprogram src="__unlit" />
    <raster src="__solid" />
    <sampler src="__default" />
    <blends>
        <blend enable="true">
            <color src="src_alpha" dest="inv_src_alpha" op="add" />
        </blend>
    </blends>
</shader>
)";
    tinyxml2::XMLDocument doc;
    auto parse_result = doc.Parse(shader.c_str(), shader.size());
    if(parse_result != tinyxml2::XML_SUCCESS) {
        return nullptr;
    }

    return new Shader(this, *doc.RootElement());
}

std::size_t Renderer::GetMaterialCount() {
    return _materials.size();
}

void Renderer::RegisterMaterial(const std::string name, Material* mat) {
    if(mat == nullptr) {
        return;
    }
    auto found_iter = _materials.find(name);
    if(found_iter != _materials.end()) {
        delete found_iter->second;
        found_iter->second = nullptr;
    }
    _materials.insert_or_assign(name, mat);
}

void Renderer::RegisterShaderProgram(const std::string& name, ShaderProgram * sp) {
    if(sp == nullptr) {
        return;
    }
    auto found_iter = _shader_programs.find(name);
    if(found_iter != _shader_programs.end()) {
        delete found_iter->second;
        found_iter->second = nullptr;
    }
    _shader_programs.insert_or_assign(name, sp);
}

void Renderer::UpdateVbo(const VertexBuffer::buffer_t& vbo) {
    if(_current_vbo_size < vbo.size()) {
        delete _temp_vbo;
        _temp_vbo = _rhi_device->CreateVertexBuffer(vbo, BufferUsage::DYNAMIC, BufferBindUsage::VERTEX_BUFFER);
        _current_vbo_size = vbo.size();
    }
    _temp_vbo->Update(_rhi_context, vbo);
}

void Renderer::UpdateIbo(const IndexBuffer::buffer_t& ibo) {
    if(_current_ibo_size < ibo.size()) {
        delete _temp_ibo;
        _temp_ibo = _rhi_device->CreateIndexBuffer(ibo, BufferUsage::DYNAMIC, BufferBindUsage::INDEX_BUFFER);
        _current_ibo_size = ibo.size();
    }
    _temp_ibo->Update(_rhi_context, ibo);
}

RHIDeviceContext* Renderer::GetDeviceContext() const {
    return _rhi_context;
}

RHIDevice* Renderer::GetDevice() const {
    return _rhi_device;
}

RHIOutput* Renderer::GetOutput() const {
    return _rhi_output;
}

ShaderProgram* Renderer::GetShaderProgram(const std::string& nameOrFile) {
    auto found_iter = _shader_programs.find(nameOrFile);
    if(found_iter == _shader_programs.end()) {
        return nullptr;
    }
    return found_iter->second;
}

ShaderProgram* Renderer::CreateShaderProgramFromHlslFile(const std::string& filepath, const std::string& entryPoint, InputLayout* inputLayout, const PipelineStage& target) {
    std::vector<unsigned char> contents;
    if(FileUtils::ReadBufferFromFile(contents, filepath)) {
        std::string data(reinterpret_cast<const char*>(contents.data()), contents.size());
        ShaderProgram* sp = _rhi_device->CreateShaderProgramFromHlslString(filepath, data, entryPoint, inputLayout, target);
        return sp;
    }
    return nullptr;
}

void Renderer::CreateAndRegisterRasterStateFromRasterDescription(const std::string& name, const RasterDesc& desc) {
    auto raster = new RasterState(_rhi_device, desc);
    RegisterRasterState(name, raster);
}

void Renderer::SetRasterState(RasterState* raster) {
    if(raster == _current_raster_state) {
        return;
    }
    _current_raster_state = raster;
    _rhi_context->SetRasterState(raster);
}

void Renderer::SetVSync(bool value) {
    _vsync = value;
}

Material* Renderer::GetMaterial(const std::string& nameOrFile) {
    auto found_iter = _materials.find(nameOrFile);
    if(found_iter == _materials.end()) {
        return nullptr;
    }
    return found_iter->second;
}

void Renderer::SetMaterial(Material* material) {
    if(material == nullptr) {
        material = GetMaterial("__default");
    }
    if(_current_material == material) {
        return;
    }
    _rhi_context->SetMaterial(material);
    _current_material = material;
}

const std::map<std::string, Texture*>& Renderer::GetLoadedTextures() const {
    return _textures;
}

Shader* Renderer::GetShader(const std::string& nameOrFile) {
    auto found_iter = _shaders.find(nameOrFile);
    if(found_iter == _shaders.end()) {
        return nullptr;
    }
    return found_iter->second;
}

void Renderer::SetModelMatrix(const Matrix4& mat) {
    _matrix_data.model = mat;
    _matrix_cb->Update(_rhi_context, &_matrix_data);
    SetConstantBuffer(MATRIX_BUFFER_INDEX, _matrix_cb);
}

void Renderer::SetViewMatrix(const Matrix4& mat) {
    _matrix_data.view = mat;
    _matrix_cb->Update(_rhi_context, &_matrix_data);
    SetConstantBuffer(MATRIX_BUFFER_INDEX, _matrix_cb);
}

void Renderer::SetProjectionMatrix(const Matrix4& mat) {
    _matrix_data.projection = mat;
    _matrix_cb->Update(_rhi_context, &_matrix_data);
    SetConstantBuffer(MATRIX_BUFFER_INDEX, _matrix_cb);
}

void Renderer::AppendModelMatrix(const Matrix4& modelMatrix) {
    _matrix_data.model = modelMatrix * _matrix_data.model;
    _matrix_cb->Update(_rhi_context, &_matrix_data);
    SetConstantBuffer(MATRIX_BUFFER_INDEX, _matrix_cb);
}

void Renderer::SetOrthoProjection(const Vector2& leftBottom, const Vector2& rightTop, const Vector2& near_far) {
    Matrix4 proj = Matrix4::CreateDXOrthographicProjection(leftBottom.x, rightTop.x, leftBottom.y, rightTop.y, near_far.x, near_far.y);
    SetProjectionMatrix(proj);
}

void Renderer::SetOrthoProjection(const Vector2& dimensions, const Vector2& origin, float nearz, float farz) {
    Vector2 half_extents = dimensions * 0.5f;
    Vector2 leftBottom = Vector2(origin.x - half_extents.x, origin.y - half_extents.y);
    Vector2 rightTop = Vector2(origin.x + half_extents.x, origin.y + half_extents.y);
    SetOrthoProjection(leftBottom, rightTop, Vector2(nearz, farz));
}

void Renderer::SetOrthoProjectionFromViewHeight(float viewHeight, float aspectRatio, float nearz, float farz) {
    float view_height = viewHeight;
    float view_width = view_height * aspectRatio;
    Vector2 view_half_extents = Vector2(view_width, view_height) * 0.50f;
    Vector2 leftBottom = Vector2(-view_half_extents.x, -view_half_extents.y);
    Vector2 rightTop = Vector2(view_half_extents.x, view_half_extents.y);
    SetOrthoProjection(leftBottom, rightTop, Vector2(nearz, farz));
}

void Renderer::SetOrthoProjectionFromViewWidth(float viewWidth, float aspectRatio, float nearz, float farz) {
    float inv_aspect_ratio = 1.0f / aspectRatio;
    float view_width = viewWidth;
    float view_height = view_width * inv_aspect_ratio;
    Vector2 view_half_extents = Vector2(view_width, view_height) * 0.50f;
    Vector2 leftBottom = Vector2(-view_half_extents.x, -view_half_extents.y);
    Vector2 rightTop = Vector2(view_half_extents.x, view_half_extents.y);
    SetOrthoProjection(leftBottom, rightTop, Vector2(nearz, farz));
}

void Renderer::SetOrthoProjectionFromCamera(const Camera3D& camera) {
    float view_height = camera.CalcNearViewHeight();
    float view_width = view_height * camera.GetAspectRatio();
    Vector2 view_half_extents = Vector2(view_width, view_height) * 0.50f;
    Vector2 leftBottom = Vector2(-view_half_extents.x, -view_half_extents.y);
    Vector2 rightTop = Vector2(view_half_extents.x, view_half_extents.y);
    SetOrthoProjection(leftBottom, rightTop, Vector2(camera.GetNearDistance(), camera.GetFarDistance()));
}

void Renderer::SetPerspectiveProjection(const Vector2& vfovDegrees_aspect, const Vector2& nz_fz) {
    Matrix4 proj = Matrix4::CreateDXPerspectiveProjection(vfovDegrees_aspect.x, vfovDegrees_aspect.y, nz_fz.x, nz_fz.y);
    SetProjectionMatrix(proj);
}

void Renderer::SetPerspectiveProjectionFromCamera(const Camera3D& camera) {
    SetPerspectiveProjection(Vector2{ camera.GetFovYDegrees(), camera.GetAspectRatio()}, Vector2{ camera.GetNearDistance(), camera.GetFarDistance()});
}

void Renderer::SetConstantBuffer(unsigned int index, ConstantBuffer* buffer) {
    _rhi_context->SetConstantBuffer(index, buffer);
}

void Renderer::SetStructuredBuffer(unsigned int index, StructuredBuffer* buffer) {
    _rhi_context->SetStructuredBuffer(index, buffer);
}

std::size_t Renderer::GetShaderCount() const {
    return _shaders.size();
}

void Renderer::SetRenderTarget(Texture* color_target /*= nullptr*/, Texture* depthstencil_target /*= nullptr*/) {
    if(color_target != nullptr) {
        _current_target = color_target;
    } else {
        _current_target = _rhi_output->GetBackBuffer();
    }

    if(depthstencil_target != nullptr) {
        _current_depthstencil = depthstencil_target;
    } else {
        _current_depthstencil = _default_depthstencil;
    }
    ID3D11DepthStencilView* dsv = _current_depthstencil->GetDepthStencilView();
    ID3D11RenderTargetView* rtv = _current_target->GetRenderTargetView();
    _rhi_context->GetDxContext()->OMSetRenderTargets(1, &rtv, dsv);
}

void Renderer::SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
    D3D11_VIEWPORT viewport;
    memset(&viewport, 0, sizeof(viewport));

    viewport.TopLeftX = static_cast<float>(x);
    viewport.TopLeftY = static_cast<float>(y);
    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    _rhi_context->GetDxContext()->RSSetViewports(1, &viewport);
}

void Renderer::SetViewportAsPercent(float /*x*/, float /*y*/, float /*w*/, float /*h*/) {
    /* DO NOTHING */
}

void Renderer::ClearColor(const Rgba& color) {
    _rhi_context->ClearColorTarget(_current_target, color);
}

void Renderer::ClearTargetColor(Texture* target, const Rgba& color) {
    _rhi_context->ClearColorTarget(target, color);
}

void Renderer::ClearDepthStencilBuffer() {
    _rhi_context->ClearDepthStencilTarget(_current_depthstencil);
}

void Renderer::Present() {
    _rhi_output->Present(_vsync);
}

Texture* Renderer::CreateOrGetTexture(const std::string& filepath, const IntVector3& dimensions) {
    namespace FS = std::experimental::filesystem;
    FS::path p(filepath);
    auto texture_iter = _textures.find(p.string());
    if(texture_iter == _textures.end()) {
        return CreateTexture(filepath, dimensions);
    } else {
        return GetTexture(filepath);
    }
}

Texture* Renderer::CreateTexture(const std::string& filepath,
                                 const IntVector3& dimensions /*= IntVector3::XY_AXIS*/,
                                 const BufferUsage& bufferUsage /*= BufferUsage::STATIC*/,
                                 const BufferBindUsage& bindUsage /*= BufferBindUsage::SHADER_RESOURCE*/,
                                 const ImageFormat& imageFormat /*= ImageFormat::R8G8B8A8_UNORM*/) {
    if(dimensions.y == 0 && dimensions.z == 0) {
        return Create1DTexture(filepath, bufferUsage, bindUsage, imageFormat);
    } else if(dimensions.z == 0) {
        return Create2DTexture(filepath, bufferUsage, bindUsage, imageFormat);
    } else {
        return Create3DTexture(filepath, dimensions, bufferUsage, bindUsage, imageFormat);
    }
}

void Renderer::SetTexture(Texture* texture, unsigned int registerIndex /*= 0*/) {
    if(_current_target == texture) {
        return;
    }
    if(texture == nullptr) {
        texture = GetTexture("__default");
    }
    _current_target = texture;
    _rhi_context->SetTexture(registerIndex, _current_target);
}

Texture* Renderer::CreateDepthStencil(RHIDevice* owner, const IntVector2& dimensions) {

    ID3D11Texture2D* dx_resource = nullptr;

    D3D11_TEXTURE2D_DESC descDepth;
    descDepth.Width = dimensions.x;
    descDepth.Height = dimensions.y;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    HRESULT texture_hr = owner->GetDxDevice()->CreateTexture2D(&descDepth, nullptr, &dx_resource);
    bool texture_creation_succeeded = SUCCEEDED(texture_hr);
    if(texture_creation_succeeded) {
        if(_current_depthstencil) {
            delete _current_depthstencil;
            _current_depthstencil = nullptr;
        }
        if(_current_depthstencil_state) {
            delete _current_depthstencil_state;
            _current_depthstencil_state = nullptr;
        }
        _current_depthstencil = new Texture2D(owner, dx_resource);
        _current_depthstencil_state = new DepthStencilState(owner);
        return _current_depthstencil;
    }
    return nullptr;
}

Texture* Renderer::Create1DTexture(const std::string& filepath, const BufferUsage& bufferUsage, const BufferBindUsage& bindUsage, const ImageFormat& imageFormat) {
    namespace FS = std::experimental::filesystem;
    FS::path p(filepath);
    if(!FS::exists(p)) {
        return GetTexture("__invalid");
    }
    Image img = Image(p.string());

    D3D11_TEXTURE1D_DESC tex_desc;
    memset(&tex_desc, 0, sizeof(tex_desc));

    tex_desc.Width = img.GetDimensions().x;     // width... 
    tex_desc.MipLevels = 1;    // setting to 0 means there's a full chain (or can generate a full chain) - we're immutable, so not allowed
    tex_desc.ArraySize = 1;    // only one texture (
    tex_desc.Usage = BufferUsageToD3DUsage(bufferUsage);            // data is set at creation time and won't change
    tex_desc.Format = ImageFormatToDxgiFormat(imageFormat);      // R8G8B8A8 texture
    tex_desc.BindFlags = BufferBindUsageToD3DBindFlags(bindUsage);   // we're going to be using this texture as a shader resource
    tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(bufferUsage);                      // Determines how I can access this resource CPU side (IMMUTABLE, So none)
    tex_desc.MiscFlags = 0;                            // Extra Flags, of note is;

                                                       // Setup Initial Data
    D3D11_SUBRESOURCE_DATA subresource_data;
    memset(&subresource_data, 0, sizeof(subresource_data));

    auto width = img.GetDimensions().x;
    auto height = img.GetDimensions().y;
    subresource_data.pSysMem = img.GetData();
    subresource_data.SysMemPitch = width * sizeof(unsigned int); // pitch is byte size of a single row)
    subresource_data.SysMemSlicePitch = width * height * sizeof(unsigned int);
    //Force specific usages for unordered access
    if((bindUsage & BufferBindUsage::UNORDERED_ACCESS) == BufferBindUsage::UNORDERED_ACCESS) {
        tex_desc.Usage = BufferUsageToD3DUsage(BufferUsage::GPU);
        tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(BufferUsage::STAGING);
    }
    ID3D11Texture1D* dx_tex = nullptr;

    bool isImmutable = bufferUsage == BufferUsage::STATIC;
    bool mustUseInitialData = isImmutable;

    HRESULT hr = _rhi_device->GetDxDevice()->CreateTexture1D(&tex_desc, (mustUseInitialData ? &subresource_data : nullptr), &dx_tex);
    bool succeeded = SUCCEEDED(hr);
    if(succeeded) {
        auto* tex = new Texture1D(_rhi_device, dx_tex);
#ifdef RENDER_DEBUG
        tex->SetDebugName(p.string().c_str());
#endif
        tex->IsLoaded(true);
        if(RegisterTexture(p.string(), tex)) {
            return tex;
        } else {
            delete tex;
            return nullptr;
        }
    } else {
        return nullptr;
    }
}

Texture* Renderer::Create1DTextureFromMemory(const unsigned char* data, unsigned int width /*= 1*/, const BufferUsage& bufferUsage /*= BufferUsage::STATIC*/, const BufferBindUsage& bindUsage /*= BufferBindUsage::SHADER_RESOURCE*/, const ImageFormat& imageFormat /*= ImageFormat::R8G8B8A8_UNORM*/) {
    D3D11_TEXTURE1D_DESC tex_desc = {};

    tex_desc.Width = width;
    tex_desc.MipLevels = 1;
    tex_desc.ArraySize = 1;
    tex_desc.Usage = BufferUsageToD3DUsage(bufferUsage);
    tex_desc.Format = ImageFormatToDxgiFormat(imageFormat);
    tex_desc.BindFlags = BufferBindUsageToD3DBindFlags(bindUsage);
    //Make every texture a target and shader resource
    tex_desc.BindFlags |= BufferBindUsageToD3DBindFlags(BufferBindUsage::SHADER_RESOURCE);
    tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(bufferUsage);
    //Force specific usages for unordered access
    if(bindUsage == BufferBindUsage::UNORDERED_ACCESS) {
        tex_desc.Usage = BufferUsageToD3DUsage(BufferUsage::GPU);
        tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(BufferUsage::STAGING);
    }
    tex_desc.MiscFlags = 0;

    // Setup Initial Data
    D3D11_SUBRESOURCE_DATA subresource_data = {};

    subresource_data.pSysMem = data;
    subresource_data.SysMemPitch = width * sizeof(unsigned char);
    subresource_data.SysMemSlicePitch = width * sizeof(unsigned char);

    ID3D11Texture1D* dx_tex = nullptr;

    //If IMMUTABLE or not multi-sampled, must use initial data.
    bool isMultiSampled = false;
    bool isImmutable = bufferUsage == BufferUsage::STATIC;
    bool mustUseInitialData = isImmutable || !isMultiSampled;

    HRESULT hr = _rhi_device->GetDxDevice()->CreateTexture1D(&tex_desc, (mustUseInitialData ? &subresource_data : nullptr), &dx_tex);
    bool succeeded = SUCCEEDED(hr);
    if(succeeded) {
        return new Texture1D(_rhi_device, dx_tex);
    } else {
        return nullptr;
    }
}

Texture* Renderer::Create1DTextureFromMemory(const std::vector<Rgba>& data, unsigned int width /*= 1*/, const BufferUsage& bufferUsage /*= BufferUsage::STATIC*/, const BufferBindUsage& bindUsage /*= BufferBindUsage::SHADER_RESOURCE*/, const ImageFormat& imageFormat /*= ImageFormat::R8G8B8A8_UNORM*/) {
    D3D11_TEXTURE1D_DESC tex_desc = {};

    tex_desc.Width = width;
    tex_desc.MipLevels = 1;
    tex_desc.ArraySize = 1;
    tex_desc.Usage = BufferUsageToD3DUsage(bufferUsage);
    tex_desc.Format = ImageFormatToDxgiFormat(imageFormat);
    tex_desc.BindFlags = BufferBindUsageToD3DBindFlags(bindUsage);
    //Make every texture a target and shader resource
    tex_desc.BindFlags |= BufferBindUsageToD3DBindFlags(BufferBindUsage::SHADER_RESOURCE);
    tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(bufferUsage);
    //Force specific usages for unordered access
    if((bindUsage & BufferBindUsage::UNORDERED_ACCESS) == BufferBindUsage::UNORDERED_ACCESS) {
        tex_desc.Usage = BufferUsageToD3DUsage(BufferUsage::GPU);
        tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(BufferUsage::STAGING);
    }
    tex_desc.MiscFlags = 0;

    // Setup Initial Data
    D3D11_SUBRESOURCE_DATA subresource_data = {};

    subresource_data.pSysMem = data.data();
    subresource_data.SysMemPitch = width * sizeof(Rgba);
    subresource_data.SysMemSlicePitch = width * sizeof(Rgba);

    ID3D11Texture1D* dx_tex = nullptr;

    //If IMMUTABLE or not multi-sampled, must use initial data.
    bool isMultiSampled = false;
    bool isImmutable = bufferUsage == BufferUsage::STATIC;
    bool mustUseInitialData = isImmutable || isMultiSampled;

    HRESULT hr = _rhi_device->GetDxDevice()->CreateTexture1D(&tex_desc, (mustUseInitialData ? &subresource_data : nullptr), &dx_tex);
    bool succeeded = SUCCEEDED(hr);
    if(succeeded) {
        return new Texture1D(_rhi_device, dx_tex);
    } else {
        return nullptr;
    }
}

Texture* Renderer::Create2DTexture(const std::string& filepath, const BufferUsage& bufferUsage, const BufferBindUsage& bindUsage, const ImageFormat& imageFormat) {
    namespace FS = std::experimental::filesystem;
    FS::path p(filepath);
    if(!FS::exists(p)) {
        return GetTexture("__invalid");
    }
    Image img = Image(p.string());

    D3D11_TEXTURE2D_DESC tex_desc;
    memset(&tex_desc, 0, sizeof(tex_desc));

    tex_desc.Width = img.GetDimensions().x;     // width... 
    tex_desc.Height = img.GetDimensions().y;    // ...and height of image in pixels.
    tex_desc.MipLevels = 1;    // setting to 0 means there's a full chain (or can generate a full chain) - we're immutable, so not allowed
    tex_desc.ArraySize = 1;    // only one texture (
    tex_desc.Usage = BufferUsageToD3DUsage(bufferUsage);            // data is set at creation time and won't change
    tex_desc.Format = ImageFormatToDxgiFormat(imageFormat);      // R8G8B8A8 texture
    tex_desc.BindFlags = BufferBindUsageToD3DBindFlags(bindUsage);   // we're going to be using this texture as a shader resource
                                                                     //Make every texture a shader resource
    tex_desc.BindFlags |= BufferBindUsageToD3DBindFlags(BufferBindUsage::SHADER_RESOURCE);
    tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(bufferUsage);                      // Determines how I can access this resource CPU side (IMMUTABLE, So none)
    tex_desc.MiscFlags = 0;                            // Extra Flags, of note is;
                                                       // D3D11_RESOURCE_MISC_GENERATE_MIPS - if we want to use this to be able to generate mips (not compatible with IMMUTABLE)

                                                       // If Multisampling - set this up - we're not multisampling, so 1 and 0
                                                       // (MSAA as far as I know only makes sense for Render Targets, not shader resource textures)
    tex_desc.SampleDesc.Count = 1;
    tex_desc.SampleDesc.Quality = 0;

    // Setup Initial Data
    D3D11_SUBRESOURCE_DATA subresource_data;
    memset(&subresource_data, 0, sizeof(subresource_data));

    auto width = img.GetDimensions().x;
    auto height = img.GetDimensions().y;
    subresource_data.pSysMem = img.GetData();
    subresource_data.SysMemPitch = width * sizeof(unsigned int); // pitch is byte size of a single row)
    subresource_data.SysMemSlicePitch = width * height * sizeof(unsigned int);
    //Force specific usages for unordered access
    if((bindUsage & BufferBindUsage::UNORDERED_ACCESS) == BufferBindUsage::UNORDERED_ACCESS) {
        tex_desc.Usage = BufferUsageToD3DUsage(BufferUsage::GPU);
        tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(BufferUsage::STAGING);
    }
    ID3D11Texture2D* dx_tex = nullptr;

    //If IMMUTABLE or not multi-sampled, must use initial data.
    bool isMultiSampled = tex_desc.SampleDesc.Count != 1 || tex_desc.SampleDesc.Quality != 0;
    bool isImmutable = bufferUsage == BufferUsage::STATIC;
    bool mustUseInitialData = isImmutable || !isMultiSampled;

    HRESULT hr = _rhi_device->GetDxDevice()->CreateTexture2D(&tex_desc, (mustUseInitialData ? &subresource_data : nullptr), &dx_tex);
    bool succeeded = SUCCEEDED(hr);
    if(succeeded) {
        auto* tex = new Texture2D(_rhi_device, dx_tex);
#ifdef RENDER_DEBUG
        tex->SetDebugName(p.string().c_str());
#endif
        tex->IsLoaded(true);
        if(RegisterTexture(p.string(), tex)) {
            return tex;
        } else {
            delete tex;
            return nullptr;
        }
    } else {
        return nullptr;
    }
}

Texture* Renderer::Create2DTextureFromMemory(const unsigned char* data, unsigned int width /*= 1*/, unsigned int height /*= 1*/, const BufferUsage& bufferUsage /*= BufferUsage::STATIC*/, const BufferBindUsage& bindUsage /*= BufferBindUsage::SHADER_RESOURCE*/, const ImageFormat& imageFormat /*= ImageFormat::R8G8B8A8_UNORM*/) {
    D3D11_TEXTURE2D_DESC tex_desc = {};

    tex_desc.Width = width;
    tex_desc.Height = height;
    tex_desc.MipLevels = 1;
    tex_desc.ArraySize = 1;
    tex_desc.Usage = BufferUsageToD3DUsage(bufferUsage);
    tex_desc.Format = ImageFormatToDxgiFormat(imageFormat);
    tex_desc.BindFlags = BufferBindUsageToD3DBindFlags(bindUsage);
    //Make every texture a target and shader resource
    tex_desc.BindFlags |= BufferBindUsageToD3DBindFlags(BufferBindUsage::SHADER_RESOURCE);
    tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(bufferUsage);
    //Force specific usages for unordered access
    if(bindUsage == BufferBindUsage::UNORDERED_ACCESS) {
        tex_desc.Usage = BufferUsageToD3DUsage(BufferUsage::GPU);
        tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(BufferUsage::STAGING);
    }
    tex_desc.MiscFlags = 0;
    tex_desc.SampleDesc.Count = 1;
    tex_desc.SampleDesc.Quality = 0;

    // Setup Initial Data
    D3D11_SUBRESOURCE_DATA subresource_data = {};

    subresource_data.pSysMem = data;
    subresource_data.SysMemPitch = width * sizeof(unsigned char);
    subresource_data.SysMemSlicePitch = width * height * sizeof(unsigned char);

    ID3D11Texture2D* dx_tex = nullptr;

    //If IMMUTABLE or not multi-sampled, must use initial data.
    bool isMultiSampled = tex_desc.SampleDesc.Count != 1 || tex_desc.SampleDesc.Quality != 0;
    bool isImmutable = bufferUsage == BufferUsage::STATIC;
    bool mustUseInitialData = isImmutable || !isMultiSampled;

    HRESULT hr = _rhi_device->GetDxDevice()->CreateTexture2D(&tex_desc, (mustUseInitialData ? &subresource_data : nullptr), &dx_tex);
    bool succeeded = SUCCEEDED(hr);
    if(succeeded) {
        return new Texture2D(_rhi_device, dx_tex);
    } else {
        return nullptr;
    }
}

Texture* Renderer::Create2DTextureFromMemory(const std::vector<Rgba>& data, unsigned int width /*= 1*/, unsigned int height /*= 1*/, const BufferUsage& bufferUsage /*= BufferUsage::STATIC*/, const BufferBindUsage& bindUsage /*= BufferBindUsage::SHADER_RESOURCE*/, const ImageFormat& imageFormat /*= ImageFormat::R8G8B8A8_UNORM*/) {
    D3D11_TEXTURE2D_DESC tex_desc = {};

    tex_desc.Width = width;
    tex_desc.Height = height;
    tex_desc.MipLevels = 1;
    tex_desc.ArraySize = 1;
    tex_desc.Usage = BufferUsageToD3DUsage(bufferUsage);
    tex_desc.Format = ImageFormatToDxgiFormat(imageFormat);
    tex_desc.BindFlags = BufferBindUsageToD3DBindFlags(bindUsage);
    //Make every texture a target and shader resource
    tex_desc.BindFlags |= BufferBindUsageToD3DBindFlags(BufferBindUsage::SHADER_RESOURCE);
    tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(bufferUsage);
    //Force specific usages for unordered access
    if((bindUsage & BufferBindUsage::UNORDERED_ACCESS) == BufferBindUsage::UNORDERED_ACCESS) {
        tex_desc.Usage = BufferUsageToD3DUsage(BufferUsage::GPU);
        tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(BufferUsage::STAGING);
    }
    tex_desc.MiscFlags = 0;
    tex_desc.SampleDesc.Count = 1;
    tex_desc.SampleDesc.Quality = 0;

    // Setup Initial Data
    D3D11_SUBRESOURCE_DATA subresource_data = {};

    subresource_data.pSysMem = data.data();
    subresource_data.SysMemPitch = width * sizeof(Rgba);
    subresource_data.SysMemSlicePitch = width * height * sizeof(Rgba);

    ID3D11Texture2D* dx_tex = nullptr;

    //If IMMUTABLE or not multi-sampled, must use initial data.
    bool isMultiSampled = tex_desc.SampleDesc.Count != 1 || tex_desc.SampleDesc.Quality != 0;
    bool isImmutable = bufferUsage == BufferUsage::STATIC;
    bool mustUseInitialData = isImmutable || isMultiSampled;

    HRESULT hr = _rhi_device->GetDxDevice()->CreateTexture2D(&tex_desc, (mustUseInitialData ? &subresource_data : nullptr), &dx_tex);
    bool succeeded = SUCCEEDED(hr);
    if(succeeded) {
        return new Texture2D(_rhi_device, dx_tex);
    } else {
        return nullptr;
    }
}

Texture* Renderer::Create3DTexture(const std::string& filepath, const IntVector3& dimensions, const BufferUsage& bufferUsage, const BufferBindUsage& bindUsage, const ImageFormat& imageFormat) {
    namespace FS = std::experimental::filesystem;
    FS::path p(filepath);
    if(!FS::exists(p)) {
        return GetTexture("__invalid");
    }

    D3D11_TEXTURE3D_DESC tex_desc;
    memset(&tex_desc, 0, sizeof(tex_desc));

    tex_desc.Width = dimensions.x;     // width... 
    tex_desc.Height = dimensions.y;    // ...and height of image in pixels.
    tex_desc.Depth = dimensions.z;
    tex_desc.MipLevels = 1;    // setting to 0 means there's a full chain (or can generate a full chain) - we're immutable, so not allowed
    tex_desc.Usage = BufferUsageToD3DUsage(bufferUsage);            // data is set at creation time and won't change
    tex_desc.Format = ImageFormatToDxgiFormat(imageFormat);      // R8G8B8A8 texture
    tex_desc.BindFlags = BufferBindUsageToD3DBindFlags(bindUsage);   // we're going to be using this texture as a shader resource
                                                                     //Make every texture a shader resource
    tex_desc.BindFlags |= BufferBindUsageToD3DBindFlags(BufferBindUsage::SHADER_RESOURCE);
    tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(bufferUsage);                      // Determines how I can access this resource CPU side (IMMUTABLE, So none)
    tex_desc.MiscFlags = 0;                            // Extra Flags, of note is;
                                                       // D3D11_RESOURCE_MISC_GENERATE_MIPS - if we want to use this to be able to generate mips (not compatible with IMMUTABLE)

                                                       // If Multisampling - set this up - we're not multisampling, so 1 and 0
                                                       // (MSAA as far as I know only makes sense for Render Targets, not shader resource textures)

                                                       // Setup Initial Data
    D3D11_SUBRESOURCE_DATA subresource_data;
    memset(&subresource_data, 0, sizeof(subresource_data));

    std::vector<unsigned char> data;
    FileUtils::ReadBufferFromFile(data, filepath);
    auto width = dimensions.x;
    auto height = dimensions.y;
    subresource_data.pSysMem = data.data();
    subresource_data.SysMemPitch = width * sizeof(unsigned int); // pitch is byte size of a single row)
    subresource_data.SysMemSlicePitch = width * height * sizeof(unsigned int);
    //Force specific usages for unordered access
    if((bindUsage & BufferBindUsage::UNORDERED_ACCESS) == BufferBindUsage::UNORDERED_ACCESS) {
        tex_desc.Usage = BufferUsageToD3DUsage(BufferUsage::GPU);
        tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(BufferUsage::STAGING);
    }
    ID3D11Texture3D* dx_tex = nullptr;

    //If IMMUTABLE or not multi-sampled, must use initial data.
    bool isMultiSampled = false;
    bool isImmutable = bufferUsage == BufferUsage::STATIC;
    bool mustUseInitialData = isImmutable || !isMultiSampled;

    HRESULT hr = _rhi_device->GetDxDevice()->CreateTexture3D(&tex_desc, (mustUseInitialData ? &subresource_data : nullptr), &dx_tex);
    bool succeeded = SUCCEEDED(hr);
    if(succeeded) {
        auto* tex = new Texture3D(_rhi_device, dx_tex);
#ifdef RENDER_DEBUG
        tex->SetDebugName(p.string().c_str());
#endif
        tex->IsLoaded(true);
        if(RegisterTexture(p.string(), tex)) {
            return tex;
        } else {
            delete tex;
            return nullptr;
        }
    } else {
        return nullptr;
    }
}

Texture* Renderer::Create3DTextureFromMemory(const unsigned char* data, unsigned int width /*= 1*/, unsigned int height /*= 1*/, unsigned int depth /*= 1*/, const BufferUsage& bufferUsage /*= BufferUsage::STATIC*/, const BufferBindUsage& bindUsage /*= BufferBindUsage::SHADER_RESOURCE*/, const ImageFormat& imageFormat /*= ImageFormat::R8G8B8A8_UNORM*/) {
    D3D11_TEXTURE3D_DESC tex_desc = {};

    tex_desc.Width = width;
    tex_desc.Height = height;
    tex_desc.Depth = depth;
    tex_desc.MipLevels = 1;
    tex_desc.Usage = BufferUsageToD3DUsage(bufferUsage);
    tex_desc.Format = ImageFormatToDxgiFormat(imageFormat);
    tex_desc.BindFlags = BufferBindUsageToD3DBindFlags(bindUsage);
    //Make every texture a target and shader resource
    tex_desc.BindFlags |= BufferBindUsageToD3DBindFlags(BufferBindUsage::SHADER_RESOURCE);
    tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(bufferUsage);
    //Force specific usages for unordered access
    if(bindUsage == BufferBindUsage::UNORDERED_ACCESS) {
        tex_desc.Usage = BufferUsageToD3DUsage(BufferUsage::GPU);
        tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(BufferUsage::STAGING);
    }
    tex_desc.MiscFlags = 0;

    // Setup Initial Data
    D3D11_SUBRESOURCE_DATA subresource_data = {};

    subresource_data.pSysMem = data;
    subresource_data.SysMemPitch = width * sizeof(unsigned char);
    subresource_data.SysMemSlicePitch = width * height * sizeof(unsigned char);

    ID3D11Texture3D* dx_tex = nullptr;

    //If IMMUTABLE or not multi-sampled, must use initial data.
    bool isMultiSampled = false;
    bool isImmutable = bufferUsage == BufferUsage::STATIC;
    bool mustUseInitialData = isImmutable || !isMultiSampled;

    HRESULT hr = _rhi_device->GetDxDevice()->CreateTexture3D(&tex_desc, (mustUseInitialData ? &subresource_data : nullptr), &dx_tex);
    bool succeeded = SUCCEEDED(hr);
    if(succeeded) {
        return new Texture3D(_rhi_device, dx_tex);
    } else {
        return nullptr;
    }
}

Texture* Renderer::Create3DTextureFromMemory(const std::vector<Rgba>& data, unsigned int width /*= 1*/, unsigned int height /*= 1*/, unsigned int depth /*= 1*/, const BufferUsage& bufferUsage /*= BufferUsage::STATIC*/, const BufferBindUsage& bindUsage /*= BufferBindUsage::SHADER_RESOURCE*/, const ImageFormat& imageFormat /*= ImageFormat::R8G8B8A8_UNORM*/) {
    D3D11_TEXTURE3D_DESC tex_desc = {};

    tex_desc.Width = width;
    tex_desc.Height = height;
    tex_desc.Depth = depth;
    tex_desc.MipLevels = 1;
    tex_desc.Usage = BufferUsageToD3DUsage(bufferUsage);
    tex_desc.Format = ImageFormatToDxgiFormat(imageFormat);
    tex_desc.BindFlags = BufferBindUsageToD3DBindFlags(bindUsage);
    //Make every texture a target and shader resource
    tex_desc.BindFlags |= BufferBindUsageToD3DBindFlags(BufferBindUsage::SHADER_RESOURCE);
    tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(bufferUsage);
    //Force specific usages for unordered access
    if(bindUsage == BufferBindUsage::UNORDERED_ACCESS) {
        tex_desc.Usage = BufferUsageToD3DUsage(BufferUsage::GPU);
        tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(BufferUsage::STAGING);
    }
    tex_desc.MiscFlags = 0;

    // Setup Initial Data
    D3D11_SUBRESOURCE_DATA subresource_data = {};

    subresource_data.pSysMem = data.data();
    subresource_data.SysMemPitch = width * sizeof(Rgba);
    subresource_data.SysMemSlicePitch = width * height * sizeof(Rgba);

    ID3D11Texture3D* dx_tex = nullptr;

    //If IMMUTABLE or not multi-sampled, must use initial data.
    bool isMultiSampled = false;
    bool isImmutable = bufferUsage == BufferUsage::STATIC;
    bool mustUseInitialData = isImmutable || !isMultiSampled;

    HRESULT hr = _rhi_device->GetDxDevice()->CreateTexture3D(&tex_desc, (mustUseInitialData ? &subresource_data : nullptr), &dx_tex);
    bool succeeded = SUCCEEDED(hr);
    if(succeeded) {
        return new Texture3D(_rhi_device, dx_tex);
    } else {
        return nullptr;
    }
}
