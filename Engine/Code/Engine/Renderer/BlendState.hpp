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
        , source_factor(BlendFactor::ONE)
        , dest_factor(BlendFactor::ZERO)
        , blend_op(BlendOperation::ADD)
        , source_factor_alpha(BlendFactor::ONE)
        , dest_factor_alpha(BlendFactor::ZERO)
        , blend_op_alpha(BlendOperation::ADD)
        , blend_color_write_enable(BlendColorWriteEnable::ALL) { /* DO NOTHING */
    }

};

class BlendState {
public:

    explicit BlendState(RHIDevice* device, const XMLElement& element);

    explicit BlendState(RHIDevice* device,
                        bool enable = false,
                        bool alphaToCoverageEnable = false,
                        const BlendFactor& source_factor = BlendFactor::ONE,
                        const BlendFactor& dest_factor = BlendFactor::ZERO,
                        const BlendOperation& blend_op = BlendOperation::ADD,
                        const BlendFactor& source_factor_alpha = BlendFactor::ONE,
                        const BlendFactor& dest_factor_alpha = BlendFactor::ZERO,
                        const BlendOperation& blend_op_alpha = BlendOperation::ADD,
                        const BlendColorWriteEnable& blend_color_write_enable = BlendColorWriteEnable::ALL);

    explicit BlendState(RHIDevice* device, bool alphaToCoverageEnable = false, BlendDesc desc = BlendDesc());
    ~BlendState();

    ID3D11BlendState* GetDxBlendState();
protected:
    bool LoadFromXML(RHIDevice* device, const XMLElement& element);
    bool CreateBlendState(RHIDevice* device
                          , bool enable = false
                          , bool alphaToCoverageEnable = false
                          , const BlendFactor& source_factor = BlendFactor::ONE
                          , const BlendFactor& dest_factor = BlendFactor::ZERO
                          , const BlendOperation& blend_op = BlendOperation::ADD
                          , const BlendFactor& source_factor_alpha = BlendFactor::ONE
                          , const BlendFactor& dest_factor_alpha = BlendFactor::ONE
                          , const BlendOperation& blend_op_alpha = BlendOperation::ADD
                          , const BlendColorWriteEnable& blend_color_write_enable = BlendColorWriteEnable::ALL);
    bool CreateBlendState(RHIDevice* device, bool alphaToCoverageEnable = false, BlendDesc render_target = BlendDesc());
    bool CreateBlendState(RHIDevice* device, bool alphaToCoverageEnable = false, const std::vector<BlendDesc>& render_targets = { BlendDesc() });
private:
    ID3D11BlendState* _dx_state;
};