#pragma once

#include "Engine/Core/DataUtils.hpp"

#include "Engine/Renderer/DirectX/DX11.hpp"

#include "Engine/RHI/RHITypes.hpp"

#include <vector>

class RHIDevice;
struct ID3D11BlendState;

struct BlendDesc {
    bool enable = false;
    BlendFactor source_factor = BlendFactor::One;
    BlendFactor dest_factor   = BlendFactor::Zero;
    BlendOperation blend_op   = BlendOperation::Add;
    BlendFactor source_factor_alpha = BlendFactor::One;
    BlendFactor dest_factor_alpha   = BlendFactor::Zero;
    BlendOperation blend_op_alpha   = BlendOperation::Add;
    BlendColorWriteEnable blend_color_write_enable = BlendColorWriteEnable::All;
    BlendDesc() = default;
    explicit BlendDesc(const XMLElement& element);
};

class BlendState {
public:
    explicit BlendState(RHIDevice* device, const XMLElement& element);
    explicit BlendState(RHIDevice* device, const BlendDesc& desc = BlendDesc{}, bool alphaCoverage = false);
    explicit BlendState(RHIDevice* device, const std::vector<BlendDesc>& descs = std::vector<BlendDesc>{}, bool alphaCoverage = false, bool independantBlend = false);
    ~BlendState();

    ID3D11BlendState* GetDxBlendState();
protected:
    bool CreateBlendState(RHIDevice* device, BlendDesc render_target = BlendDesc{});
    bool CreateBlendState(RHIDevice* device, const std::vector<BlendDesc>& render_targets = { BlendDesc{} });
private:
    bool _alpha_to_coverage_enable = false;
    bool _independant_blend_enable = false;
    std::vector<BlendDesc> _descs{};
    ID3D11BlendState* _dx_state = nullptr;
};