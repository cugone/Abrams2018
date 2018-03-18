#pragma once

#include "Engine/RHI/RHITypes.hpp"
#include "Engine/RHI/RHIOutput.hpp"

class Window;
class RHIDevice;
class Texture2D;
class DXTexture2D;
class IntVector2;
class Rgba;
class DXDevice;

struct IDXGISwapChain1;

class DXOutput : public RHIOutput {
public:
    DXOutput(DXDevice* parent, Window* wnd, IDXGISwapChain1* swap_chain);
    virtual ~DXOutput();

    virtual void Present(bool vsync) override;
    virtual bool SetDisplayMode(const RHIOutputMode& newMode) override;

protected:
    virtual void CreateBackbuffer() override;
    virtual void ResetBackbuffer() override;
private:
    IDXGISwapChain1* _dx_swapchain = nullptr;
    DXDevice* _dx_parentdevice = nullptr;
    DXTexture2D* _dx_backbuffer = nullptr;
};