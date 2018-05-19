#include "Engine/Renderer/BlendState.hpp"

#include "Engine/Core/DataUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/RHI/RHIDevice.hpp"

//#define WIN32_LEAN_AND_MEAN
//#include <windows.h>

#include <algorithm>
#include <locale>
#include <string>

bool BlendState::LoadFromXML(RHIDevice* device, const XMLElement& element) {

    //Default values
    bool alphaToCoverageEnable = false;
    BlendDesc default_desc;

    auto xml_blends = element.FirstChildElement("blends");
    if(xml_blends != nullptr) {
        DataUtils::ValidateXmlElement(*xml_blends, "blends", "blend", "", "", "alphacoverage");
        alphaToCoverageEnable = DataUtils::ParseXmlAttribute(*xml_blends, "alphacoverage", false);

        std::vector<BlendDesc> desc;
        desc.reserve(DataUtils::GetChildElementCount(*xml_blends, "blend"));
        for(auto xml_blend = xml_blends->FirstChildElement("blend"); xml_blend != nullptr; xml_blend = xml_blend->NextSiblingElement("blend")) {
            DataUtils::ValidateXmlElement(*xml_blend, "blend", "", "", "color,alpha,enablemask", "enable");

            BlendDesc cur_desc;

            cur_desc.enable = DataUtils::ParseXmlAttribute(*xml_blend, "enable", false);

            auto xml_color = xml_blend->FirstChildElement("color");
            if(xml_color != nullptr) {
                DataUtils::ValidateXmlElement(*xml_color, "color", "", "src,dest,op");
                std::string source_factor_str = "one";
                source_factor_str = DataUtils::ParseXmlAttribute(*xml_color, "src", source_factor_str);
                std::string dest_factor_str = "zero";
                dest_factor_str = DataUtils::ParseXmlAttribute(*xml_color, "dest", dest_factor_str);
                std::string op_str = "add";
                op_str = DataUtils::ParseXmlAttribute(*xml_color, "op", op_str);

                cur_desc.source_factor = BlendFactorFromString(source_factor_str);
                cur_desc.dest_factor = BlendFactorFromString(dest_factor_str);
                cur_desc.blend_op = BlendOperationFromString(op_str);
            }

            auto xml_alpha = xml_blend->FirstChildElement("alpha");
            if(xml_alpha != nullptr) {
                DataUtils::ValidateXmlElement(*xml_alpha, "alpha", "", "src,dest,op");
                std::string source_factor_str = "one";
                source_factor_str = DataUtils::ParseXmlAttribute(*xml_color, "src", source_factor_str);
                std::string dest_factor_str = "zero";
                dest_factor_str = DataUtils::ParseXmlAttribute(*xml_color, "dest", dest_factor_str);
                std::string op_str = "add";
                op_str = DataUtils::ParseXmlAttribute(*xml_color, "op", op_str);

                cur_desc.source_factor_alpha = BlendFactorFromString(source_factor_str);
                cur_desc.dest_factor_alpha = BlendFactorFromString(dest_factor_str);
                cur_desc.blend_op_alpha = BlendOperationFromString(op_str);
            }

            auto xml_mask = xml_blend->FirstChildElement("enablemask");
            if(xml_mask != nullptr) {
                DataUtils::ValidateXmlElement(*xml_mask, "enablemask", "", "value");
                std::string mask_str = "all";
                mask_str = DataUtils::ParseXmlAttribute(*xml_mask, "value", mask_str);

                cur_desc.blend_color_write_enable = BlendColorWriteEnableFromString(mask_str);
            }

            desc.push_back(cur_desc);
        }
        if(desc.size() > 8) {
            desc.erase(desc.begin() + 8, desc.end());
            desc.shrink_to_fit();
        }
        if(!desc.empty()) {
            return CreateBlendState(device, alphaToCoverageEnable, desc);
        }
    }
    return CreateBlendState(device, alphaToCoverageEnable, default_desc);
}

bool BlendState::CreateBlendState(RHIDevice* device
                                  , bool enable /* = false */
                                  , bool alphaToCoverageEnable /* = false*/
                                  , const BlendFactor& source_factor /*= BlendFactor::ONE*/
                                  , const BlendFactor& dest_factor /*= BlendFactor::ZERO*/
                                  , const BlendOperation& blend_op /*= BlendOperation::ADD*/
                                  , const BlendFactor& source_factor_alpha /*= BlendFactor::ONE*/
                                  , const BlendFactor& dest_factor_alpha /*= BlendFactor::ONE*/
                                  , const BlendOperation& blend_op_alpha /*= BlendOperation::ADD*/
                                  , const BlendColorWriteEnable& blend_color_write_enable /*= BlendRenderTargetWriteMask::ALL*/) {
    D3D11_BLEND_DESC desc;
    memset(&desc, 0, sizeof(desc));

    desc.AlphaToCoverageEnable = alphaToCoverageEnable; //multisampling
    desc.IndependentBlendEnable = false; //multiple render targets

    desc.RenderTarget[0].BlendEnable = enable;
    desc.RenderTarget[0].BlendOp = BlendOpToD3DBlendOp(blend_op);
    desc.RenderTarget[0].SrcBlend = BlendFactorToD3DBlendFactor(source_factor);
    desc.RenderTarget[0].DestBlend = BlendFactorToD3DBlendFactor(dest_factor);

    desc.RenderTarget[0].BlendOpAlpha = BlendOpToD3DBlendOp(blend_op_alpha);
    desc.RenderTarget[0].SrcBlendAlpha = BlendFactorToD3DBlendFactor(source_factor_alpha);
    desc.RenderTarget[0].DestBlendAlpha = BlendFactorToD3DBlendFactor(dest_factor_alpha);

    desc.RenderTarget[0].RenderTargetWriteMask = BlendColorWriteEnableToD3DBlendColorWriteEnable(blend_color_write_enable);

    HRESULT hr = device->GetDxDevice()->CreateBlendState(&desc, &_dx_state);
    return SUCCEEDED(hr);
}

bool BlendState::CreateBlendState(RHIDevice* device, bool alphaToCoverageEnable /*= false*/, BlendDesc render_target /*= BlendDesc()*/) {
    return CreateBlendState(device, render_target.enable, alphaToCoverageEnable,
                            render_target.source_factor, render_target.dest_factor, render_target.blend_op,
                            render_target.source_factor_alpha, render_target.dest_factor_alpha, render_target.blend_op_alpha,
                            render_target.blend_color_write_enable);
}

bool BlendState::CreateBlendState(RHIDevice* device, bool alphaToCoverageEnable /*= false*/, const std::vector<BlendDesc>& render_targets /*= {BlendDesc()}*/) {
    D3D11_BLEND_DESC desc;
    memset(&desc, 0, sizeof(desc));

    desc.AlphaToCoverageEnable = alphaToCoverageEnable;
    desc.IndependentBlendEnable = render_targets.size() > 1;

    for(std::size_t i = 0; i < 8 && i < render_targets.size(); ++i) {
        desc.RenderTarget[i].BlendEnable = render_targets[i].enable;
        desc.RenderTarget[i].BlendOp = BlendOpToD3DBlendOp(render_targets[i].blend_op);
        desc.RenderTarget[i].SrcBlend = BlendFactorToD3DBlendFactor(render_targets[i].source_factor);
        desc.RenderTarget[i].DestBlend = BlendFactorToD3DBlendFactor(render_targets[i].dest_factor);

        desc.RenderTarget[i].BlendOpAlpha = BlendOpToD3DBlendOp(render_targets[i].blend_op_alpha);
        desc.RenderTarget[i].SrcBlendAlpha = BlendFactorToD3DBlendFactor(render_targets[i].source_factor_alpha);
        desc.RenderTarget[i].DestBlendAlpha = BlendFactorToD3DBlendFactor(render_targets[i].dest_factor_alpha);

        desc.RenderTarget[i].RenderTargetWriteMask = BlendColorWriteEnableToD3DBlendColorWriteEnable(render_targets[i].blend_color_write_enable);
    }
    HRESULT hr = device->GetDxDevice()->CreateBlendState(&desc, &_dx_state);
    return SUCCEEDED(hr);
}

BlendState::BlendState(RHIDevice* device, const XMLElement& element)
    : _dx_state(nullptr) {
    if(!LoadFromXML(device, element)) {
        _dx_state->Release();
        _dx_state = nullptr;
        ERROR_AND_DIE("\nBlendState: Failed to create.\n");
    }
}

BlendState::BlendState(RHIDevice* device,
                       bool alphaToCoverageEnable /*= false*/,
                       BlendDesc desc /*= BlendDesc()*/)
    : _dx_state(nullptr) {
    if(!CreateBlendState(device, alphaToCoverageEnable, desc)) {
        _dx_state->Release();
        _dx_state = nullptr;
        ERROR_AND_DIE("\nBlendState: Failed to create.\n");
    }
}

BlendState::BlendState(RHIDevice* device,
                       bool enable /* = false */,
                       bool alphaToCoverageEnable /* = false*/,
                       const BlendFactor& source_factor /*= BlendFactor::ONE*/,
                       const BlendFactor& dest_factor /*= BlendFactor::ZERO*/,
                       const BlendOperation& blend_op /*= BlendOperation::ADD*/,
                       const BlendFactor& source_factor_alpha /*= BlendFactor::ONE*/,
                       const BlendFactor& dest_factor_alpha /*= BlendFactor::ONE*/,
                       const BlendOperation& blend_op_alpha /*= BlendOperation::ADD*/,
                       const BlendColorWriteEnable& blend_color_write_enable /*= BlendRenderTargetMask::ALL*/)
    : _dx_state(nullptr) {
    if(!CreateBlendState(device
                         , enable
                         , alphaToCoverageEnable
                         , source_factor
                         , dest_factor
                         , blend_op
                         , source_factor_alpha
                         , dest_factor_alpha
                         , blend_op_alpha
                         , blend_color_write_enable)) {
        _dx_state->Release();
        _dx_state = nullptr;
        ERROR_AND_DIE("\nBlendState: Failed to create.\n");
    }
}

BlendState::~BlendState() {
    _dx_state->Release();
    _dx_state = nullptr;
}

ID3D11BlendState* BlendState::GetDxBlendState() {
    return _dx_state;
}
