#pragma once

#include "Engine/Core/DataUtils.hpp"

#include "Engine/Renderer/DirectX/DX11.hpp"

#include "Engine/RHI/RHITypes.hpp"

#include <vector>

class RHIDevice;
struct ID3D11BlendState;

struct BlendDesc {
    bool enable;
    BlendFactor source_factor;
    BlendFactor dest_factor;
    BlendOperation blend_op;
    BlendFactor source_factor_alpha;
    BlendFactor dest_factor_alpha;
    BlendOperation blend_op_alpha;
    BlendColorWriteEnable blend_color_write_enable;

    BlendDesc()
        :enable(false)
        , source_factor(BlendFactor::One)
        , dest_factor(BlendFactor::Zero)
        , blend_op(BlendOperation::Add)
        , source_factor_alpha(BlendFactor::One)
        , dest_factor_alpha(BlendFactor::Zero)
        , blend_op_alpha(BlendOperation::Add)
        , blend_color_write_enable(BlendColorWriteEnable::All) { /* DO NOTHING */
    }

};

class BlendState {
public:

    explicit BlendState(RHIDevice* device, const XMLElement& element);

    explicit BlendState(RHIDevice* device,
                        bool enable = false,
                        bool alphaToCoverageEnable = false,
                        const BlendFactor& source_factor = BlendFactor::One,
                        const BlendFactor& dest_factor = BlendFactor::Zero,
                        const BlendOperation& blend_op = BlendOperation::Add,
                        const BlendFactor& source_factor_alpha = BlendFactor::One,
                        const BlendFactor& dest_factor_alpha = BlendFactor::Zero,
                        const BlendOperation& blend_op_alpha = BlendOperation::Add,
                        const BlendColorWriteEnable& blend_color_write_enable = BlendColorWriteEnable::All);

    explicit BlendState(RHIDevice* device, bool alphaToCoverageEnable = false, BlendDesc desc = BlendDesc());
    ~BlendState();

    ID3D11BlendState* GetDxBlendState();
protected:
    bool LoadFromXML(RHIDevice* device, const XMLElement& element);
    bool CreateBlendState(RHIDevice* device
                          , bool enable = false
                          , bool alphaToCoverageEnable = false
                          , const BlendFactor& source_factor = BlendFactor::One
                          , const BlendFactor& dest_factor = BlendFactor::Zero
                          , const BlendOperation& blend_op = BlendOperation::Add
                          , const BlendFactor& source_factor_alpha = BlendFactor::One
                          , const BlendFactor& dest_factor_alpha = BlendFactor::One
                          , const BlendOperation& blend_op_alpha = BlendOperation::Add
                          , const BlendColorWriteEnable& blend_color_write_enable = BlendColorWriteEnable::All);
    bool CreateBlendState(RHIDevice* device, bool alphaToCoverageEnable = false, BlendDesc render_target = BlendDesc());
    bool CreateBlendState(RHIDevice* device, bool alphaToCoverageEnable = false, const std::vector<BlendDesc>& render_targets = { BlendDesc() });
private:
    ID3D11BlendState* _dx_state;
};