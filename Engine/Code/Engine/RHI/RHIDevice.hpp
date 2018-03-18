#pragma once

#include "Engine/RHI/RHITypes.hpp"

#include "Engine/Math/IntVector2.hpp"

class RHIDeviceContext;
class IntVector2;
class Window;
class RHIOutput;

class RHIDevice {
public:
    RHIDevice();
    virtual ~RHIDevice();

    RHIDeviceContext* GetImmediateContext() const;

    virtual RHIOutput* CreateOutput(Window* window, const RHIOutputMode& mode = RHIOutputMode::WINDOWED) = 0;
    virtual RHIOutput* CreateOutput(const IntVector2& clientSize, const IntVector2& clientPosition = IntVector2::ZERO, const RHIOutputMode& outputMode = RHIOutputMode::WINDOWED) = 0;

    RHIDeviceType deviceType = RHIDeviceType::NONE;
protected:
    RHIDeviceContext* _immediate_context = nullptr;
private:
    virtual RHIOutput* CreateOutputFromWindow(Window*& window) = 0;

};