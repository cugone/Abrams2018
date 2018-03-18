#pragma once

class Texture;
class Rgba;
class RHIDevice;

class RHIDeviceContext {
public:
    RHIDeviceContext(RHIDevice* parentDevice = nullptr);
    virtual ~RHIDeviceContext();

    virtual void ClearState() = 0;
    virtual void Flush() = 0;

    virtual void ClearColorTarget(Texture* output, const Rgba& color) = 0;
    virtual void ClearDepthStencilTarget(Texture* output
                                         , bool depth = true
                                         , bool stencil = true
                                         , float depthValue = 1.0f
                                         , unsigned char stencilValue = 0) = 0;

    virtual void SetTexture(unsigned int index, Texture* texture) = 0;

    RHIDevice* GetParentDevice() const;

private:
    RHIDevice* _device;
};
