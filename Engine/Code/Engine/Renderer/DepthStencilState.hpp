#pragma once

#include <utility>

#include "Engine/RHI/RHITypes.hpp"

#include "Engine/Core/DataUtils.hpp"

class RHIDevice;

class DepthStencilState {
public:
    DepthStencilState() = default;
    virtual ~DepthStencilState() = 0;

    DepthStencilState(RHIDevice* device, const XMLElement& element);
    explicit DepthStencilState(
        RHIDevice* device
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
    );
protected:
    bool LoadFromXml(RHIDevice* device, const XMLElement& element);
    bool CreateDepthStencilState(
        RHIDevice* device
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
    );

private:
};