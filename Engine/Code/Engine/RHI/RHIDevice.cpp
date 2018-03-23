#include "Engine/RHI/RHIDevice.hpp"

#include "Engine/RHI/RHIOutput.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"
#include "Engine/Renderer/Window.hpp"

RHIDevice::RHIDevice()
: _immediate_context(nullptr)
{
    /* DO NOTHING */
}
RHIDevice::~RHIDevice() {
    _immediate_context = nullptr;
}

RHIDeviceContext* RHIDevice::GetImmediateContext() const {
    return _immediate_context;
}
