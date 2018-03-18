#include "Engine/RHI/RHIDeviceContext.hpp"

#include "Engine/Core/Rgba.hpp"

RHIDeviceContext::RHIDeviceContext(RHIDevice* parentDevice /*= nullptr*/)
    : _device(parentDevice)
{
    /* DO NOTHING */
}

RHIDeviceContext::~RHIDeviceContext() {
    _device = nullptr;
}

RHIDevice* RHIDeviceContext::GetParentDevice() const {
    return _device;
}
