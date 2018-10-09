#include "Engine/Renderer/RasterState.hpp"

#include "Engine/Core/BuildConfig.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Renderer/DirectX/DX11.hpp"

#include "Engine/RHI/RHIDevice.hpp"

#include <algorithm>
#include <locale>


void RasterState::SetDebugName([[maybe_unused]] const std::string& name) const noexcept {
#ifdef RENDER_DEBUG
    _dx_state->SetPrivateData(WKPDID_D3DDebugObjectName, name.size(), name.data());
#endif
}

RasterState::RasterState(RHIDevice* device, const XMLElement& element)
    : RasterState(device, RasterDesc{element})
{
    /* DO NOTHING */
}

RasterState::RasterState(RHIDevice* device, const RasterDesc& desc)
    : _desc(desc)
{
    if(!CreateRasterState(device, _desc)) {
        if(_dx_state) {
            _dx_state->Release();
            _dx_state = nullptr;
        }
        ERROR_AND_DIE("RasterState: dx Rasterizer failed to create.\n");
    }
}

RasterState::~RasterState() {
    if(_dx_state) {
        _dx_state->Release();
        _dx_state = nullptr;
    }
}

const RasterDesc& RasterState::GetDesc() const {
    return _desc;
}

ID3D11RasterizerState* RasterState::GetDxRasterState() {
    return _dx_state;
}

bool RasterState::CreateRasterState(RHIDevice* device, const RasterDesc& raster_desc /*= RasterDesc()*/) {

    D3D11_RASTERIZER_DESC desc{};

    desc.FillMode = FillModeToD3DFillMode(raster_desc.fillmode);
    desc.CullMode = CullModeToD3DCullMode(raster_desc.cullmode);
    desc.FrontCounterClockwise = false;
    desc.AntialiasedLineEnable = raster_desc.antialiasedLineEnable;
    desc.DepthBias = raster_desc.depthBias;
    desc.DepthBiasClamp = raster_desc.depthBiasClamp;
    desc.SlopeScaledDepthBias = raster_desc.slopeScaledDepthBias;
    desc.DepthClipEnable = raster_desc.depthClipEnable;
    desc.ScissorEnable = raster_desc.scissorEnable;

    HRESULT hr = device->GetDxDevice()->CreateRasterizerState(&desc, &_dx_state);
    return SUCCEEDED(hr);
}

RasterDesc::RasterDesc(const XMLElement& element) {
    if(auto xml_raster = element.FirstChildElement("raster")) {
        DataUtils::ValidateXmlElement(*xml_raster, "raster", "fill,cull", "", "antialiasing,depthbias,depthclip,scissor,msaa");
        auto xml_fill = xml_raster->FirstChildElement("fill");
        std::string fill_str = "solid";
        fill_str = DataUtils::ParseXmlElementText(*xml_fill, fill_str);
        this->fillmode = FillModeFromString(fill_str);

        auto xml_cull = xml_raster->FirstChildElement("cull");
        std::string cull_str = "back";
        cull_str = DataUtils::ParseXmlElementText(*xml_cull, cull_str);
        this->cullmode = CullModeFromString(cull_str);

        this->antialiasedLineEnable = false;
        if(auto xml_antialiasing = xml_raster->FirstChildElement("antialiasing")) {
            DataUtils::ValidateXmlElement(*xml_antialiasing, "antialiasing", "", "");
            this->antialiasedLineEnable = DataUtils::ParseXmlElementText(*xml_antialiasing, this->antialiasedLineEnable);
        }

        this->depthBias = 0;
        this->depthBiasClamp = 0.0f;
        this->slopeScaledDepthBias = 0.0f;
        if(auto xml_depthbias = xml_raster->FirstChildElement("depthbias")) {
            DataUtils::ValidateXmlElement(*xml_depthbias, "depthbias", "", "value,clamp,slopescaled");
            this->depthBias = DataUtils::ParseXmlAttribute(*xml_depthbias, "value", this->depthBias);
            this->depthBiasClamp = DataUtils::ParseXmlAttribute(*xml_depthbias, "clamp", this->depthBiasClamp);
            this->slopeScaledDepthBias = DataUtils::ParseXmlAttribute(*xml_depthbias, "slopescaled", this->slopeScaledDepthBias);
        }

        this->depthClipEnable = true;
        if(auto xml_depthclip = xml_raster->FirstChildElement("depthclip")) {
            DataUtils::ValidateXmlElement(*xml_depthclip, "depthclip", "", "");
            this->depthClipEnable = DataUtils::ParseXmlElementText(*xml_depthclip, this->depthClipEnable);
        }

        this->scissorEnable = false;
        if(auto xml_scissor = xml_raster->FirstChildElement("scissor")) {
            DataUtils::ValidateXmlElement(*xml_scissor, "scissor", "", "");
            this->scissorEnable = DataUtils::ParseXmlElementText(*xml_scissor, this->scissorEnable);
        }

        this->multisampleEnable = false;
        if(auto xml_msaa = xml_raster->FirstChildElement("msaa")) {
            DataUtils::ValidateXmlElement(*xml_msaa, "msaa", "", "");
            this->multisampleEnable = DataUtils::ParseXmlElementText(*xml_msaa, this->multisampleEnable);
        }
    }
}
