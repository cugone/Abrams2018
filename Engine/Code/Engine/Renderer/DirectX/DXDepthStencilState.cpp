#include "Engine/Renderer/DirectX/DXDepthStencilState.hpp"

#include "Engine/Renderer/DirectX/DXDevice.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/RHI/RHIDevice.hpp"

DXDepthStencilState::DXDepthStencilState(DXDevice* device, const XMLElement& element)
    : DepthStencilState(device, element)
{
    auto rhi_device = dynamic_cast<RHIDevice*>(device);
    bool load_succeeded = rhi_device && LoadFromXml(rhi_device, element);
    if(!load_succeeded) {
        _dx_depthstencilstate->Release();
        _dx_depthstencilstate = nullptr;
        ERROR_AND_DIE("DepthStencilState failed to load from Xml.");
    }
}

DXDepthStencilState::~DXDepthStencilState() {
    _dx_depthstencilstate->Release();
    _dx_depthstencilstate = nullptr;
}

ID3D11DepthStencilState* DXDepthStencilState::GetDxDepthStencilState() {
    return _dx_depthstencilstate;
}

void DXDepthStencilState::SetDxDepthStencilState(ID3D11DepthStencilState* state) {
    _dx_depthstencilstate = state;
}

DXDepthStencilState::DXDepthStencilState(
    DXDevice* device
    , bool enableDepthTest /*= true*/
    , bool enableStencilTest /*= false*/
    , bool enableDepthWrite /*= true*/
    , bool enableStencilRead /*= true*/
    , bool enableStencilWrite /*= true*/
    , const ComparisonFunction& depthComparison /*= ComparisonFunction::LESS*/
    , std::pair<const StencilOperation&, const StencilOperation&> failFrontBackOp /*= std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP)*/
    , std::pair<const StencilOperation&, const StencilOperation&> failDepthFrontBackOp /*= std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP)*/
    , std::pair<const StencilOperation&, const StencilOperation&> passFrontBackOp /*= std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP)*/
    , std::pair<const ComparisonFunction&, const ComparisonFunction&> stencilComparisonFrontBack /*= std::make_pair(ComparisonFunction::ALWAYS, ComparisonFunction::ALWAYS)*/
)
    : DepthStencilState()
    , _dx_depthstencilstate(nullptr) {
    bool state_succeeded = CreateDepthStencilState(device,
                                                   enableDepthTest,
                                                   enableStencilTest,
                                                   enableDepthWrite,
                                                   enableStencilRead,
                                                   enableStencilWrite,
                                                   depthComparison,
                                                   failFrontBackOp,
                                                   failDepthFrontBackOp,
                                                   passFrontBackOp,
                                                   stencilComparisonFrontBack);
    if(!state_succeeded) {
        _dx_depthstencilstate->Release();
        _dx_depthstencilstate = nullptr;
        ERROR_AND_DIE("Depth Stencil failed to create.");
    }
}
