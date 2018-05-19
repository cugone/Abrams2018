#pragma once

#include "Engine/Core/DataUtils.hpp"

#include "Engine/RHI/RHITypes.hpp"

#include <utility>

struct ID3D11DepthStencilState;
class RHIDevice;

class DepthStencilState {
public:
    explicit DepthStencilState(RHIDevice* device
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
    ~DepthStencilState();
    explicit DepthStencilState(RHIDevice* device, const XMLElement& element);
    ID3D11DepthStencilState* GetDxDepthStencilState() const;
protected:
private:
    bool LoadFromXml(RHIDevice* device, const XMLElement& element);
    bool CreateDepthStencilState(RHIDevice* device
                                 , bool enableDepthTest = true
                                 , bool enableStencilTest = false
                                 , bool enableDepthWrite = true
                                 , bool enableStencilRead = true
                                 , bool enableStencilWrite = true
                                 , const ComparisonFunction& depthComparison = ComparisonFunction::LESS
                                 , std::pair<const StencilOperation&, const StencilOperation&> failFrontBackOp = std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP)
                                 , std::pair<const StencilOperation&, const StencilOperation&> failDepthFrontBackOp = std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP)
                                 , std::pair<const StencilOperation&, const StencilOperation&> passFrontBackOp = std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP)
                                 , std::pair<const ComparisonFunction&, const ComparisonFunction&> stencilComparisonFrontBack = std::make_pair(ComparisonFunction::ALWAYS, ComparisonFunction::ALWAYS));
    bool _enableDepthTest = true;
    bool _enableStencilTest = false;
    bool _enableDepthWrite = true;
    bool _enableStencilRead = true;
    bool _enableStencilWrite = true;
    const ComparisonFunction& _depthComparison = ComparisonFunction::LESS;
    std::pair<const StencilOperation&, const StencilOperation&> _failFrontBackOp = std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP);
    std::pair<const StencilOperation&, const StencilOperation&> _failDepthFrontBackOp = std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP);
    std::pair<const StencilOperation&, const StencilOperation&> _passFrontBackOp = std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP);
    std::pair<const ComparisonFunction&, const ComparisonFunction&> _stencilComparisonFrontBack = std::make_pair(ComparisonFunction::ALWAYS, ComparisonFunction::ALWAYS);
    ID3D11DepthStencilState* _dx_state = nullptr;
};