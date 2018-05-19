#include "Engine/Renderer/RasterState.hpp"

#include <algorithm>
#include <locale>

#include "Engine/Renderer/DirectX/DX11.hpp"

//#define WIN32_LEAN_AND_MEAN
//#include <windows.h>

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/RHI/RHIDevice.hpp"

RasterState::RasterState(RHIDevice* device, const FillMode& fillmode /*= FillMode::SOLID*/, const CullMode& cullmode /*= CullMode::BACK*/, bool antiAliasing /*= false*/) {
    if(!CreateRasterState(device, fillmode, cullmode, antiAliasing)) {
        _dx_state->Release();
        _dx_state = nullptr;
        ERROR_AND_DIE("RasterState: dx Rasterizer failed to create.\n");
    }
}
RasterState::RasterState(RHIDevice* device, const XMLElement& element) {
    if(!LoadFromXML(device, element)) {
        _dx_state->Release();
        _dx_state = nullptr;
        ERROR_AND_DIE("RasterState: Load from XML failed.\n");
    }
}
RasterState::~RasterState() {
    _dx_state->Release();
    _dx_state = nullptr;
}
ID3D11RasterizerState* RasterState::GetDxRasterState() {
    return _dx_state;
}

bool RasterState::LoadFromXML(RHIDevice* device, const XMLElement& element) {

    RasterDesc desc;
    desc.fillmode = FillMode::SOLID;
    desc.cullmode = CullMode::BACK;
    desc.antialiasedLineEnable = false;
    desc.depthClipEnable = true;
    desc.scissorEnable = false;

    auto xml_raster = element.FirstChildElement("raster");
    if(xml_raster != nullptr) {

        DataUtils::ValidateXmlElement(*xml_raster, "raster", "fill,cull", "", "antialiasing,depthbias,depthclip,scissor,msaa");
        auto xml_fill = xml_raster->FirstChildElement("fill");
        std::string fill_str = "solid";
        fill_str = DataUtils::ParseXmlElementText(*xml_fill, fill_str);
        desc.fillmode = FillModeFromString(fill_str);

        auto xml_cull = xml_raster->FirstChildElement("cull");
        std::string cull_str = "back";
        cull_str = DataUtils::ParseXmlElementText(*xml_cull, cull_str);
        desc.cullmode = CullModeFromString(cull_str);

        desc.antialiasedLineEnable = false;
        auto xml_antialiasing = xml_raster->FirstChildElement("antialiasing");
        if(xml_antialiasing != nullptr) {
            DataUtils::ValidateXmlElement(*xml_antialiasing, "antialiasing", "", "");
            desc.antialiasedLineEnable = DataUtils::ParseXmlElementText(*xml_antialiasing, desc.antialiasedLineEnable);
        }

        desc.depthBias = 0;
        desc.depthBiasClamp = 0.0f;
        desc.slopeScaledDepthBias = 0.0f;
        auto xml_depthbias = xml_raster->FirstChildElement("depthbias");
        if(xml_depthbias != nullptr) {
            DataUtils::ValidateXmlElement(*xml_depthbias, "depthbias", "", "value,clamp,slopescaled");
            desc.depthBias = DataUtils::ParseXmlAttribute(*xml_depthbias, "value", desc.depthBias);
            desc.depthBiasClamp = DataUtils::ParseXmlAttribute(*xml_depthbias, "clamp", desc.depthBiasClamp);
            desc.slopeScaledDepthBias = DataUtils::ParseXmlAttribute(*xml_depthbias, "slopescaled", desc.slopeScaledDepthBias);
        }

        desc.depthClipEnable = true;
        auto xml_depthclip = xml_raster->FirstChildElement("depthclip");
        if(xml_depthclip != nullptr) {
            DataUtils::ValidateXmlElement(*xml_depthclip, "depthclip", "", "");
            desc.depthClipEnable = DataUtils::ParseXmlElementText(*xml_depthclip, desc.depthClipEnable);
        }

        desc.scissorEnable = false;
        auto xml_scissor = xml_raster->FirstChildElement("scissor");
        if(xml_scissor != nullptr) {
            DataUtils::ValidateXmlElement(*xml_scissor, "scissor", "", "");
            desc.scissorEnable = DataUtils::ParseXmlElementText(*xml_scissor, desc.scissorEnable);
        }

        desc.multisampleEnable = false;
        auto xml_msaa = xml_raster->FirstChildElement("scissor");
        if(xml_msaa != nullptr) {
            DataUtils::ValidateXmlElement(*xml_msaa, "scissor", "", "");
            desc.multisampleEnable = DataUtils::ParseXmlElementText(*xml_msaa, desc.multisampleEnable);
        }
    }

    return CreateRasterState(device, desc);
}
bool RasterState::CreateRasterState(RHIDevice* device, const RasterDesc& raster_desc /*= RasterDesc()*/) {

    D3D11_RASTERIZER_DESC desc;
    memset(&desc, 0, sizeof(desc));

    desc.FillMode = FillModeToD3DFillMode(raster_desc.fillmode);
    desc.CullMode = CullModeToD3DCullMode(raster_desc.cullmode);
    desc.FrontCounterClockwise = true;
    desc.AntialiasedLineEnable = raster_desc.antialiasedLineEnable;
    desc.DepthBias = raster_desc.depthBias;
    desc.DepthBiasClamp = raster_desc.depthBiasClamp;
    desc.SlopeScaledDepthBias = raster_desc.slopeScaledDepthBias;
    desc.DepthClipEnable = raster_desc.depthClipEnable;
    desc.ScissorEnable = raster_desc.scissorEnable;

    HRESULT hr = device->GetDxDevice()->CreateRasterizerState(&desc, &_dx_state);
    return SUCCEEDED(hr);
}

bool RasterState::CreateRasterState(RHIDevice* device, const FillMode& fillmode /*= FillMode::SOLID*/, const CullMode& cullmode /*= CullMode::BACK*/, bool antiAliasing /*= false*/) {

    D3D11_RASTERIZER_DESC desc;
    memset(&desc, 0, sizeof(desc));

    desc.FillMode = FillModeToD3DFillMode(fillmode);
    desc.CullMode = CullModeToD3DCullMode(cullmode);
    desc.FrontCounterClockwise = true;
    desc.AntialiasedLineEnable = antiAliasing;
    desc.DepthBias = 0;
    desc.DepthBiasClamp = 0.0f;
    desc.SlopeScaledDepthBias = 0.0f;
    desc.DepthClipEnable = true;
    desc.ScissorEnable = false;

    HRESULT hr = device->GetDxDevice()->CreateRasterizerState(&desc, &_dx_state);
    return SUCCEEDED(hr);
}
