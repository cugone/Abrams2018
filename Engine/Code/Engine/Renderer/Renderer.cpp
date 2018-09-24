#include "Engine/Renderer/Renderer.hpp"

#include "Engine/Core/BuildConfig.cpp"
#include "Engine/Core/DataUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/KerningFont.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Vertex3D.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Frustum.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vector2.hpp"

#include "Engine/RHI/RHIInstance.hpp"
#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"
#include "Engine/RHI/RHIOutput.hpp"

#include "Engine/Renderer/AnimatedSprite.hpp"
#include "Engine/Renderer/Camera3D.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Renderer/DepthStencilState.hpp"
#include "Engine/Renderer/InputLayout.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/RasterState.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/StructuredBuffer.hpp"
#include "Engine/Renderer/TextureArray2D.hpp"
#include "Engine/Renderer/Texture1D.hpp"
#include "Engine/Renderer/Texture2D.hpp"
#include "Engine/Renderer/Texture3D.hpp"
#include "Engine/Renderer/Window.hpp"

#include "Thirdparty/stb/stb_image.h"
#include "Thirdparty/TinyXML2/tinyxml2.h"

#include <numeric>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <sstream>

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

    for(auto& font : _fonts) {
        delete font.second;
        font.second = nullptr;
    }
    _fonts.clear();

    for(auto& ds : _depthstencils) {
        delete ds.second;
        ds.second = nullptr;
    }
    _depthstencils.clear();

    delete _temp_vbo;
    _temp_vbo = nullptr;

    delete _temp_ibo;
    _temp_ibo = nullptr;

    delete _default_depthstencil;
    _default_depthstencil = nullptr;

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

    {
    VertexBuffer::buffer_t default_vbo(1024);
    IndexBuffer::buffer_t default_ibo(1024);
    _temp_vbo = _rhi_device->CreateVertexBuffer(default_vbo, BufferUsage::Dynamic, BufferBindUsage::Vertex_Buffer);
    _temp_ibo = _rhi_device->CreateIndexBuffer(default_ibo, BufferUsage::Dynamic, BufferBindUsage::Index_Buffer);
    _current_vbo_size = default_vbo.size();
    _current_ibo_size = default_ibo.size();
    }

    _matrix_cb = _rhi_device->CreateConstantBuffer(&_matrix_data, sizeof(_matrix_data), BufferUsage::Dynamic, BufferBindUsage::Constant_Buffer);
    _time_cb = _rhi_device->CreateConstantBuffer(&_time_data, sizeof(_time_data), BufferUsage::Dynamic, BufferBindUsage::Constant_Buffer);

    CreateAndRegisterDefaultDepthStencilStates();
    CreateAndRegisterDefaultSamplers();
    CreateAndRegisterDefaultRasterStates();
    CreateAndRegisterDefaultTextures();
    CreateAndRegisterDefaultShaderPrograms();
    CreateAndRegisterDefaultShaders();
    CreateAndRegisterDefaultMaterials();

    _default_depthstencil = CreateDepthStencil(_rhi_device, _window_dimensions);
    SetDepthStencilState(GetDepthStencilState("__default"));
    SetRasterState(GetRasterState("__solid"));
    SetSampler(GetSampler("__default"));
    _current_material = nullptr; //Explicit to avoid defaulting to full lighting material.
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
    namespace FS = std::filesystem;
    FS::path p(name);
    p.make_preferred();
    auto found_texture = _textures.find(p.string());
    if(found_texture == _textures.end()) {
        _textures.insert_or_assign(name, texture);
        return true;
    } else {
        return false;
    }
}

Texture* Renderer::GetTexture(const std::string& nameOrFile) {
    namespace FS = std::filesystem;
    FS::path p(nameOrFile);
    p.make_preferred();
    auto found_iter = _textures.find(p.string());
    if(found_iter == _textures.end()) {
        return nullptr;
    }
    return _textures[p.string()];
}

void Renderer::DrawPoint(const Vertex3D& point) {
    std::vector<Vertex3D> vbo = { point };
    std::vector<unsigned int> ibo = { 0 };
    DrawIndexed(PrimitiveType::Points, vbo, ibo);
}

void Renderer::DrawPoint(const Vector3& point, const Rgba& color /*= Rgba::WHITE*/, const Vector2& tex_coords /*= Vector2::ZERO*/) {
    DrawPoint(Vertex3D(point, color, tex_coords));
}

void Renderer::DrawFrustum(const Frustum& frustum, const Rgba& color /*= Rgba::YELLOW*/, const Vector2& tex_coords /*= Vector2::ZERO*/) {

    const Vector3& point1{frustum.GetNearBottomLeft()};
    const Vector3& point2{frustum.GetNearTopLeft()};
    const Vector3& point3{frustum.GetNearTopRight()};
    const Vector3& point4{frustum.GetNearBottomRight()};
    const Vector3& point5{frustum.GetFarBottomLeft()};
    const Vector3& point6{frustum.GetFarTopLeft()};
    const Vector3& point7{frustum.GetFarTopRight()};
    const Vector3& point8{frustum.GetFarBottomRight()};
    std::vector<Vertex3D> vbo{
        Vertex3D{point1, color, tex_coords}, //Near
        Vertex3D{point2, color, tex_coords},
        Vertex3D{point3, color, tex_coords},
        Vertex3D{point4, color, tex_coords},
        Vertex3D{point5, color, tex_coords}, //Far
        Vertex3D{point6, color, tex_coords},
        Vertex3D{point7, color, tex_coords},
        Vertex3D{point8, color, tex_coords},
    };
    std::vector<unsigned int> ibo{
        0, 1, 1, 2, 2, 3, 3, 0, //Near
        4, 5, 5, 6, 6, 7, 7, 4, //Far
        0, 4, 1, 5, 2, 6, 3, 7, //Edges
    };
    DrawIndexed(PrimitiveType::Lines, vbo, ibo);
}

void Renderer::DrawWorldGridXZ(float radius /*= 500.0f*/, float major_gridsize /*= 20.0f*/, float minor_gridsize /*= 5.0f*/, const Rgba& major_color /*= Rgba::WHITE*/, const Rgba& minor_color /*= Rgba::DARK_GRAY*/) {
    static std::vector<Vertex3D> vbo;
    float half_length = radius;
    float length = radius * 2.0f;
    float space_between_majors = length * (major_gridsize / length);
    float space_between_minors = length * (minor_gridsize / length);
    vbo.clear();
    vbo.reserve(4 * static_cast<std::size_t>(std::ceil(length / minor_gridsize)) - static_cast<std::size_t>(major_gridsize));
    //MAJOR LINES
    for(float x = -half_length; x < half_length + 1.0f; x += space_between_majors) {
        vbo.push_back(Vertex3D(Vector3(x, 0.0f, -half_length), major_color));
        vbo.push_back(Vertex3D(Vector3(x, 0.0f, half_length), major_color));
    }
    for(float z = -half_length; z < half_length + 1.0f; z += space_between_majors) {
        vbo.push_back(Vertex3D(Vector3(-half_length, 0.0f, z), major_color));
        vbo.push_back(Vertex3D(Vector3(half_length, 0.0f, z), major_color));
    }
    //MINOR LINES
    for(float x = -half_length; x < half_length; x += space_between_minors) {
        if(MathUtils::IsEquivalent(std::fmod(x, space_between_majors), 0.0f)) {
            continue;
        }
        vbo.push_back(Vertex3D(Vector3(x, 0.0f, -half_length), minor_color));
        vbo.push_back(Vertex3D(Vector3(x, 0.0f, half_length), minor_color));
    }
    for(float z = -half_length; z < half_length; z += space_between_minors) {
        if(MathUtils::IsEquivalent(std::fmod(z, space_between_majors), 0.0f)) {
            continue;
        }
        vbo.push_back(Vertex3D(Vector3(-half_length, 0.0f, z), minor_color));
        vbo.push_back(Vertex3D(Vector3(half_length, 0.0f, z), minor_color));
    }

    static std::vector<unsigned int> ibo{};
    ibo.resize(vbo.size());
    std::iota(std::begin(ibo), std::end(ibo), 0);

    SetModelMatrix(Matrix4::GetIdentity());
    SetMaterial(GetMaterial("__unlit"));
    std::size_t major_count = ibo.empty() ? 0 : static_cast<std::size_t>(major_gridsize);
    std::size_t major_start = 0;
    std::size_t minor_count = ibo.empty() ? 0 : ibo.size() - static_cast<std::size_t>(minor_gridsize);
    std::size_t minor_start = ibo.empty() ? 0 : major_count;
    DrawIndexed(PrimitiveType::Lines, vbo, ibo, major_count, major_start);
    DrawIndexed(PrimitiveType::Lines, vbo, ibo, minor_count, minor_start);

}

void Renderer::DrawWorldGridXY(float radius /*= 500.0f*/, float major_gridsize /*= 20.0f*/, float minor_gridsize /*= 5.0f*/, const Rgba& major_color /*= Rgba::WHITE*/, const Rgba& minor_color /*= Rgba::DARK_GRAY*/) {

    float half_length = radius;
    float length = radius * 2.0f;
    float space_between_majors = std::floor(length * (major_gridsize / length));
    float space_between_minors = std::floor(length * (minor_gridsize / length));
    std::vector<Vertex3D> major_vbo{};
    //MAJOR LINES
    for(float x = -half_length; x < half_length + 1.0f; x += space_between_majors) {
        major_vbo.push_back(Vertex3D(Vector3(x, -half_length, 0.0f), major_color));
        major_vbo.push_back(Vertex3D(Vector3(x, half_length, 0.0f), major_color));
    }
    for(float y = -half_length; y < half_length + 1.0f; y += space_between_majors) {
        major_vbo.push_back(Vertex3D(Vector3(-half_length, y, 0.0f), major_color));
        major_vbo.push_back(Vertex3D(Vector3(half_length, y, 0.0f), major_color));
    }
    bool major_minor_are_same_size = MathUtils::IsEquivalent(major_gridsize, minor_gridsize);
    bool has_minors = !major_minor_are_same_size;
    std::vector<Vertex3D> minor_vbo{};
    if(has_minors) {
        //MINOR LINES
        for(float x = -half_length; x < half_length; x += space_between_minors) {
            if(MathUtils::IsEquivalent(std::fmod(x, space_between_majors), 0.0f)) {
                continue;
            }
            minor_vbo.push_back(Vertex3D(Vector3(x, -half_length, 0.0f), minor_color));
            minor_vbo.push_back(Vertex3D(Vector3(x, half_length, 0.0f), minor_color));
        }
        for(float y = -half_length; y < half_length; y += space_between_minors) {
            if(MathUtils::IsEquivalent(std::fmod(y, space_between_majors), 0.0f)) {
                continue;
            }
            minor_vbo.push_back(Vertex3D(Vector3(-half_length, y, 0.0f), minor_color));
            minor_vbo.push_back(Vertex3D(Vector3(half_length, y, 0.0f), minor_color));
        }
    }

    std::vector<unsigned int> ibo{};
    ibo.resize(major_vbo.size() + minor_vbo.size());
    std::iota(std::begin(ibo), std::begin(ibo) + major_vbo.size(), 0);
    std::iota(std::begin(ibo) + major_vbo.size(), std::begin(ibo) + major_vbo.size() + minor_vbo.size(), major_vbo.size());

    SetModelMatrix(Matrix4::GetIdentity());
    SetMaterial(GetMaterial("__unlit"));
    std::size_t major_start = 0;
    std::size_t major_count = major_vbo.size();
    std::size_t minor_start = major_vbo.size();
    std::size_t minor_count = minor_vbo.size();
    static std::vector<Vertex3D> vbo;
    vbo.clear();
    auto new_capacity = static_cast<std::size_t>(std::ceil(length / minor_gridsize));
    vbo.reserve(4 * new_capacity);
    vbo.insert(std::end(vbo), std::begin(major_vbo), std::end(major_vbo));
    vbo.insert(std::end(vbo), std::begin(minor_vbo), std::end(minor_vbo));
    DrawIndexed(PrimitiveType::Lines, vbo, ibo, major_count, major_start);
    DrawIndexed(PrimitiveType::Lines, vbo, ibo, minor_count, minor_start);
}

void Renderer::DrawAxes(float maxlength /*= 1000.0f*/, bool disable_unit_depth /*= true*/) {
    static std::vector<Vertex3D> vbo{
        Vertex3D{Vector3::ZERO, Rgba::RED},
        Vertex3D{Vector3::ZERO, Rgba::GREEN},
        Vertex3D{Vector3::ZERO, Rgba::BLUE},
        Vertex3D{Vector3::X_AXIS * maxlength, Rgba::RED},
        Vertex3D{Vector3::Y_AXIS * maxlength, Rgba::GREEN},
        Vertex3D{Vector3::Z_AXIS * maxlength, Rgba::BLUE},
        Vertex3D{Vector3::X_AXIS, Rgba::RED},
        Vertex3D{Vector3::Y_AXIS, Rgba::GREEN},
        Vertex3D{Vector3::Z_AXIS, Rgba::BLUE},
    };
    static std::vector<unsigned int> ibo{
        0, 3, 1, 4, 2, 5,
        0, 6, 1, 7, 2, 8
    };
    SetModelMatrix(Matrix4::GetIdentity());
    SetMaterial(GetMaterial("__unlit"));
    DrawIndexed(PrimitiveType::Lines, vbo, ibo, 6, 0);
    if(disable_unit_depth) {
        DisableDepth();
    }
    DrawIndexed(PrimitiveType::Lines, vbo, ibo, 6, 6);
}

void Renderer::DrawDebugSphere(float radius, const Rgba& color) {
    int num_sides = 64;
    auto num_sides_as_float = static_cast<float>(num_sides);
    std::vector<Vector3> verts;
    verts.reserve(num_sides * 3);
    float anglePerVertex = 360.0f / num_sides_as_float;
    for(float degrees = 0.0f; degrees < 360.0f; degrees += anglePerVertex) {
        float radians = MathUtils::ConvertDegreesToRadians(degrees);
        float pX = radius * std::cos(radians);
        float pY = radius * std::sin(radians);
        verts.push_back(Vector3(Vector2(pX, pY), 0.0f));
    }
    for(float degrees = 0.0f; degrees < 360.0f; degrees += anglePerVertex) {
        float radians = MathUtils::ConvertDegreesToRadians(degrees);
        float pX = radius * std::cos(radians);
        float pY = radius * std::sin(radians);
        verts.push_back(Vector3(pX, 0.0f, pY));
    }
    for(float degrees = 0.0f; degrees < 360.0f; degrees += anglePerVertex) {
        float radians = MathUtils::ConvertDegreesToRadians(degrees);
        float pX = radius * std::cos(radians);
        float pY = radius * std::sin(radians);
        verts.push_back(Vector3(0.0f, pX, pY));
    }

    std::vector<Vertex3D> vbo{};
    vbo.resize(verts.size());
    for(std::size_t i = 0; i < vbo.size(); ++i) {
        vbo[i] = Vertex3D(verts[i], color);
    }

    std::vector<unsigned int> ibo;
    ibo.resize(num_sides * 3);
    auto first_start  = ibo.begin() + 0;
    auto first_end    = ibo.begin() + num_sides;
    auto first_value = 0;
    auto second_start = ibo.begin() + num_sides;
    auto second_end   = ibo.begin() + num_sides * 2;
    auto second_value = num_sides;
    auto third_start  = ibo.begin() + num_sides * 2;
    auto third_end    = ibo.begin() + num_sides * 3;
    auto third_value = num_sides * 2;
    std::iota(first_start, first_end, first_value);
    std::iota(second_start, second_end, second_value);
    std::iota(third_start, third_end, third_value);
    DrawIndexed(PrimitiveType::LinesStrip, vbo, ibo, num_sides, first_value);
    DrawIndexed(PrimitiveType::LinesStrip, vbo, ibo, num_sides, second_value);
    DrawIndexed(PrimitiveType::LinesStrip, vbo, ibo, num_sides, third_value);
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

void Renderer::DrawIndexed(const PrimitiveType& topology, const std::vector<Vertex3D>& vbo, const std::vector<unsigned int>& ibo, std::size_t vertex_count, std::size_t startVertex /*= 0*/, std::size_t baseVertexLocation /*= 0*/) {
    UpdateVbo(vbo);
    UpdateIbo(ibo);
    DrawIndexed(topology, _temp_vbo, _temp_ibo, vertex_count, startVertex, baseVertexLocation);
}

AnimatedSprite* Renderer::CreateAnimatedSprite(const std::string& filepath) {
    namespace FS = std::filesystem;
    FS::path p(filepath);
    p.make_preferred();
    tinyxml2::XMLDocument doc;
    auto xml_result = doc.LoadFile(p.string().c_str());
    if(xml_result == tinyxml2::XML_SUCCESS) {
        auto xml_root = doc.RootElement();
        return new AnimatedSprite(*this, *xml_root);
    }
    if(p.has_extension() && StringUtils::ToLowerCase(p.extension().string()) == ".gif") {
        return CreateAnimatedSpriteFromGif(filepath);
    }
    return nullptr;
}

AnimatedSprite* Renderer::CreateAnimatedSprite(const XMLElement& elem) {
    return new AnimatedSprite(*this, elem);
}

SpriteSheet* Renderer::CreateSpriteSheet(const XMLElement& elem) {
    return new SpriteSheet(*this, elem);
}

SpriteSheet* Renderer::CreateSpriteSheet(const std::string& filepath, unsigned int width /*= 1*/, unsigned int height /*= 1*/) {
    namespace FS = std::filesystem;
    FS::path p(filepath);
    p.make_preferred();
    if(!FS::exists(p)) {
        DebuggerPrintf((p.string() + " not found.\n").c_str());
        return nullptr;
    }
    if(StringUtils::ToLowerCase(p.extension().string()) == ".gif") {
        return CreateSpriteSheetFromGif(p.string());
    }
    tinyxml2::XMLDocument doc;
    auto xml_load = doc.LoadFile(p.string().c_str());
    if(xml_load == tinyxml2::XML_SUCCESS) {
        auto xml_root = doc.RootElement();
        return CreateSpriteSheet(*xml_root);
    }
    return new SpriteSheet(*this, filepath, width, height);
}

SpriteSheet* Renderer::CreateSpriteSheetFromGif(const std::string& filepath) {
    namespace FS = std::filesystem;
    FS::path p(filepath);
    p.make_preferred();
    if(StringUtils::ToLowerCase(p.extension().string()) != ".gif") {
        return nullptr;
    }
    Image img(p.string());
    auto delays = img.GetDelaysIfGif();
    auto tex = GetTexture(p.string());
    auto spr = new SpriteSheet(tex, 1, delays.size());
    tex = nullptr;
    return spr;
}

AnimatedSprite* Renderer::CreateAnimatedSpriteFromGif(const std::string& filepath) {
    namespace FS = std::filesystem;
    FS::path p(filepath);
    p.make_preferred();
    if(StringUtils::ToLowerCase(p.extension().string()) != ".gif") {
        return nullptr;
    }
    Image img(p.string());
    auto delays = img.GetDelaysIfGif();
    auto tex = GetTexture(p.string());
    auto spr = new SpriteSheet(tex, 1, delays.size());
    float duration_sum = 0.0f;
    for(auto i : delays) {
        duration_sum += (i * 0.001f);
    }
    auto anim = new AnimatedSprite(*this, spr, duration_sum, 0, delays.size());
    tinyxml2::XMLDocument doc;
    std::ostringstream ss;
    ss << R"("<material name="__Gif_)" << p.stem().string() << R"("><shader src="__2D" /><textures><diffuse src=")" << p.string() << R"(" /></textures></material>)";
    doc.Parse(ss.str().c_str());
    auto anim_mat = new Material(this, *doc.RootElement());
    anim->SetMaterial(anim_mat);
    RegisterMaterial(anim_mat);
    tex = nullptr;
    return anim;
}

void Renderer::Draw(const PrimitiveType& topology, VertexBuffer* vbo, std::size_t vertex_count) {
    GUARANTEE_OR_DIE(_current_material, "Attempting to call Draw function without a material set!\n");
    D3D11_PRIMITIVE_TOPOLOGY d3d_prim = PrimitiveTypeToD3dTopology(topology);
    _rhi_context->GetDxContext()->IASetPrimitiveTopology(d3d_prim);
    unsigned int stride = sizeof(VertexBuffer::arraybuffer_t);
    unsigned int offsets = 0;
    ID3D11Buffer* dx_vbo_buffer = vbo->GetDxBuffer();
    _rhi_context->GetDxContext()->IASetVertexBuffers(0, 1, &dx_vbo_buffer, &stride, &offsets);
    _rhi_context->Draw(vertex_count);
}

void Renderer::DrawIndexed(const PrimitiveType& topology, VertexBuffer* vbo, IndexBuffer* ibo, std::size_t index_count, std::size_t startVertex /*= 0*/, std::size_t baseVertexLocation /*= 0*/) {
    GUARANTEE_OR_DIE(_current_material, "Attempting to call Draw function without a material set!\n");
    D3D11_PRIMITIVE_TOPOLOGY d3d_prim = PrimitiveTypeToD3dTopology(topology);
    _rhi_context->GetDxContext()->IASetPrimitiveTopology(d3d_prim);
    unsigned int stride = sizeof(VertexBuffer::arraybuffer_t);
    unsigned int offsets = 0;
    ID3D11Buffer* dx_vbo_buffer = vbo->GetDxBuffer();
    ID3D11Buffer* dx_ibo_buffer = ibo->GetDxBuffer();
    _rhi_context->GetDxContext()->IASetVertexBuffers(0, 1, &dx_vbo_buffer, &stride, &offsets);
    _rhi_context->GetDxContext()->IASetIndexBuffer(dx_ibo_buffer, DXGI_FORMAT_R32_UINT, offsets);
    _rhi_context->DrawIndexed(index_count, startVertex, baseVertexLocation);
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
    DrawIndexed(PrimitiveType::Points, vbo, ibo);
}
void Renderer::DrawPoint2D(const Vector2& point, const Rgba& color /*= Rgba::WHITE*/) {
    DrawPoint2D(point.x, point.y, color);
}

void Renderer::DrawLine2D(float startX, float startY, float endX, float endY, const Rgba& color /*= Rgba::WHITE*/, float thickness /*= 0.0f*/) {
    bool use_thickness = thickness > 0.0f;
    if(!use_thickness) {
        Vertex3D start = Vertex3D(Vector3(Vector2(startX, startY), 0.0f), color, Vector2::ZERO);
        Vertex3D end = Vertex3D(Vector3(Vector2(endX, endY), 0.0f), color, Vector2::ONE);
        std::vector<Vertex3D> vbo = {
            start
            , end
        };
        std::vector<unsigned int> ibo = {
            0, 1
        };
        DrawIndexed(PrimitiveType::Lines, vbo, ibo);
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
        DrawQuad2D(Vector2(start + direction * length * 0.5f), Vector2(displacement * 0.5f), color);
    }
}

void Renderer::DrawLine2D(const Vector2& start, const Vector2& end, const Rgba& color /*= Rgba::WHITE*/, float thickness /*= 0.0f*/) {
    DrawLine2D(start.x, start.y, end.x, end.y, color, thickness);
}

void Renderer::DrawQuad2D(float left, float bottom, float right, float top, const Rgba& color /*= Rgba::WHITE*/, const Vector4& texCoords /*= Vector4::ZW_AXIS*/) {
    Vector3 v_lb = Vector3(left, bottom, 0.0f);
    Vector3 v_rt = Vector3(right, top, 0.0f);
    Vector3 v_lt = Vector3(left, top, 0.0f);
    Vector3 v_rb = Vector3(right, bottom, 0.0f);
    Vector2 uv_lt = Vector2(texCoords.x, texCoords.y);
    Vector2 uv_lb = Vector2(texCoords.x, texCoords.w);
    Vector2 uv_rt = Vector2(texCoords.z, texCoords.y);
    Vector2 uv_rb = Vector2(texCoords.z, texCoords.w);
    std::vector<Vertex3D> vbo = {
        Vertex3D(v_lb, color, uv_lb)
        ,Vertex3D(v_lt, color, uv_lt)
        ,Vertex3D(v_rt, color, uv_rt)
        ,Vertex3D(v_rb, color, uv_rb)
    };
    std::vector<unsigned int> ibo = {
        0, 1, 2
        , 0, 2, 3
    };
    DrawIndexed(PrimitiveType::Triangles, vbo, ibo);

}

void Renderer::DrawQuad2D(const Rgba& color) {
    DrawQuad2D(Vector2::ZERO, Vector2(0.5f, 0.5f), color);
}

void Renderer::DrawQuad2D(const Vector2& position /*= Vector2::ZERO*/, const Vector2& halfExtents /*= Vector2(0.5f, 0.5f)*/, const Rgba& color /*= Rgba::WHITE*/, const Vector4& texCoords /*= Vector4::ZW_AXIS*/) {
    float left = position.x - halfExtents.x;
    float bottom = position.y + halfExtents.y;
    float right = position.x + halfExtents.x;
    float top = position.y - halfExtents.y;
    DrawQuad2D(left, bottom, right, top, color, texCoords);
}

void Renderer::DrawQuad2D(const Vector4& texCoords) {
    DrawQuad2D(Vector2::ZERO, Vector2(0.5f, 0.5f), Rgba::WHITE, texCoords);
}

void Renderer::DrawQuad2D(const Rgba& color, const Vector4& texCoords) {
    DrawQuad2D(Vector2::ZERO, Vector2(0.5f, 0.5f), color, texCoords);
}

void Renderer::DrawCircle2D(float centerX, float centerY, float radius, const Rgba& color /*= Rgba::WHITE*/) {
    DrawPolygon2D(centerX, centerY, radius, 65, color);
}

void Renderer::DrawCircle2D(const Vector2& center, float radius, const Rgba& color /*= Rgba::WHITE*/) {
    DrawCircle2D(center.x, center.y, radius, color);
}

void Renderer::DrawFilledCircle2D(const Vector2& center, float radius, const Rgba& color /*= Rgba::WHITE*/) {

    int num_sides = 65;
    std::vector<Vector3> verts;
    verts.reserve(num_sides + 1);
    float anglePerVertex = 360.0f / static_cast<float>(num_sides);
    for(float degrees = 0.0f; degrees < 360.0f; degrees += anglePerVertex) {
        float radians = MathUtils::ConvertDegreesToRadians(degrees);
        float pX = radius * std::cos(radians) + center.x;
        float pY = radius * std::sin(radians) + center.y;
        verts.emplace_back(Vector2(pX, pY), 0.0f);
    }

    std::vector<Vertex3D> vbo;
    vbo.reserve(verts.size());
    for(std::size_t i = 0; i < verts.size(); ++i) {
        vbo.emplace_back(verts[i], color);
    }

    std::vector<unsigned int> ibo(num_sides * 3);
    unsigned int j = 1;
    for(std::size_t i = 1; i < ibo.size(); i += 3) {
        ibo[i] = (j++);
        ibo[i + 1] = (j);
    }
    DrawIndexed(PrimitiveType::TriangleStrip, vbo, ibo);
}

void Renderer::DrawAABB2(const AABB2& bounds, const Rgba& edgeColor, const Rgba& fillColor, const Vector2& edgeHalfExtents /*= Vector2(0.5f, 0.5f)*/) {
    Vector2 lt_inner(bounds.mins.x, bounds.mins.y);
    Vector2 lb_inner(bounds.mins.x, bounds.maxs.y);
    Vector2 rt_inner(bounds.maxs.x, bounds.mins.y);
    Vector2 rb_inner(bounds.maxs.x, bounds.maxs.y);
    Vector2 lt_outer(bounds.mins.x - edgeHalfExtents.x, bounds.mins.y - edgeHalfExtents.y);
    Vector2 lb_outer(bounds.mins.x - edgeHalfExtents.x, bounds.maxs.y + edgeHalfExtents.y);
    Vector2 rt_outer(bounds.maxs.x + edgeHalfExtents.x, bounds.mins.y - edgeHalfExtents.y);
    Vector2 rb_outer(bounds.maxs.x + edgeHalfExtents.x, bounds.maxs.y + edgeHalfExtents.y);
    std::vector<Vertex3D> vbo = {
        Vertex3D(Vector3(rt_outer, 0.0f), edgeColor),
        Vertex3D(Vector3(lt_outer, 0.0f), edgeColor),
        Vertex3D(Vector3(lt_inner, 0.0f), edgeColor),
        Vertex3D(Vector3(rt_inner, 0.0f), edgeColor),
        Vertex3D(Vector3(rb_outer, 0.0f), edgeColor),
        Vertex3D(Vector3(rb_inner, 0.0f), edgeColor),
        Vertex3D(Vector3(lb_outer, 0.0f), edgeColor),
        Vertex3D(Vector3(lb_inner, 0.0f), edgeColor),
        Vertex3D(Vector3(rt_inner, 0.0f), fillColor),
        Vertex3D(Vector3(lt_inner, 0.0f), fillColor),
        Vertex3D(Vector3(lb_inner, 0.0f), fillColor),
        Vertex3D(Vector3(rb_inner, 0.0f), fillColor),
    };

    std::vector<unsigned int> ibo = {
        8, 9, 10,
        8, 10, 11,
        0, 1, 2,
        0, 2, 3,
        4, 0, 3,
        4, 3, 5,
        6, 4, 5,
        6, 5, 7,
        1, 6, 7,
        1, 7, 2,
    };

    DrawIndexed(PrimitiveType::Triangles, vbo, ibo);
}

void Renderer::DrawAABB2(const Rgba& edgeColor, const Rgba& fillColor) {
    AABB2 bounds;
    bounds.mins = Vector2(-0.5f, -0.5f);
    bounds.maxs = Vector2(0.5f, 0.5f);
    Vector2 edge_half_extents = Vector2(0.5f, 0.5f);
    DrawAABB2(bounds, edgeColor, fillColor, edge_half_extents);
}

void Renderer::DrawX2D(const Vector2& position /*= Vector2::ZERO*/, const Vector2& half_extents /*= Vector2(0.5f, 0.5f)*/, const Rgba& color /*= Rgba::WHITE*/) {
    float left = position.x - half_extents.x;
    float top = position.y - half_extents.y;
    float right = position.x + half_extents.x;
    float bottom = position.y + half_extents.y;
    Vector3 lt = Vector3(left, top, 0.0f);
    Vector3 rt = Vector3(right, top, 0.0f);
    Vector3 lb = Vector3(left, bottom, 0.0f);
    Vector3 rb = Vector3(right, bottom, 0.0f);
    std::vector<Vertex3D> vbo = {
        Vertex3D(lt, color),
        Vertex3D(rb, color),
        Vertex3D(lb, color),
        Vertex3D(rt, color),
    };

    std::vector<unsigned int> ibo = {
        0, 1, 2, 3
    };

    DrawIndexed(PrimitiveType::Lines, vbo, ibo);
}

void Renderer::DrawX2D(const Rgba& color) {
    DrawX2D(Vector2::ZERO, Vector2(0.5f, 0.5f), color);
}

void Renderer::DrawPolygon2D(float centerX, float centerY, float radius, std::size_t numSides /*= 3*/, const Rgba& color /*= Rgba::WHITE*/) {
    auto num_sides_as_float = static_cast<float>(numSides);
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
    DrawIndexed(PrimitiveType::LinesStrip, vbo, ibo);
}

void Renderer::DrawPolygon2D(const Vector2& center, float radius, std::size_t numSides /*= 3*/, const Rgba& color /*= Rgba::WHITE*/) {
    DrawPolygon2D(center.x, center.y, radius, numSides, color);
}

void Renderer::DrawTextLine(KerningFont* font, const std::string& text, const Rgba& color /*= Rgba::WHITE*/, float /*scale*/ /*= 1.0f*/) {
    if(font == nullptr) {
        return;
    }
    //Yes, it's inefficient but I need the size later anyway!
    std::size_t text_size = text.size();
    if(text_size == 0) {
        return;
    }

    float cursor_x = 0.0f;
    float cursor_y = 0.0f;
    float line_top = cursor_y - font->GetCommonDef().base;
    float texture_w = static_cast<float>(font->GetCommonDef().scale.x);
    float texture_h = static_cast<float>(font->GetCommonDef().scale.y);
    std::vector<Vertex3D> vbo;
    vbo.reserve(text_size * 4);
    std::vector<unsigned int> ibo;
    ibo.reserve(text_size * 6);

    for(auto text_iter = text.begin(); text_iter != text.end(); /* DO NOTHING */) {
        KerningFont::CharDef current_def = font->GetCharDef(*text_iter);
        float char_uvl = current_def.position.x / texture_w;
        float char_uvt = current_def.position.y / texture_h;
        float char_uvr = char_uvl + (current_def.dimensions.x / texture_w);
        float char_uvb = char_uvt + (current_def.dimensions.y / texture_h);

        float quad_top = line_top + current_def.offsets.y;
        float quad_bottom = quad_top + current_def.dimensions.y;
        float quad_left = cursor_x - current_def.offsets.x;
        float quad_right = quad_left + current_def.dimensions.x;

        vbo.push_back(Vertex3D(Vector3(quad_left, quad_bottom, 0.0f), color, Vector2(char_uvl, char_uvb)));
        vbo.push_back(Vertex3D(Vector3(quad_left, quad_top, 0.0f), color, Vector2(char_uvl, char_uvt)));
        vbo.push_back(Vertex3D(Vector3(quad_right, quad_top, 0.0f), color, Vector2(char_uvr, char_uvt)));
        vbo.push_back(Vertex3D(Vector3(quad_right, quad_bottom, 0.0f), color, Vector2(char_uvr, char_uvb)));

        unsigned int s = static_cast<unsigned int>(vbo.size());
        ibo.push_back(s - 4);
        ibo.push_back(s - 3);
        ibo.push_back(s - 2);
        ibo.push_back(s - 4);
        ibo.push_back(s - 2);
        ibo.push_back(s - 1);

        auto previous_char = text_iter;
        ++text_iter;
        if(text_iter != text.end()) {
            int kern_value = font->GetKerningValue(*previous_char, *text_iter);
            cursor_x += (current_def.xadvance + kern_value);
        }
    }
    DrawIndexed(PrimitiveType::Triangles, vbo, ibo);
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
    float4 normal : NORMAL;
};

struct ps_in_t {
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : UV;
    float4 normal : NORMAL;
    float3 world_position : WORLD;
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
    float4 normal = input_vertex.normal;
    float4 world = mul(local, g_MODEL);
    float4 view = mul(world, g_VIEW);
    float4 clip = mul(view, g_PROJECTION);

    output.position = clip;
    output.color = input_vertex.color;
    output.uv = input_vertex.uv;
    output.normal = normal;
    output.world_position = world.xyz;

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
    auto normal = offsetof(Vertex3D, normal);
    il->AddElement(pos_offset, ImageFormat::R32G32B32_Float, "POSITION");
    il->AddElement(color_offset, ImageFormat::R8G8B8A8_UNorm, "COLOR");
    il->AddElement(uv_offset, ImageFormat::R32G32_Float, "UV");
    il->AddElement(normal, ImageFormat::R32G32B32_Float, "NORMAL");
    auto vs_bytecode = _rhi_device->CompileShader("__defaultVS", program.data(), program.size(), "VertexFunction", PipelineStage::Vs);
    ID3D11VertexShader* vs = nullptr;
    _rhi_device->GetDxDevice()->CreateVertexShader(vs_bytecode->GetBufferPointer(), vs_bytecode->GetBufferSize(), nullptr, &vs);
    il->CreateInputLayout(vs_bytecode->GetBufferPointer(), vs_bytecode->GetBufferSize());
    auto ps_bytecode = _rhi_device->CompileShader("__defaultPS", program.data(), program.size(), "PixelFunction", PipelineStage::Ps);
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
    il->AddElement(pos_offset, ImageFormat::R32G32B32_Float, "POSITION");
    il->AddElement(color_offset, ImageFormat::R8G8B8A8_UNorm, "COLOR");
    il->AddElement(uv_offset, ImageFormat::R32G32_Float, "UV");
    auto vs_bytecode = _rhi_device->CompileShader("__unlitVS", program.data(), program.size(), "VertexFunction", PipelineStage::Vs);
    ID3D11VertexShader* vs = nullptr;
    _rhi_device->GetDxDevice()->CreateVertexShader(vs_bytecode->GetBufferPointer(), vs_bytecode->GetBufferSize(), nullptr, &vs);
    il->CreateInputLayout(vs_bytecode->GetBufferPointer(), vs_bytecode->GetBufferSize());
    auto ps_bytecode = _rhi_device->CompileShader("__unlitPS", program.data(), program.size(), "PixelFunction", PipelineStage::Ps);
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

    auto mat_2d = CreateDefault2DMaterial();
    RegisterMaterial(mat_2d->GetName(), mat_2d);

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

Material* Renderer::CreateDefault2DMaterial() {
    std::string material =
        R"(
<material name="__2D">
    <shader src="__2D" />
</material>
)";

    tinyxml2::XMLDocument doc;
    auto parse_result = doc.Parse(material.c_str(), material.size());
    if(parse_result != tinyxml2::XML_SUCCESS) {
        return nullptr;
    }
    return new Material(this, *doc.RootElement());

}

Material* Renderer::CreateMaterialFromFont(KerningFont* font) {
    if(font == nullptr) {
        return nullptr;
    }
    namespace FS = std::filesystem;
    FS::path folderpath = font->GetFilePath();
    folderpath = folderpath.parent_path();
    std::string name = font->GetName();
    std::string shader = "__2D";
    std::ostringstream material_stream;
    material_stream << "<material name=\"Font_" << name << "\">";
    material_stream << "<shader src=\"" << shader << "\" />";
    std::size_t image_count = font->GetImagePaths().size();
    bool has_textures = image_count > 0;
    if(has_textures) {
        material_stream << "<textures>";
    }
    bool has_lots_of_textures = has_textures && image_count > 6;
    for(std::size_t i = 0; i < image_count; ++i) {
        FS::path image_path = font->GetImagePaths()[i];
        auto fullpath = folderpath / image_path;
        fullpath.make_preferred();
        switch(i) {
            case 0: material_stream << R"(<diffuse src=")"   << fullpath << R"(" />)"; break;
            case 1: material_stream << R"(<normal src=")"    << fullpath << R"(" />)"; break;
            case 2: material_stream << R"(<lighting src=")"  << fullpath << R"(" />)"; break;
            case 3: material_stream << R"(<specular src=")"  << fullpath << R"(" />)"; break;
            case 4: material_stream << R"(<occlusion src=")" << fullpath << R"(" />)"; break;
            case 5: material_stream << R"(<emissive src=")"  << fullpath << R"(" />)"; break;
            default: /* DO NOTHING */;
        }
        if(i >= 6 && has_lots_of_textures) {
            material_stream << R"(<texture index=")" << (i-6) << R"("\" src=")" << fullpath << R"(" />)";
        }
    }
    if(has_textures) {
        material_stream << "</textures>";
    }
    material_stream << "</material>";
    tinyxml2::XMLDocument doc;
    std::string material_string = material_stream.str();
    auto result = doc.Parse(material_string.c_str(), material_string.size());
    if(result != tinyxml2::XML_SUCCESS) {
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
    RasterState* state = new RasterState(_rhi_device, FillMode::Wireframe, CullMode::Back, false);
    return state;
}

RasterState* Renderer::CreateSolidRaster() {
    RasterState* state = new RasterState(_rhi_device, FillMode::Solid, CullMode::Back, false);
    return state;
}

RasterState* Renderer::CreateWireframeNoCullingRaster() {
    RasterState* state = new RasterState(_rhi_device, FillMode::Wireframe, CullMode::None, false);
    return state;
}

RasterState* Renderer::CreateSolidNoCullingRaster() {
    RasterState* state = new RasterState(_rhi_device, FillMode::Solid, CullMode::None, false);
    return state;
}

RasterState* Renderer::CreateWireframeFrontCullingRaster() {
    RasterState* state = new RasterState(_rhi_device, FillMode::Wireframe, CullMode::Front, false);
    return state;
}

RasterState* Renderer::CreateSolidFrontCullingRaster() {
    RasterState* state = new RasterState(_rhi_device, FillMode::Solid, CullMode::Front, false);
    return state;
}

void Renderer::CreateAndRegisterDefaultDepthStencilStates() {
    DepthStencilState* default_state = CreateDefaultDepthStencilState();
    RegisterDepthStencilState("__default", default_state);
    DepthStencilState* depth_disabled = CreateDisabledDepth();
    RegisterDepthStencilState("__depthdisabled", depth_disabled);
    DepthStencilState* depth_enabled = CreateEnabledDepth();
    RegisterDepthStencilState("__depthenabled", depth_enabled);
}

DepthStencilState* Renderer::CreateDefaultDepthStencilState() {
    DepthStencilDesc desc;
    DepthStencilState* state = new DepthStencilState(_rhi_device, desc);
    return state;
}

DepthStencilState* Renderer::CreateDisabledDepth() {
    DepthStencilDesc desc;
    desc.depth_enabled = false;
    desc.depth_comparison = ComparisonFunction::Always;
    DepthStencilState* state = new DepthStencilState(_rhi_device, desc);
    return state;
}

DepthStencilState* Renderer::CreateEnabledDepth() {
    DepthStencilDesc desc;
    desc.depth_enabled = true;
    desc.depth_comparison = ComparisonFunction::Less;
    DepthStencilState* state = new DepthStencilState(_rhi_device, desc);
    return state;
}

void Renderer::UnbindAllShaderResources() {
    _rhi_context->UnbindAllShaderResources();
}

void Renderer::RegisterDepthStencilState(const std::string& name, DepthStencilState* depthstencil) {
    if(depthstencil == nullptr) {
        return;
    }
    auto found_iter = _depthstencils.find(name);
    if(found_iter != _depthstencils.end()) {
        delete found_iter->second;
        found_iter->second = nullptr;
    }
    _depthstencils.insert_or_assign(name, depthstencil);

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

void Renderer::RegisterFont(const std::string& name, KerningFont* font) {
    if(font == nullptr) {
        return;
    }
    auto found_iter = _fonts.find(name);
    if(found_iter != _fonts.end()) {
        delete found_iter->second;
        found_iter->second = nullptr;
    }
    _fonts.insert_or_assign(name, font);
}

void Renderer::RegisterFont(KerningFont* font) {
    if(font == nullptr) {
        return;
    }
    std::string name = font->GetName();
    auto found_iter = _fonts.find(name);
    if(found_iter != _fonts.end()) {
        delete found_iter->second;
        found_iter->second = nullptr;
    }
    _fonts.insert_or_assign(name, font);
}

bool Renderer::RegisterFont(const std::string& filepath) {
    namespace FS = std::filesystem;
    return RegisterFont(FS::path{ filepath });
}

bool Renderer::RegisterFont(const std::filesystem::path& filepath) {
    auto font = new KerningFont(this);
    std::filesystem::path filepath_copy = filepath;
    filepath_copy.make_preferred();
    if(font->LoadFromFile(filepath_copy.string())) {
        for(auto& texture_filename : font->GetImagePaths()) {
            namespace FS = std::filesystem;
            FS::path folderpath = font->GetFilePath();
            folderpath.make_preferred();
            folderpath = folderpath.parent_path();
            FS::path texture_path = folderpath / FS::path{ texture_filename };
            texture_path.make_preferred();
            CreateTexture(texture_path.string(), IntVector3::XY_AXIS);
        }
        Material* mat = CreateMaterialFromFont(font);
        if(mat) {
            font->SetMaterial(mat);
            RegisterMaterial(mat->GetName(), mat);
            RegisterFont(font->GetName(), font);
            return true;
        }
    }
    delete font;
    font = nullptr;
    return false;
}

void Renderer::RegisterFontsFromFolder(const std::string& folderpath, bool recursive /*= false*/) {
    namespace FS = std::filesystem;
    FS::path path{ folderpath };
    path.make_preferred();
    return RegisterFontsFromFolder(path, recursive);
}

void Renderer::RegisterFontsFromFolder(const std::filesystem::path& folderpath, bool recursive /*= false*/) {
    namespace FS = std::filesystem;
    auto cb =
    [this](const FS::path& p) {
        this->RegisterFont(p);
    };
    FileUtils::IterateFileInFolders(folderpath, ".fnt", cb, recursive);
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

    auto default_2D = CreateDefault2DShader();
    RegisterShader(default_2D->GetName(), default_2D);
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

Shader* Renderer::CreateDefault2DShader() {
std::string shader =
R"(
<shader name = "__2D">
    <shaderprogram src = "__unlit" />
    <raster>
        <fill>solid</fill>
        <cull>none</cull>
        <antialiasing>false</antialiasing>
    </raster>
        <blends>
            <blend enable = "true">
            <color src = "src_alpha" dest = "inv_src_alpha" op = "add" />
        </blend>
    </blends>
    <depth enable = "false" writable = "false" />
    <stencil enable = "false" readable = "false" writable = "false" />
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

void Renderer::RegisterMaterial(const std::string& name, Material* mat) {
    if(mat == nullptr) {
        return;
    }
    auto found_iter = _materials.find(name);
    if(found_iter != _materials.end()) {
        std::ostringstream ss;
        ss << __FUNCTION__ << ": Material \"" << name << "\" already exists. Overwriting.\n";
        DebuggerPrintf(ss.str().c_str());
        delete found_iter->second;
        found_iter->second = nullptr;
    }
    _materials.insert_or_assign(name, mat);
}

void Renderer::RegisterMaterial(Material* mat) {
    if(mat == nullptr) {
        return;
    }
    std::string name = mat->GetName();
    auto found_iter = _materials.find(name);
    if(found_iter != _materials.end()) {
        std::ostringstream ss;
        ss << __FUNCTION__ << ": Material \"" << name << "\" already exists. Overwriting.\n";
        DebuggerPrintf(ss.str().c_str());
        delete found_iter->second;
        found_iter->second = nullptr;
    }
    _materials.insert_or_assign(name, mat);
}

bool Renderer::RegisterMaterial(const std::string& filepath) {
    namespace FS = std::filesystem;
    return RegisterMaterial(FS::path{filepath});
}

bool Renderer::RegisterMaterial(const std::filesystem::path& filepath) {
    namespace FS = std::filesystem;
    std::filesystem::path filepath_copy = filepath;
    tinyxml2::XMLDocument doc;
    if(filepath_copy.has_extension() && filepath.extension() == ".material") {
        filepath_copy.make_preferred();
        const auto p_str = filepath_copy.string();
        if(doc.LoadFile(p_str.c_str()) == tinyxml2::XML_SUCCESS) {
            Material* mat = new Material(this, *doc.RootElement());
            RegisterMaterial(mat->GetName(), mat);
            return true;
        }
    }
    return false;
}

void Renderer::RegisterMaterialsFromFolder(const std::string& folderpath, bool recursive /*= false*/) {
    namespace FS = std::filesystem;
    FS::path path{ folderpath };
    path.make_preferred();
    RegisterMaterialsFromFolder(path, recursive);
}

void Renderer::RegisterMaterialsFromFolder(const std::filesystem::path& folderpath, bool recursive /*= false*/) {
    namespace FS = std::filesystem;
    auto cb =
    [this](const FS::path& p) {
        this->RegisterMaterial(p);
    };
    FileUtils::IterateFileInFolders(folderpath, "", cb, recursive);
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
        _temp_vbo = _rhi_device->CreateVertexBuffer(vbo, BufferUsage::Dynamic, BufferBindUsage::Vertex_Buffer);
        _current_vbo_size = vbo.size();
    }
    _temp_vbo->Update(_rhi_context, vbo);
}

void Renderer::UpdateIbo(const IndexBuffer::buffer_t& ibo) {
    if(_current_ibo_size < ibo.size()) {
        delete _temp_ibo;
        _temp_ibo = _rhi_device->CreateIndexBuffer(ibo, BufferUsage::Dynamic, BufferBindUsage::Index_Buffer);
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
    namespace FS = std::filesystem;
    FS::path p{ nameOrFile };
    p.make_preferred();
    auto found_iter = _shader_programs.find(p.string());
    if(found_iter == _shader_programs.end()) {
        return nullptr;
    }
    return found_iter->second;
}

ShaderProgram* Renderer::CreateShaderProgramFromHlslFile(const std::string& filepath, const std::string& entryPoint, InputLayout* inputLayout, const PipelineStage& target) {
    std::vector<unsigned char> contents;
    if(FileUtils::ReadBufferFromFile(contents, filepath)) {
        std::string data(reinterpret_cast<const char*>(contents.data()), contents.size());
        bool requested_retry = false;
        ShaderProgram* sp = nullptr;
        do {
            sp = _rhi_device->CreateShaderProgramFromHlslString(filepath, data, entryPoint, inputLayout, target);
#ifdef RENDER_DEBUG
            if(sp == nullptr) {
                std::ostringstream error_msg{};
                error_msg << "Shader \"" << filepath << "\" failed to compile.\n";
                error_msg << "See Output window for details.\n";
                error_msg << "Press Retry to re-compile.";
                auto button_id = ::MessageBoxA(GetOutput()->GetWindow()->GetWindowHandle(), error_msg.str().c_str(), "Shader compilation error.", MB_RETRYCANCEL | MB_ICONERROR);
                requested_retry = button_id == IDRETRY;
            }
#endif
        } while(requested_retry);
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

std::size_t Renderer::GetFontCount() const {
    return _fonts.size();
}

KerningFont* Renderer::GetFont(const std::string& nameOrFile) {
    auto found_iter = _fonts.find(nameOrFile);
    if(found_iter == _fonts.end()) {
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
    SetPerspectiveProjection(Vector2{ camera.CalcFovYDegrees(), camera.GetAspectRatio()}, Vector2{ camera.GetNearDistance(), camera.GetFarDistance()});
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

void Renderer::SetViewportAndScissor(unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
    SetScissorAndViewport(x, y, width, height);
}

void Renderer::SetViewports(const std::vector<AABB3>& viewports) {
    std::vector<D3D11_VIEWPORT> dxViewports{};
    dxViewports.resize(viewports.size());

    for(std::size_t i = 0; i < dxViewports.size(); ++i) {
        dxViewports[i].TopLeftX = viewports[i].mins.x;
        dxViewports[i].TopLeftY = viewports[i].mins.y;
        dxViewports[i].Width = viewports[i].maxs.x;
        dxViewports[i].Height = viewports[i].maxs.y;
        dxViewports[i].MinDepth = viewports[i].mins.z;
        dxViewports[i].MaxDepth = viewports[i].maxs.z;
    }
    _rhi_context->GetDxContext()->RSSetViewports(dxViewports.size(), dxViewports.data());
}

void Renderer::SetScissor(unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
    D3D11_RECT scissor;
    scissor.left = x;
    scissor.right = x + width;
    scissor.top = y;
    scissor.bottom = y + height;
    _rhi_context->GetDxContext()->RSSetScissorRects(1, &scissor);
}

void Renderer::SetScissorAndViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
    SetViewport(x, y, width, height);
    SetScissor(x, y, width, height);
}

void Renderer::SetScissors(const std::vector<AABB2>& scissors) {
    std::vector<D3D11_RECT> dxScissors{};
    dxScissors.resize(scissors.size());

    for(std::size_t i = 0; i < scissors.size(); ++i) {
        dxScissors[i].left   = static_cast<long>(scissors[i].mins.x);
        dxScissors[i].top    = static_cast<long>(scissors[i].mins.y);
        dxScissors[i].right  = static_cast<long>(scissors[i].maxs.x);
        dxScissors[i].bottom = static_cast<long>(scissors[i].maxs.y);
    }
    _rhi_context->GetDxContext()->RSSetScissorRects(dxScissors.size(), dxScissors.data());
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

void Renderer::ClearTargetDepthStencilBuffer(Texture* target, bool depth /*= true*/, bool stencil /*= true*/, float depthValue /*= 1.0f*/, unsigned char stencilValue /*= 0*/) {
    _rhi_context->ClearDepthStencilTarget(target, depth, stencil, depthValue, stencilValue);
}

void Renderer::Present() {
    _rhi_output->Present(_vsync);
}

Texture* Renderer::CreateOrGetTexture(const std::string& filepath, const IntVector3& dimensions) {
    namespace FS = std::filesystem;
    FS::path p(filepath);
    p.make_preferred();
    auto texture_iter = _textures.find(p.string());
    if(texture_iter == _textures.end()) {
        return CreateTexture(p.string(), dimensions);
    } else {
        return GetTexture(p.string());
    }
}

void Renderer::RegisterTexturesFromFolder(const std::string& folderpath, bool recursive /*= false*/) {
    namespace FS = std::filesystem;
    FS::path path{ folderpath };
    path.make_preferred();
    RegisterTexturesFromFolder(path, recursive);
}

void Renderer::RegisterTexturesFromFolder(const std::filesystem::path& folderpath, bool recursive /*= false*/) {
    namespace FS = std::filesystem;
    auto cb =
        [this](const FS::path& p) {
        this->RegisterTexture(p);
    };
    FileUtils::IterateFileInFolders(folderpath, std::string{}, cb, recursive);
}

bool Renderer::RegisterTexture(const std::filesystem::path& filepath) {
    namespace FS = std::filesystem;
    const auto p_str = filepath.string();
    Texture* tex = CreateTexture(p_str, IntVector3::XY_AXIS);
    if(tex) {
        return true;
    }
    return false;
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
        return new Texture2D(owner, dx_resource);
    }
    return nullptr;
}

Texture* Renderer::CreateRenderableDepthStencil(RHIDevice* owner, const IntVector2& dimensions) {

    ID3D11Texture2D* dx_resource = nullptr;

    D3D11_TEXTURE2D_DESC descDepth;
    descDepth.Width = dimensions.x;
    descDepth.Height = dimensions.y;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_R32_TYPELESS;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    HRESULT texture_hr = owner->GetDxDevice()->CreateTexture2D(&descDepth, nullptr, &dx_resource);
    bool texture_creation_succeeded = SUCCEEDED(texture_hr);
    if(texture_creation_succeeded) {
        return new Texture2D(owner, dx_resource);
    }
    return nullptr;
}

void Renderer::SetDepthStencilState(DepthStencilState* depthstencil) {
    if(depthstencil == _current_depthstencil_state) {
        return;
    }
    _current_depthstencil_state = depthstencil;
    _rhi_context->SetDepthStencilState(depthstencil);
}

DepthStencilState* Renderer::GetDepthStencilState(const std::string& name) {
    auto found_iter = _depthstencils.find(name);
    if(found_iter == _depthstencils.end()) {
        return nullptr;
    }
    return found_iter->second;
}

void Renderer::EnableDepth() {
    SetDepthStencilState(GetDepthStencilState("__depthenabled"));
}

void Renderer::DisableDepth() {
    SetDepthStencilState(GetDepthStencilState("__depthdisabled"));
}

Texture* Renderer::Create1DTexture(const std::string& filepath, const BufferUsage& bufferUsage, const BufferBindUsage& bindUsage, const ImageFormat& imageFormat) {
    namespace FS = std::filesystem;
    FS::path p(filepath);
    p.make_preferred();
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
    if((bindUsage & BufferBindUsage::Unordered_Access) == BufferBindUsage::Unordered_Access) {
        tex_desc.Usage = BufferUsageToD3DUsage(BufferUsage::Gpu);
        tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(BufferUsage::Staging);
    }
    ID3D11Texture1D* dx_tex = nullptr;

    bool isImmutable = bufferUsage == BufferUsage::Static;
    bool mustUseInitialData = isImmutable;

    HRESULT hr = _rhi_device->GetDxDevice()->CreateTexture1D(&tex_desc, (mustUseInitialData ? &subresource_data : nullptr), &dx_tex);
    bool succeeded = SUCCEEDED(hr);
    if(succeeded) {
        auto* tex = new Texture1D(_rhi_device, dx_tex);
        tex->SetDebugName(p.string().c_str());
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
    tex_desc.BindFlags |= BufferBindUsageToD3DBindFlags(BufferBindUsage::Shader_Resource);
    tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(bufferUsage);
    //Force specific usages for unordered access
    if(bindUsage == BufferBindUsage::Unordered_Access) {
        tex_desc.Usage = BufferUsageToD3DUsage(BufferUsage::Gpu);
        tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(BufferUsage::Staging);
    }
    tex_desc.MiscFlags = 0;

    // Setup Initial Data
    D3D11_SUBRESOURCE_DATA subresource_data = {};

    subresource_data.pSysMem = data;
    subresource_data.SysMemPitch = width * sizeof(unsigned int);
    subresource_data.SysMemSlicePitch = width * sizeof(unsigned int);

    ID3D11Texture1D* dx_tex = nullptr;

    //If IMMUTABLE or not multi-sampled, must use initial data.
    bool isMultiSampled = false;
    bool isImmutable = bufferUsage == BufferUsage::Static;
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
    tex_desc.BindFlags |= BufferBindUsageToD3DBindFlags(BufferBindUsage::Shader_Resource);
    tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(bufferUsage);
    //Force specific usages for unordered access
    if((bindUsage & BufferBindUsage::Unordered_Access) == BufferBindUsage::Unordered_Access) {
        tex_desc.Usage = BufferUsageToD3DUsage(BufferUsage::Gpu);
        tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(BufferUsage::Staging);
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
    bool isImmutable = bufferUsage == BufferUsage::Static;
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
    namespace FS = std::filesystem;
    FS::path p(filepath);
    p.make_preferred();
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
    tex_desc.BindFlags |= BufferBindUsageToD3DBindFlags(BufferBindUsage::Shader_Resource);
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
    if((bindUsage & BufferBindUsage::Unordered_Access) == BufferBindUsage::Unordered_Access) {
        tex_desc.Usage = BufferUsageToD3DUsage(BufferUsage::Gpu);
        tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(BufferUsage::Staging);
    }
    ID3D11Texture2D* dx_tex = nullptr;

    //If IMMUTABLE or not multi-sampled, must use initial data.
    bool isMultiSampled = tex_desc.SampleDesc.Count != 1 || tex_desc.SampleDesc.Quality != 0;
    bool isImmutable = bufferUsage == BufferUsage::Static;
    bool mustUseInitialData = isImmutable || !isMultiSampled;

    HRESULT hr = _rhi_device->GetDxDevice()->CreateTexture2D(&tex_desc, (mustUseInitialData ? &subresource_data : nullptr), &dx_tex);
    bool succeeded = SUCCEEDED(hr);
    if(succeeded) {
        auto tex = new Texture2D(_rhi_device, dx_tex);
        tex->SetDebugName(p.string().c_str());
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
    tex_desc.BindFlags |= BufferBindUsageToD3DBindFlags(BufferBindUsage::Shader_Resource);
    tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(bufferUsage);
    //Force specific usages for unordered access
    if(bindUsage == BufferBindUsage::Unordered_Access) {
        tex_desc.Usage = BufferUsageToD3DUsage(BufferUsage::Gpu);
        tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(BufferUsage::Staging);
    }
    tex_desc.MiscFlags = 0;
    tex_desc.SampleDesc.Count = 1;
    tex_desc.SampleDesc.Quality = 0;

    // Setup Initial Data
    D3D11_SUBRESOURCE_DATA subresource_data = {};

    subresource_data.pSysMem = data;
    subresource_data.SysMemPitch = width * sizeof(unsigned int);
    subresource_data.SysMemSlicePitch = width * height * sizeof(unsigned int);

    ID3D11Texture2D* dx_tex = nullptr;

    //If IMMUTABLE or not multi-sampled, must use initial data.
    bool isMultiSampled = tex_desc.SampleDesc.Count != 1 || tex_desc.SampleDesc.Quality != 0;
    bool isImmutable = bufferUsage == BufferUsage::Static;
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
    tex_desc.BindFlags |= BufferBindUsageToD3DBindFlags(BufferBindUsage::Shader_Resource);
    tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(bufferUsage);
    //Force specific usages for unordered access
    if((bindUsage & BufferBindUsage::Unordered_Access) == BufferBindUsage::Unordered_Access) {
        tex_desc.Usage = BufferUsageToD3DUsage(BufferUsage::Gpu);
        tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(BufferUsage::Staging);
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
    bool isImmutable = bufferUsage == BufferUsage::Static;
    bool mustUseInitialData = isImmutable || isMultiSampled;

    HRESULT hr = _rhi_device->GetDxDevice()->CreateTexture2D(&tex_desc, (mustUseInitialData ? &subresource_data : nullptr), &dx_tex);
    bool succeeded = SUCCEEDED(hr);
    if(succeeded) {
        return new Texture2D(_rhi_device, dx_tex);
    } else {
        return nullptr;
    }
}

Texture* Renderer::Create2DTextureArrayFromMemory(const unsigned char* data, unsigned int width /*= 1*/, unsigned int height /*= 1*/, unsigned int depth /*= 1*/, const BufferUsage& bufferUsage /*= BufferUsage::STATIC*/, const BufferBindUsage& bindUsage /*= BufferBindUsage::SHADER_RESOURCE*/, const ImageFormat& imageFormat /*= ImageFormat::R8G8B8A8_UNORM*/) {
    D3D11_TEXTURE2D_DESC tex_desc = {};

    tex_desc.Width = width;
    tex_desc.Height = height;
    tex_desc.MipLevels = 1;
    tex_desc.ArraySize = depth;
    tex_desc.Usage = BufferUsageToD3DUsage(bufferUsage);
    tex_desc.Format = ImageFormatToDxgiFormat(imageFormat);
    tex_desc.BindFlags = BufferBindUsageToD3DBindFlags(bindUsage);
    //Make every texture a target and shader resource
    tex_desc.BindFlags |= BufferBindUsageToD3DBindFlags(BufferBindUsage::Shader_Resource);
    tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(bufferUsage);
    //Force specific usages for unordered access
    if(bindUsage == BufferBindUsage::Unordered_Access) {
        tex_desc.Usage = BufferUsageToD3DUsage(BufferUsage::Gpu);
        tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(BufferUsage::Staging);
    }
    tex_desc.MiscFlags = 0;
    tex_desc.SampleDesc.Count = 1;
    tex_desc.SampleDesc.Quality = 0;

    // Setup Initial Data
    D3D11_SUBRESOURCE_DATA* subresource_data = new D3D11_SUBRESOURCE_DATA[depth];
    for(unsigned int i = 0; i < depth; ++i) {
        subresource_data[i].pSysMem = data;
        subresource_data[i].SysMemPitch = width * sizeof(unsigned int);
        subresource_data[i].SysMemSlicePitch = width * height * sizeof(unsigned int);
    }
    ID3D11Texture2D* dx_tex = nullptr;

    //If IMMUTABLE or not multi-sampled, must use initial data.
    bool isMultiSampled = tex_desc.SampleDesc.Count != 1 || tex_desc.SampleDesc.Quality != 0;
    bool isImmutable = bufferUsage == BufferUsage::Static;
    bool mustUseInitialData = isImmutable || !isMultiSampled;

    HRESULT hr = _rhi_device->GetDxDevice()->CreateTexture2D(&tex_desc, (mustUseInitialData ? subresource_data : nullptr), &dx_tex);
    delete[] subresource_data;
    subresource_data = nullptr;
    bool succeeded = SUCCEEDED(hr);
    if(succeeded) {
        return new TextureArray2D(_rhi_device, dx_tex);
    } else {
        return nullptr;
    }
}

Texture* Renderer::Create2DTextureFromGifBuffer(const unsigned char* data, unsigned int width /*= 1*/, unsigned int height /*= 1*/, unsigned int depth /*= 1*/, const BufferUsage& bufferUsage /*= BufferUsage::STATIC*/, const BufferBindUsage& bindUsage /*= BufferBindUsage::SHADER_RESOURCE*/, const ImageFormat& imageFormat /*= ImageFormat::R8G8B8A8_UNORM*/) {
    D3D11_TEXTURE2D_DESC tex_desc = {};

    tex_desc.Width = width;
    tex_desc.Height = height;
    tex_desc.MipLevels = 1;
    tex_desc.ArraySize = depth;
    tex_desc.Usage = BufferUsageToD3DUsage(bufferUsage);
    tex_desc.Format = ImageFormatToDxgiFormat(imageFormat);
    tex_desc.BindFlags = BufferBindUsageToD3DBindFlags(bindUsage);
    //Make every texture a target and shader resource
    tex_desc.BindFlags |= BufferBindUsageToD3DBindFlags(BufferBindUsage::Shader_Resource);
    tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(bufferUsage);
    //Force specific usages for unordered access
    if(bindUsage == BufferBindUsage::Unordered_Access) {
        tex_desc.Usage = BufferUsageToD3DUsage(BufferUsage::Gpu);
        tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(BufferUsage::Staging);
    }
    tex_desc.MiscFlags = 0;
    tex_desc.SampleDesc.Count = 1;
    tex_desc.SampleDesc.Quality = 0;

    // Setup Initial Data
    D3D11_SUBRESOURCE_DATA* subresource_data = new D3D11_SUBRESOURCE_DATA[depth];
    for(unsigned int i = 0; i < depth; ++i) {
        subresource_data[i].pSysMem = data;
        subresource_data[i].SysMemPitch = width * sizeof(unsigned int);
        subresource_data[i].SysMemSlicePitch = width * height * sizeof(unsigned int);
    }
    ID3D11Texture2D* dx_tex = nullptr;

    //If IMMUTABLE or not multi-sampled, must use initial data.
    bool isMultiSampled = tex_desc.SampleDesc.Count != 1 || tex_desc.SampleDesc.Quality != 0;
    bool isImmutable = bufferUsage == BufferUsage::Static;
    bool mustUseInitialData = isImmutable || !isMultiSampled;

    HRESULT hr = _rhi_device->GetDxDevice()->CreateTexture2D(&tex_desc, (mustUseInitialData ? subresource_data : nullptr), &dx_tex);
    delete[] subresource_data;
    subresource_data = nullptr;
    bool succeeded = SUCCEEDED(hr);
    if(succeeded) {
        return new Texture2D(_rhi_device, dx_tex);
    } else {
        return nullptr;
    }
}

Texture* Renderer::Create2DTextureArrayFromGifBuffer(const unsigned char* data, unsigned int width /*= 1*/, unsigned int height /*= 1*/, unsigned int depth /*= 1*/, const BufferUsage& bufferUsage /*= BufferUsage::STATIC*/, const BufferBindUsage& bindUsage /*= BufferBindUsage::SHADER_RESOURCE*/, const ImageFormat& imageFormat /*= ImageFormat::R8G8B8A8_UNORM*/) {
    D3D11_TEXTURE2D_DESC tex_desc = {};

    tex_desc.Width = width;
    tex_desc.Height = height;
    tex_desc.MipLevels = 1;
    tex_desc.ArraySize = depth;
    tex_desc.Usage = BufferUsageToD3DUsage(bufferUsage);
    tex_desc.Format = ImageFormatToDxgiFormat(imageFormat);
    tex_desc.BindFlags = BufferBindUsageToD3DBindFlags(bindUsage);
    //Make every texture a target and shader resource
    tex_desc.BindFlags |= BufferBindUsageToD3DBindFlags(BufferBindUsage::Shader_Resource);
    tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(bufferUsage);
    //Force specific usages for unordered access
    if(bindUsage == BufferBindUsage::Unordered_Access) {
        tex_desc.Usage = BufferUsageToD3DUsage(BufferUsage::Gpu);
        tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(BufferUsage::Staging);
    }
    tex_desc.MiscFlags = 0;
    tex_desc.SampleDesc.Count = 1;
    tex_desc.SampleDesc.Quality = 0;

    // Setup Initial Data
    D3D11_SUBRESOURCE_DATA* subresource_data = new D3D11_SUBRESOURCE_DATA[depth];
    for(unsigned int i = 0; i < depth; ++i) {
        subresource_data[i].pSysMem = data;
        subresource_data[i].SysMemPitch = width * sizeof(unsigned int);
        subresource_data[i].SysMemSlicePitch = width * height * sizeof(unsigned int);
    }
    ID3D11Texture2D* dx_tex = nullptr;

    //If IMMUTABLE or not multi-sampled, must use initial data.
    bool isMultiSampled = tex_desc.SampleDesc.Count != 1 || tex_desc.SampleDesc.Quality != 0;
    bool isImmutable = bufferUsage == BufferUsage::Static;
    bool mustUseInitialData = isImmutable || !isMultiSampled;

    HRESULT hr = _rhi_device->GetDxDevice()->CreateTexture2D(&tex_desc, (mustUseInitialData ? subresource_data : nullptr), &dx_tex);
    delete[] subresource_data;
    subresource_data = nullptr;
    bool succeeded = SUCCEEDED(hr);
    if(succeeded) {
        return new TextureArray2D(_rhi_device, dx_tex);
    } else {
        return nullptr;
    }
}

Texture* Renderer::Create3DTexture(const std::string& filepath, const IntVector3& dimensions, const BufferUsage& bufferUsage, const BufferBindUsage& bindUsage, const ImageFormat& imageFormat) {
    namespace FS = std::filesystem;
    FS::path p(filepath);
    p.make_preferred();
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
    tex_desc.BindFlags |= BufferBindUsageToD3DBindFlags(BufferBindUsage::Shader_Resource);
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
    if((bindUsage & BufferBindUsage::Unordered_Access) == BufferBindUsage::Unordered_Access) {
        tex_desc.Usage = BufferUsageToD3DUsage(BufferUsage::Gpu);
        tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(BufferUsage::Staging);
    }
    ID3D11Texture3D* dx_tex = nullptr;

    //If IMMUTABLE or not multi-sampled, must use initial data.
    bool isMultiSampled = false;
    bool isImmutable = bufferUsage == BufferUsage::Static;
    bool mustUseInitialData = isImmutable || !isMultiSampled;

    HRESULT hr = _rhi_device->GetDxDevice()->CreateTexture3D(&tex_desc, (mustUseInitialData ? &subresource_data : nullptr), &dx_tex);
    bool succeeded = SUCCEEDED(hr);
    if(succeeded) {
        auto* tex = new Texture3D(_rhi_device, dx_tex);
        tex->SetDebugName(p.string().c_str());
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
    tex_desc.BindFlags |= BufferBindUsageToD3DBindFlags(BufferBindUsage::Shader_Resource);
    tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(bufferUsage);
    //Force specific usages for unordered access
    if(bindUsage == BufferBindUsage::Unordered_Access) {
        tex_desc.Usage = BufferUsageToD3DUsage(BufferUsage::Gpu);
        tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(BufferUsage::Staging);
    }
    tex_desc.MiscFlags = 0;

    // Setup Initial Data
    D3D11_SUBRESOURCE_DATA subresource_data = {};

    subresource_data.pSysMem = data;
    subresource_data.SysMemPitch = width * sizeof(unsigned int);
    subresource_data.SysMemSlicePitch = width * height * sizeof(unsigned int);

    ID3D11Texture3D* dx_tex = nullptr;

    //If IMMUTABLE or not multi-sampled, must use initial data.
    bool isMultiSampled = false;
    bool isImmutable = bufferUsage == BufferUsage::Static;
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
    tex_desc.BindFlags |= BufferBindUsageToD3DBindFlags(BufferBindUsage::Shader_Resource);
    tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(bufferUsage);
    //Force specific usages for unordered access
    if(bindUsage == BufferBindUsage::Unordered_Access) {
        tex_desc.Usage = BufferUsageToD3DUsage(BufferUsage::Gpu);
        tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(BufferUsage::Staging);
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
    bool isImmutable = bufferUsage == BufferUsage::Static;
    bool mustUseInitialData = isImmutable || !isMultiSampled;

    HRESULT hr = _rhi_device->GetDxDevice()->CreateTexture3D(&tex_desc, (mustUseInitialData ? &subresource_data : nullptr), &dx_tex);
    bool succeeded = SUCCEEDED(hr);
    if(succeeded) {
        return new Texture3D(_rhi_device, dx_tex);
    } else {
        return nullptr;
    }
}
