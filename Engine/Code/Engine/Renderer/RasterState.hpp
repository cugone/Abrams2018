#pragma once

#include "Engine/Core/DataUtils.hpp"

#include "Engine/RHI/RHITypes.hpp"

class RHIDevice;
struct ID3D11RasterizerState;

struct RasterDesc {
    FillMode fillmode = FillMode::Solid;
    CullMode cullmode = CullMode::Back;
    float depthBiasClamp = 0.0f;
    float slopeScaledDepthBias = 0.0f;
    int depthBias = 0;
    bool depthClipEnable = true;
    bool scissorEnable = false;
    bool multisampleEnable = false;
    bool antialiasedLineEnable = false;
    RasterDesc() = default;
    explicit RasterDesc(const XMLElement& element);
};

class RasterState {
public:
    RasterState(RHIDevice* device, const RasterDesc& desc);
    RasterState(RHIDevice* device, const XMLElement& element);
    ~RasterState();

    const RasterDesc& GetDesc() const;
    ID3D11RasterizerState* GetDxRasterState();

    void SetDebugName([[maybe_unused]] const std::string& name) const noexcept;
protected:
    bool CreateRasterState(RHIDevice* device, const RasterDesc& raster_desc = RasterDesc{});
private:
    RasterDesc _desc{};
    ID3D11RasterizerState* _dx_state;

};