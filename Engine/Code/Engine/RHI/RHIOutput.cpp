#include "Engine/RHI/RHIOutput.hpp"

#include "Engine/Renderer/DirectX/DX11.hpp"

#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"

#include "Engine/Renderer/Window.hpp"

#include "Engine/Math/IntVector2.hpp"
#include "Engine/Renderer/Texture.hpp"

#include "Engine/Core/Rgba.hpp"

RHIOutput::RHIOutput(RHIDevice* parent, Window* wnd)
    : _parentDevice(parent)
    , _window(wnd)
    , _back_buffer(nullptr)
{
    /* DO NOTHING */
}

RHIOutput::~RHIOutput() {

    _parentDevice = nullptr;

    delete _window;
    _window = nullptr;

    delete _back_buffer;
    _back_buffer = nullptr;

}

const RHIDevice* RHIOutput::GetParentDevice() const {
    return _parentDevice;
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
        return _window->GetClientSize();
    } else {
        return IntVector2::ZERO;
    }
}

float RHIOutput::GetAspectRatio() const {
    if(_window) {
        auto dims = GetDimensions();
        return dims.y / static_cast<float>(dims.x);
    }
    return 0.0f;
}
