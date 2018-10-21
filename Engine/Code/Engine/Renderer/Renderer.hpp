#pragma once

#include "Engine/Core/DataUtils.hpp"
#include "Engine/Core/Vertex3D.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/Matrix4.hpp"

#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/StructuredBuffer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"

#include "Engine/RHI/RHITypes.hpp"

#include <filesystem>
#include <map>
#include <string>

class AABB2;
class AnimatedSprite;
class BlendState;
class Camera3D;
class ConstantBuffer;
class DepthStencilState;
struct DepthStencilDesc;
class IndexBuffer;
class IntVector3;
class KerningFont;
class Material;
class RasterState;
struct RasterDesc;
class Rgba;
class RHIDeviceContext;
class RHIDevice;
class RHIOutput;
class RHIInstance;
class Sampler;
struct SamplerDesc;
class Shader;
class ShaderProgram;
class SpriteSheet;
class StructuredBuffer;
class Texture;
class Texture1D;
class Texture2D;
class Texture3D;
class VertexBuffer;
class Frustum;

struct matrix_buffer_t {
    Matrix4 model{};
    Matrix4 view{};
    Matrix4 projection{};
};

struct time_buffer_t {
    float game_time = 0.0f;
    float system_time = 0.0f;
    float game_frame_time = 0.0f;
    float system_frame_time = 0.0f;
};

struct PointLightDesc {
    Vector3 position = Vector3::ZERO;
    Vector3 attenuation = Vector3::Z_AXIS;
    float intensity = 1.0f;
    Rgba color = Rgba::WHITE;
};

struct DirectionalLightDesc {
    Vector3 direction = Vector3::X_AXIS;
    Vector3 attenuation = Vector3::X_AXIS;
    float intensity = 1.0f;
    Rgba color = Rgba::WHITE;
};

struct SpotLightDesc {
    Vector3 position = Vector3::ZERO;
    Vector3 direction = Vector3::X_AXIS;
    Vector3 attenuation = Vector3::Z_AXIS;
    Vector2 inner_outer_anglesDegrees = Vector2{30.0f, 60.0f};
    float intensity = 1.0f;
    Rgba color = Rgba::WHITE;
};

struct light_t {
    Vector4 position = Vector4::ZERO;
    Vector4 color = Vector4::ONE_XYZ_ZERO_W;
    Vector4 attenuation = Vector4::Z_AXIS;
    Vector4 specAttenuation = Vector4::X_AXIS;
    Vector4 innerOuterDotThresholds = Vector4(-2.0f, -3.0f, 0.0f, 0.0f);
    Vector4 direction = -Vector4::Z_AXIS;
};

constexpr const unsigned int max_light_count = 16;

struct lighting_buffer_t {
    light_t lights[max_light_count] = { light_t{} };
    Vector4 ambient = Vector4::ZERO;
    Vector4 specular_glossy_emissive_factors = Vector4(1.0f, 8.0f, 0.0f, 1.0f);
    Vector4 eye_position = Vector4::ZERO;
    int useVertexNormals = 0;
    float padding[3] = {0.0f, 0.0f, 0.0f};
};

class Renderer {
public:
    Renderer() = default;
    Renderer(unsigned int width, unsigned int height);
    ~Renderer();

    void Initialize(bool headless = false);

    void BeginFrame();
    void Update(float deltaSeconds);
    void Render() const;
    void EndFrame();

    float GetGameFrameTime() const;
    float GetSystemFrameTime() const;
    float GetGameTime() const;
    float GetSystemTime() const;

    void SetFullscreen();
    void SetWindowed();
    void SetBorderlessWindowed();
    void SetWindowTitle(const std::string& newTitle);

    VertexBuffer* CreateVertexBuffer(const VertexBuffer::buffer_t& vbo) const;
    IndexBuffer* CreateIndexBuffer(const IndexBuffer::buffer_t& ibo) const;
    ConstantBuffer* CreateConstantBuffer(void* const& buffer, const std::size_t& buffer_size) const;
    StructuredBuffer* CreateStructuredBuffer(const StructuredBuffer::buffer_t& sbo, std::size_t element_size, std::size_t element_count) const;

    Texture* CreateOrGetTexture(const std::string& filepath, const IntVector3& dimensions);
    void RegisterTexturesFromFolder(const std::string& folderpath, bool recursive = false);
    bool RegisterTexture(const std::string& name, Texture* texture);
    void SetTexture(Texture* texture, unsigned int registerIndex = 0);

    Texture* GetTexture(const std::string& nameOrFile);

    Texture* CreateDepthStencil(const RHIDevice* owner, const IntVector2& dimensions);
    Texture* CreateRenderableDepthStencil(const RHIDevice* owner, const IntVector2& dimensions);

    void SetDepthStencilState(DepthStencilState* depthstencil);
    DepthStencilState* GetDepthStencilState(const std::string& name);
    void EnableDepth();
    void DisableDepth();

    Texture* Create1DTexture(const std::string& filepath, const BufferUsage& bufferUsage, const BufferBindUsage& bindUsage, const ImageFormat& imageFormat);
    Texture* Create1DTextureFromMemory(const unsigned char* data, unsigned int width = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm);
    Texture* Create1DTextureFromMemory(const std::vector<Rgba>& data, unsigned int width = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm);
    Texture* Create2DTexture(const std::string& filepath, const BufferUsage& bufferUsage, const BufferBindUsage& bindUsage, const ImageFormat& imageFormat);
    Texture* Create2DTextureFromMemory(const unsigned char* data, unsigned int width = 1, unsigned int height = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm);
    Texture* Create2DTextureFromMemory(const std::vector<Rgba>& data, unsigned int width = 1, unsigned int height = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm);
    Texture* Create2DTextureArrayFromMemory(const unsigned char* data, unsigned int width = 1, unsigned int height = 1, unsigned int depth = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm);
    Texture* Create2DTextureFromGifBuffer(const unsigned char* data, unsigned int width = 1, unsigned int height = 1, unsigned int depth = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm);
    Texture* Create2DTextureArrayFromGifBuffer(const unsigned char* data, unsigned int width = 1, unsigned int height = 1, unsigned int depth = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm);
    Texture* Create3DTexture(const std::string& filepath, const IntVector3& dimensions, const BufferUsage& bufferUsage, const BufferBindUsage& bindUsage, const ImageFormat& imageFormat);
    Texture* Create3DTextureFromMemory(const unsigned char* data, unsigned int width = 1, unsigned int height = 1, unsigned int depth = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm);
    Texture* Create3DTextureFromMemory(const std::vector<Rgba>& data, unsigned int width = 1, unsigned int height = 1, unsigned int depth = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm);
    Texture* CreateTexture(const std::string& filepath
                           , const IntVector3& dimensions
                           , const BufferUsage& bufferUsage = BufferUsage::Static
                           , const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource
                           , const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm);

    SpriteSheet* CreateSpriteSheet(const std::string& filepath, unsigned int width = 1, unsigned int height = 1);
    SpriteSheet* CreateSpriteSheet(const XMLElement& elem);
    AnimatedSprite* CreateAnimatedSprite(const std::string& filepath);
    AnimatedSprite* CreateAnimatedSprite(const XMLElement& elem);

    void SetRenderTarget(Texture* color_target = nullptr, Texture* depthstencil_target = nullptr);
    void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
    void SetViewportAndScissor(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
    void SetViewports(const std::vector<AABB3>& viewports);
    void SetViewportAsPercent(float x, float y, float w, float h);

    void SetScissor(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
    void SetScissorAndViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
    void SetScissors(const std::vector<AABB2>& scissors);

    void ClearColor(const Rgba& color);
    void ClearTargetColor(Texture* target, const Rgba& color);
    void ClearTargetDepthStencilBuffer(Texture* target, bool depth = true, bool stencil = true, float depthValue = 1.0f, unsigned char stencilValue = 0);
    void ClearDepthStencilBuffer();
    void Present();

    void DrawPoint(const Vertex3D& point);
    void DrawPoint(const Vector3& point, const Rgba& color = Rgba::WHITE, const Vector2& tex_coords = Vector2::ZERO);
    void DrawFrustum(const Frustum& frustum, const Rgba& color = Rgba::YELLOW, const Vector2& tex_coords = Vector2::ZERO);
    void DrawWorldGridXZ(float radius = 500.0f, float major_gridsize = 20.0f, float minor_gridsize = 5.0f, const Rgba& major_color = Rgba::WHITE, const Rgba& minor_color = Rgba::DARK_GRAY);
    void DrawWorldGridXY(float radius = 500.0f, float major_gridsize = 20.0f, float minor_gridsize = 5.0f, const Rgba& major_color = Rgba::WHITE, const Rgba& minor_color = Rgba::DARK_GRAY);
    void DrawAxes(float maxlength = 1000.0f, bool disable_unit_depth = true);
    void DrawDebugSphere(const Rgba& color);

    void Draw(const PrimitiveType& topology, const std::vector<Vertex3D>& vbo);
    void Draw(const PrimitiveType& topology, const std::vector<Vertex3D>& vbo, std::size_t vertex_count);
    void DrawIndexed(const PrimitiveType& topology, const std::vector<Vertex3D>& vbo, const std::vector<unsigned int>& ibo);
    void DrawIndexed(const PrimitiveType& topology, const std::vector<Vertex3D>& vbo, const std::vector<unsigned int>& ibo, std::size_t vertex_count, std::size_t startVertex = 0, std::size_t baseVertexLocation = 0);

    void SetLightingEyePosition(const Vector3& position);
    void SetAmbientLight(const Rgba& ambient);
    void SetAmbientLight(const Rgba& color, float intensity);
    void SetSpecGlossEmitFactors(Material* mat);
    void SetUseVertexNormalsForLighting(bool value);

    const light_t& GetLight(unsigned int index) const;
    void SetPointLight(unsigned int index, const PointLightDesc& desc);
    void SetDirectionalLight(unsigned int index, const DirectionalLightDesc& desc);
    void SetSpotlight(unsigned int index, const SpotLightDesc& desc);

    RHIDeviceContext* GetDeviceContext() const;
    const RHIDevice* GetDevice() const;
    RHIOutput* GetOutput() const;

    ShaderProgram* GetShaderProgram(const std::string& nameOrFile);
    ShaderProgram* CreateShaderProgramFromHlslFile(const std::string& filepath, const std::string& entryPointList, const PipelineStage& target) const;
    void CreateAndRegisterShaderProgramFromHlslFile(const std::string& filepath, const std::string& entryPointList, const PipelineStage& target);
    void RegisterShaderProgramsFromFolder(const std::string& folderpath, const std::string& entrypoint, const PipelineStage& target, bool recursive = false);
    void CreateAndRegisterRasterStateFromRasterDescription(const std::string& name, const RasterDesc& desc);
    void SetRasterState(RasterState* raster);
    RasterState* GetRasterState(const std::string& name);

    void CreateAndRegisterSamplerFromSamplerDescription(const std::string& name, const SamplerDesc& desc);
    Sampler* GetSampler(const std::string& name);
    void SetSampler(Sampler* sampler);

    void SetVSync(bool value);

    Material* CreateMaterialFromFont(KerningFont* font);
    bool RegisterMaterial(const std::string& filepath);
    void RegisterMaterial(Material* mat);
    void RegisterMaterialsFromFolder(const std::string& folderpath, bool recursive = false);
    std::size_t GetMaterialCount();
    Material* GetMaterial(const std::string& nameOrFile);
    void SetMaterial(Material* material);
    const std::map<std::string, Texture*>& GetLoadedTextures() const;

    std::size_t GetShaderCount() const;
    Shader* GetShader(const std::string& nameOrFile);
    void RegisterShadersFromFolder(const std::string& filepath, bool recursive = false);

    std::size_t GetFontCount() const;
    KerningFont* GetFont(const std::string& nameOrFile);
    bool RegisterFont(const std::string& filepath);
    void RegisterFont(KerningFont* font);
    void RegisterFontsFromFolder(const std::string& folderpath, bool recursive = false);

    void UpdateGameTime(float deltaSeconds);

    void SetModelMatrix(const Matrix4& mat);
    void SetViewMatrix(const Matrix4& mat);
    void SetProjectionMatrix(const Matrix4& mat);
    void SetOrthoProjection(const Vector2& leftBottom, const Vector2& rightTop, const Vector2& near_far);
    void SetOrthoProjection(const Vector2& dimensions, const Vector2& origin, float nearz, float farz);
    void SetOrthoProjectionFromViewHeight(float viewHeight, float aspectRatio, float nearz, float farz);
    void SetOrthoProjectionFromViewWidth(float viewWidth, float aspectRatio, float nearz, float farz);
    void SetOrthoProjectionFromCamera(const Camera3D& camera);
    void SetPerspectiveProjection(const Vector2& vfovDegrees_aspect, const Vector2& nz_fz);
    void SetPerspectiveProjectionFromCamera(const Camera3D& camera);

    void AppendModelMatrix(const Matrix4& modelMatrix);

    void SetConstantBuffer(unsigned int index, ConstantBuffer* buffer);
    void SetStructuredBuffer(unsigned int index, StructuredBuffer* buffer);

    void DrawQuad(const Vector3& position = Vector3::ZERO, const Vector3& halfExtents = Vector3::XY_AXIS * 0.5f, const Rgba& color = Rgba::WHITE, const Vector4& texCoords = Vector4::ZW_AXIS, const Vector3& normalFront = Vector3::Z_AXIS, const Vector3& worldUp = Vector3::Y_AXIS);
    void DrawQuad(const Rgba& frontColor, const Rgba& backColor, const Vector3& position = Vector3::ZERO, const Vector3& halfExtents = Vector3::XY_AXIS * 0.5f, const Vector4& texCoords = Vector4::ZW_AXIS, const Vector3& normalFront = Vector3::Z_AXIS, const Vector3& worldUp = Vector3::Y_AXIS);
    void DrawPoint2D(float pointX, float pointY, const Rgba& color = Rgba::WHITE);
    void DrawPoint2D(const Vector2& point, const Rgba& color = Rgba::WHITE);
    void DrawLine2D(float startX, float startY, float endX, float endY, const Rgba& color = Rgba::WHITE, float thickness = 0.0f);
    void DrawLine2D(const Vector2& start, const Vector2& end, const Rgba& color = Rgba::WHITE, float thickness = 0.0f);
    void DrawQuad2D(float left, float bottom, float right, float top, const Rgba& color = Rgba::WHITE, const Vector4& texCoords = Vector4::ZW_AXIS);
    void DrawQuad2D(const Vector2& position = Vector2::ZERO, const Vector2& halfExtents = Vector2(0.5f, 0.5f), const Rgba& color = Rgba::WHITE, const Vector4& texCoords = Vector4::ZW_AXIS);
    void DrawQuad2D(const Rgba& color);
    void DrawQuad2D(const Vector4& texCoords);
    void DrawQuad2D(const Rgba& color, const Vector4& texCoords);
    void DrawCircle2D(float centerX, float centerY, float radius, const Rgba& color = Rgba::WHITE);
    void DrawCircle2D(const Vector2& center, float radius, const Rgba& color = Rgba::WHITE);
    void DrawFilledCircle2D(const Vector2& center, float radius, const Rgba& color = Rgba::WHITE);
    void DrawAABB2(const AABB2& bounds, const Rgba& edgeColor, const Rgba& fillColor, const Vector2& edgeHalfExtents = Vector2(0.5f, 0.5f));
    void DrawAABB2(const Rgba& edgeColor, const Rgba& fillColor);
    void DrawPolygon2D(float centerX, float centerY, float radius, std::size_t numSides = 3, const Rgba& color = Rgba::WHITE);
    void DrawPolygon2D(const Vector2& center, float radius, std::size_t numSides = 3, const Rgba& color = Rgba::WHITE);
    void DrawX2D(const Vector2& position = Vector2::ZERO, const Vector2& half_extents = Vector2(0.5f, 0.5f), const Rgba& color = Rgba::WHITE);
    void DrawX2D(const Rgba& color);
    void DrawTextLine(KerningFont* font, const std::string& text, const Rgba& color = Rgba::WHITE);
    void DrawMultilineText(KerningFont* font, const std::string& text, const Rgba& color = Rgba::WHITE);
    void AppendMultiLineTextBuffer(KerningFont* font, const std::string& text, const Vector2& start_position, const Rgba& color, std::vector<Vertex3D>& vbo, std::vector<unsigned int>& ibo);

    constexpr static unsigned int MATRIX_BUFFER_INDEX = 0;
    constexpr static unsigned int TIME_BUFFER_INDEX = 1;
    constexpr static unsigned int LIGHTING_BUFFER_INDEX = 2;
    constexpr static unsigned int CONSTANT_BUFFER_START_INDEX = 3;
    constexpr static unsigned int STRUCTURED_BUFFER_START_INDEX = 64;
    constexpr static unsigned int MAX_LIGHT_COUNT = max_light_count;

    std::vector<ConstantBuffer*> CreateConstantBuffersFromShaderProgram(const ShaderProgram* _shader_program) const;

    void SetWinProc(const std::function<bool(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) >& windowProcedure);
protected:
private:
    void UpdateSystemTime(float deltaSeconds);
    void RegisterTexturesFromFolder(const std::filesystem::path& folderpath, bool recursive = false);
    bool RegisterTexture(const std::filesystem::path& filepath);
    void RegisterShaderProgram(const std::string& name, ShaderProgram * sp);
    void RegisterShaderProgramsFromFolder(const std::filesystem::path& folderpath, const std::string& entrypoint, const PipelineStage& target, bool recursive = false);
    void RegisterShader(const std::string& name, Shader* shader);
    bool RegisterShader(const std::filesystem::path& filepath);
    void RegisterShadersFromFolder(const std::filesystem::path& folderpath, bool recursive = false);
    void RegisterMaterial(const std::string& name, Material* mat);
    bool RegisterMaterial(const std::filesystem::path& filepath);
    void RegisterMaterialsFromFolder(const std::filesystem::path& folderpath, bool recursive = false);
    void RegisterRasterState(const std::string& name, RasterState* raster);
    void RegisterDepthStencilState(const std::string& name, DepthStencilState* depthstencil);
    void RegisterSampler(const std::string& name, Sampler* sampler);
    void RegisterFont(const std::string& name, KerningFont* font);
    bool RegisterFont(const std::filesystem::path& filepath);
    void RegisterFontsFromFolder(const std::filesystem::path& folderpath, bool recursive = false);

    void UpdateVbo(const VertexBuffer::buffer_t& vbo);
    void UpdateIbo(const IndexBuffer::buffer_t& ibo);

    void Draw(const PrimitiveType& topology, VertexBuffer* vbo, std::size_t vertex_count);
    void DrawIndexed(const PrimitiveType& topology, VertexBuffer* vbo, IndexBuffer* ibo, std::size_t index_count, std::size_t startVertex = 0, std::size_t baseVertexLocation = 0);

    SpriteSheet* CreateSpriteSheetFromGif(const std::string& filepath);
    AnimatedSprite* CreateAnimatedSpriteFromGif(const std::string& filepath);

    void SetLightAtIndex(unsigned int index, const light_t& light);
    void SetPointLight(unsigned int index, const light_t& light);
    void SetDirectionalLight(unsigned int index, const light_t& light);
    void SetSpotlight(unsigned int index, const light_t& light);
    
    void CreateAndRegisterDefaultTextures();
    Texture* CreateDefaultTexture();
    Texture* CreateInvalidTexture();
    Texture* CreateDefaultDiffuseTexture();
    Texture* CreateDefaultNormalTexture();
    Texture* CreateDefaultLightingTexture();
    Texture* CreateDefaultSpecularTexture();
    Texture* CreateDefaultOcclusionTexture();
    Texture* CreateDefaultEmissiveTexture();
    void CreateAndRegisterDefaultDepthStencil();

    void CreateAndRegisterDefaultShaderPrograms();
    ShaderProgram* CreateDefaultShaderProgram();
    ShaderProgram* CreateDefaultUnlitShaderProgram();
    ShaderProgram* CreateDefaultNormalShaderProgram();
    ShaderProgram* CreateDefaultNormalMapShaderProgram();

    void CreateAndRegisterDefaultShaders();
    Shader* CreateDefaultShader();
    Shader* CreateDefaultUnlitShader();
    Shader* CreateDefault2DShader();
    Shader* CreateDefaultNormalShader();
    Shader* CreateDefaultNormalMapShader();

    void CreateAndRegisterDefaultMaterials();
    Material* CreateDefaultMaterial();
    Material* CreateDefaultUnlitMaterial();
    Material* CreateDefault2DMaterial();
    Material* CreateDefaultNormalMaterial();
    Material* CreateDefaultNormalMapMaterial();

    void CreateAndRegisterDefaultSamplers();
    Sampler* CreateDefaultSampler();
    Sampler* CreateLinearSampler();
    Sampler* CreatePointSampler();

    void CreateAndRegisterDefaultRasterStates();
    RasterState* CreateDefaultRaster();
    RasterState* CreateWireframeRaster();
    RasterState* CreateSolidRaster();
    RasterState* CreateWireframeNoCullingRaster();
    RasterState* CreateSolidNoCullingRaster();
    RasterState* CreateWireframeFrontCullingRaster();
    RasterState* CreateSolidFrontCullingRaster();

    void CreateAndRegisterDefaultDepthStencilStates();
    DepthStencilState* CreateDefaultDepthStencilState();
    DepthStencilState* CreateDisabledDepth();
    DepthStencilState* CreateEnabledDepth();

    void UnbindAllShaderResources();
    void UnbindAllConstantBuffers();

    matrix_buffer_t _matrix_data = {};
    time_buffer_t _time_data = {};
    lighting_buffer_t _lighting_data = {};
    std::size_t _current_vbo_size = 0;
    std::size_t _current_ibo_size = 0;
    RHIDeviceContext* _rhi_context = nullptr;
    RHIDevice* _rhi_device = nullptr;
    RHIOutput* _rhi_output = nullptr;
    RHIInstance* _rhi_instance = nullptr;
    Texture* _current_target = nullptr;
    Texture* _current_depthstencil = nullptr;
    Texture* _default_depthstencil = nullptr;
    DepthStencilState* _current_depthstencil_state = nullptr;
    RasterState* _current_raster_state = nullptr;
    Sampler* _current_sampler = nullptr;
    Material* _current_material = nullptr;
    IntVector2 _window_dimensions = IntVector2::ZERO;
    RHIOutputMode _current_outputMode = RHIOutputMode::Windowed;
    VertexBuffer* _temp_vbo = nullptr;
    IndexBuffer* _temp_ibo = nullptr;
    ConstantBuffer* _matrix_cb = nullptr;
    ConstantBuffer* _time_cb = nullptr;
    ConstantBuffer* _lighting_cb = nullptr;
    std::map<std::string, Texture*> _textures = {};
    std::map<std::string, ShaderProgram*> _shader_programs = {};
    std::map<std::string, Material*> _materials = {};
    std::map<std::string, Shader*> _shaders = {};
    std::map<std::string, Sampler*> _samplers = {};
    std::map<std::string, RasterState*> _rasters = {};
    std::map<std::string, DepthStencilState*> _depthstencils = {};
    std::map<std::string, KerningFont*> _fonts = {};
    bool _vsync = false;
    friend class Shader;

};
