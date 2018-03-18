#pragma once

#include <map>
#include <string>

#include "Engine/RHI/RHITypes.hpp"

#include "Engine/Math/IntVector2.hpp"

class Rgba;
class RHIDeviceContext;
class RHIDevice;
class RHIOutput;
class RHIInstance;
class Texture;
class Texture2D;
class IntVector3;
class DepthStencilState;


struct depthstencil_state_t {
    bool depth_enabled;
    bool depth_write;
    bool stencil_enabled;
    bool stencil_read;
    bool stencil_write;
    StencilOperation stencil_failFrontOp;
    StencilOperation stencil_failBackOp;
    StencilOperation stencil_failDepthFrontOp;
    StencilOperation stencil_failDepthBackOp;
    StencilOperation stencil_passFrontOp;
    StencilOperation stencil_passBackOp;
    ComparisonFunction stencil_testFront;
    ComparisonFunction stencil_testBack;
};


class Renderer {
public:
    Renderer() = default;
    Renderer(unsigned int width, unsigned int height);
    virtual ~Renderer() = 0;

    virtual void Initialize() = 0;
    virtual void BeginFrame() = 0;
    virtual void Update(float deltaSeconds) = 0;
    virtual void Render() const = 0;
    virtual void EndFrame() = 0;

    virtual void SetRenderTarget(Texture* color_target = nullptr, Texture* depthstencil_target = nullptr) = 0;
    virtual void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) = 0;
    virtual void SetViewportAsPercent(float x, float y, float w, float h) = 0;
    virtual void ClearColor(const Rgba& color) = 0;
    virtual void ClearTargetColor(Texture* target, const Rgba& color) = 0;
    virtual void ClearDepthStencilBuffer() = 0;
    virtual void Present(bool vsync = false) = 0;

    virtual Texture* CreateOrGetTexture(const std::wstring& filepath, const IntVector3& dimensions) = 0;

    bool RegisterTexture(const std::wstring& name, Texture* texture);
    virtual void SetTexture(Texture* texture, unsigned int registerIndex = 0) = 0;

    Texture* GetTexture(const std::wstring& filepath);

    virtual Texture* CreateDepthStencil(RHIDevice* owner, const IntVector2& dimensions) = 0;

    virtual Texture* CreateTexture(const std::wstring& filepath,
                           const IntVector3& dimensions,
                           const BufferUsage& bufferUsage = BufferUsage::STATIC,
                           const BufferBindUsage& bindUsage = BufferBindUsage::SHADER_RESOURCE,
                           const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNORM) = 0;

    virtual void SetVSync(bool vsync_setting) = 0;

    RHIDeviceContext* GetDeviceContext() const;
    RHIDevice* GetDevice() const;
    RHIOutput* GetOutput() const;

protected:
    std::map<std::wstring, Texture*> _textures{};
    RHIDeviceContext * _rhi_context = nullptr;
    RHIDevice* _rhi_device = nullptr;
    RHIOutput* _rhi_output = nullptr;
    RHIInstance* _rhi_instance = nullptr;
    Texture* _current_target = nullptr;
    Texture* _current_depthstencil = nullptr;
    Texture* _default_depthstencil = nullptr;
    DepthStencilState* _current_depthstencil_state = nullptr;
    IntVector2 _window_dimensions = IntVector2::ZERO;
    RHIOutputMode _current_outputMode = RHIOutputMode::WINDOWED;
    bool _vsync_setting = false;
private:
};
