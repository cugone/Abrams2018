#pragma once

class Texture2D;
class Rgba;
class RHIDevice;

class RHIDeviceContext {
public:
    RHIDeviceContext(RHIDevice* parentDevice = nullptr);
    virtual ~RHIDeviceContext();

    virtual void ClearState() = 0;
    virtual void Flush() = 0;

    virtual void ClearColorTarget(Texture2D* output, const Rgba& color) = 0;
    virtual void ClearDepthStencilTarget(Texture2D* output
                                         , bool depth = true
                                         , bool stencil = true
                                         , float depthValue = 1.0f
                                         , unsigned char stencilValue = 0) = 0;

    RHIDevice* GetParentDevice() const;

private:
    RHIDevice* _device;
};
