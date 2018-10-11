#include "Engine/RHI/RHIOutput.hpp"

#include "Engine/Core/BuildConfig.hpp"

#include "Engine/Renderer/DirectX/DX11.hpp"

#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"

#include "Engine/Renderer/Window.hpp"

#include "Engine/Math/IntVector2.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Texture2D.hpp"

#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include <sstream>

RHIOutput::RHIOutput(const RHIDevice* parent, Window* wnd, IDXGISwapChain* swapchain)
    : _parent_device(parent)
    , _window(wnd)
    , _dxgi_swapchain(swapchain)
{
    CreateBackbuffer();
}

RHIOutput::~RHIOutput() {
    _parent_device = nullptr;

    if(_dxgi_swapchain) {
        _dxgi_swapchain->Release();
        _dxgi_swapchain = nullptr;
    }

    delete _back_buffer;
    _back_buffer = nullptr;

    delete _window;
    _window = nullptr;

}

const RHIDevice* RHIOutput::GetParentDevice() const {
    return _parent_device;
}

RHIDevice* RHIOutput::GetParentDevice() {
    return const_cast<RHIDevice*>(static_cast<const RHIOutput&>(*this).GetParentDevice());
}

const Window* RHIOutput::GetWindow() const {
    return _window;
}

Window* RHIOutput::GetWindow() {
    return const_cast<Window*>(static_cast<const RHIOutput&>(*this).GetWindow());
}

Texture* RHIOutput::GetBackBuffer() {
    return _back_buffer;
}

IntVector2 RHIOutput::GetDimensions() const {
    if(_window) {
        return _window->GetDimensions();
    } else {
        return IntVector2::ZERO;
    }
}

float RHIOutput::GetAspectRatio() const {
    if(_window) {
        const auto& dims = GetDimensions();
        if(dims.y < dims.x) {
            return dims.x / static_cast<float>(dims.y);
        } else {
            return dims.y / static_cast<float>(dims.x);
        }
    }
    return 0.0f;
}

void RHIOutput::SetDisplayMode(const RHIOutputMode& newMode) {
    _window->SetDisplayMode(newMode);
}

void RHIOutput::SetDimensions(const IntVector2& clientSize) {
    _window->SetDimensions(clientSize);
}

void RHIOutput::Present(bool vsync) {
    _dxgi_swapchain->Present(vsync ? 1 : 0, 0);
}

void RHIOutput::CreateBackbuffer() {
    if(_back_buffer != nullptr) {
        delete _back_buffer;
    }
    ID3D11Texture2D* back_buffer = nullptr;
    _dxgi_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&back_buffer));
    _back_buffer = new Texture2D(_parent_device, back_buffer);
    _back_buffer->SetDebugName("__back_buffer");
}

void RHIOutput::ResetBackbuffer() {
    delete _back_buffer;
    _back_buffer = nullptr;
    CreateBackbuffer();
}
