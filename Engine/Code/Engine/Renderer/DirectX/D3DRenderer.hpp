#pragma once

#include "Engine/Renderer/Renderer.hpp"

class DXInstance;
class DXDevice;
class DXDeviceContext;
class DXOutput;
class IntVector3;

class D3DRenderer : public Renderer {
public:
    D3DRenderer() = default;
    D3DRenderer(unsigned int width, unsigned int height);
    virtual ~D3DRenderer();

    virtual void SetVSync(bool vsync_setting) override;

    virtual void Initialize() override;
    virtual void BeginFrame() override;
    virtual void Update(float deltaSeconds) override;
    virtual void Render() const override;
    virtual void EndFrame() override;

    virtual void SetRenderTarget(Texture* color_target = nullptr, Texture* depthstencil_target = nullptr) override;
    virtual void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) override;
    virtual void SetViewportAsPercent(float x, float y, float w, float h) override;
    virtual void ClearColor(const Rgba& color) override;
    virtual void ClearTargetColor(Texture* target, const Rgba& color) override;
    virtual void ClearDepthStencilBuffer() override;
    virtual void Present(bool vsync = false) override;
    virtual Texture* CreateOrGetTexture(const std::wstring& filepath, const IntVector3& dimensions) override;
    virtual void SetTexture(Texture* texture, unsigned int registerIndex = 0) override;


    virtual Texture* CreateTexture(const std::wstring& filepath,
                                     const IntVector3& dimensions,
                                     const BufferUsage& bufferUsage = BufferUsage::STATIC,
                                     const BufferBindUsage& bindUsage = BufferBindUsage::SHADER_RESOURCE,
                                     const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNORM) override;

protected:
    virtual Texture* CreateDepthStencil(RHIDevice* owner, const IntVector2& dimensions) override;
private:
    DXInstance* _dx_rhiinstance = nullptr;
    DXDevice* _dx_rhidevice = nullptr;
    DXDeviceContext* _dx_rhicontext = nullptr;
    DXOutput* _dx_rhioutput = nullptr;
};