#pragma once

#include "Engine/RHI/RHITypes.hpp"

#include "Engine/Math/IntVector2.hpp"

class RHIDeviceContext;
class IntVector2;
class Window;
class RHIOutput;
class DepthStencilState;

class RHIDevice {
public:
    RHIDevice();
    virtual ~RHIDevice() = 0;

    RHIDeviceContext* GetImmediateContext() const;

    virtual RHIOutput* CreateOutput(Window* window, const RHIOutputMode& mode = RHIOutputMode::WINDOWED) = 0;
    virtual RHIOutput* CreateOutput(const IntVector2& clientSize, const IntVector2& clientPosition = IntVector2::ZERO, const RHIOutputMode& outputMode = RHIOutputMode::WINDOWED) = 0;
    virtual bool CreateDepthStencilState(DepthStencilState* state
        , bool enableDepthTest = true
        , bool enableStencilTest = false
        , bool enableDepthWrite = true
        , bool enableStencilRead = true
        , bool enableStencilWrite = true
        , const ComparisonFunction& depthComparison = ComparisonFunction::LESS
        , std::pair<const StencilOperation&, const StencilOperation&> failFrontBackOp = std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP)
        , std::pair<const StencilOperation&, const StencilOperation&> failDepthFrontBackOp = std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP)
        , std::pair<const StencilOperation&, const StencilOperation&> passFrontBackOp = std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP)
        , std::pair<const ComparisonFunction&, const ComparisonFunction&> stencilComparisonFrontBack = std::make_pair(ComparisonFunction::ALWAYS, ComparisonFunction::ALWAYS)
    ) = 0;

    RHIDeviceType deviceType = RHIDeviceType::NONE;
protected:
    RHIDeviceContext* _immediate_context = nullptr;
private:
    virtual RHIOutput* CreateOutputFromWindow(Window*& window) = 0;

};