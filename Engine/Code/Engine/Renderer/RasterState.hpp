#pragma once

#include "Engine/Core/DataUtils.hpp"

#include "Engine/RHI/RHITypes.hpp"

class RHIDevice;
struct ID3D11RasterizerState;

struct RasterDesc {
    FillMode fillmode;
    CullMode cullmode;
    float depthBiasClamp;
    float slopeScaledDepthBias;
    int depthBias;
    bool depthClipEnable;
    bool scissorEnable;
    bool multisampleEnable;
    bool antialiasedLineEnable;
    RasterDesc()
        : fillmode(FillMode::Solid)
        , cullmode(CullMode::Back)
        , depthBiasClamp(0.0f)
        , slopeScaledDepthBias(0.0f)
        , depthBias(0)
        , depthClipEnable(true)
        , scissorEnable(true)
        , multisampleEnable(false)
        , antialiasedLineEnable(false)
    {
        /* DO NOTHING */
    }
};

class RasterState {
public:
    RasterState(RHIDevice* device, const RasterDesc& desc);
    RasterState(RHIDevice* device, const FillMode& fillmode = FillMode::Solid, const CullMode& cullmode = CullMode::Back, bool antiAliasing = false);
    RasterState(RHIDevice* device, const XMLElement& element);
    ~RasterState();

    ID3D11RasterizerState* GetDxRasterState();
protected:
    bool LoadFromXML(RHIDevice* device, const XMLElement& element);
    bool CreateRasterState(RHIDevice* device, const FillMode& fillmode = FillMode::Solid, const CullMode& cullmode = CullMode::Back, bool antiAliasing = false);
    bool CreateRasterState(RHIDevice* device, const RasterDesc& raster_desc = RasterDesc());
private:
    ID3D11RasterizerState* _dx_state;

};