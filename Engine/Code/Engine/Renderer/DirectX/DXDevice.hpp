#pragma once

#include "Engine/Renderer/DirectX/DX11.hpp"
#include "Engine/RHI/RHIDevice.hpp"

class Window;

class DXDevice : public RHIDevice {
public:
    DXDevice();
    virtual ~DXDevice();

    virtual RHIOutput* CreateOutput(Window* window, const RHIOutputMode& mode = RHIOutputMode::WINDOWED) override;
    virtual RHIOutput* CreateOutput(const IntVector2& clientSize, const IntVector2& clientPosition = IntVector2::ZERO, const RHIOutputMode& outputMode = RHIOutputMode::WINDOWED) override;
    D3D_FEATURE_LEVEL* GetFeatureLevel() const;
    ID3D11Device* GetDxDevice();
protected:

private:
    virtual RHIOutput* CreateOutputFromWindow(Window*& window) override;
    ID3D11Device* _dx_device = nullptr;
    D3D_FEATURE_LEVEL* _highestSupportedFeatureLevel = nullptr;
};