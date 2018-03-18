#pragma once

#include "Engine/RHI/RHITypes.hpp"

class Window;
class RHIDevice;
class Texture2D;
class IntVector2;
class Rgba;

struct IDXGISwapChain;

class RHIOutput {
public:
    RHIOutput(RHIDevice* parent, Window* wnd);

    virtual ~RHIOutput() = 0;

    const RHIDevice* GetParentDevice() const;
    RHIDevice* GetParentDevice();

    const Window* GetWindow() const;
    Window* GetWindow();

    Texture2D* GetBackBuffer();
    IntVector2 GetDimensions() const;
    float GetAspectRatio() const;

    virtual bool SetDisplayMode(const RHIOutputMode& newMode) = 0;

    virtual void Present(bool vsync) = 0;

protected:
    virtual void CreateBackbuffer() = 0;
    virtual void ResetBackbuffer() = 0;
    Window * _window = nullptr;
    RHIDevice* _parentDevice = nullptr;
    Texture2D* _back_buffer = nullptr;
private:

};