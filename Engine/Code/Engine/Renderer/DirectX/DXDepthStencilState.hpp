#pragma once

#include "Engine/Renderer/DirectX/DX11.hpp"
#include "Engine/Renderer/DepthStencilState.hpp"

class DXDevice;
struct ID3D11DepthStencilState;

class DXDepthStencilState : public DepthStencilState {
public:
    explicit DXDepthStencilState(
        DXDevice* device
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

    explicit DXDepthStencilState(DXDevice* device, const XMLElement& element);
    virtual ~DXDepthStencilState();

    ID3D11DepthStencilState* GetDxDepthStencilState();
    void SetDxDepthStencilState(ID3D11DepthStencilState* state);
protected:
private:
    ID3D11DepthStencilState* _dx_depthstencilstate;
};