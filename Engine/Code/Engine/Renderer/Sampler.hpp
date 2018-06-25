#pragma once

#include "Engine/Core/DataUtils.hpp"

#include "Engine/RHI/RHITypes.hpp"

class RHIDevice;
struct ID3D11SamplerState;

struct SamplerDesc {
    FilterMode min_filter;
    FilterMode mag_filter;
    FilterMode mip_filter;
    FilterComparisonMode compare_mode;
    TextureAddressMode UaddressMode;
    TextureAddressMode VaddressMode;
    TextureAddressMode WaddressMode;
    Rgba borderColor;
    ComparisonFunction compareFunc;
    unsigned int maxAnisotropicLevel;
    float mipmapLODBias;
    float minLOD;
    float maxLOD;
    SamplerDesc()
        :min_filter(FilterMode::Point)
        , mag_filter(FilterMode::Point)
        , mip_filter(FilterMode::Point)
        , compare_mode(FilterComparisonMode::None)
        , UaddressMode(TextureAddressMode::Wrap)
        , VaddressMode(TextureAddressMode::Wrap)
        , WaddressMode(TextureAddressMode::Wrap)
        , borderColor(Rgba::WHITE)
        , compareFunc(ComparisonFunction::Never)
        , maxAnisotropicLevel(1)
        , mipmapLODBias(0.0f)
        , minLOD(0.0f)
        , maxLOD(0.0f) {/* DO NOTHING */
    }

};

class Sampler {
public:
    explicit Sampler(RHIDevice* device
            , const FilterMode& min_filter = FilterMode::Point
            , const FilterMode& mag_filter = FilterMode::Point
            , const FilterMode& mip_filter = FilterMode::Point
            , const FilterComparisonMode& compare_mode = FilterComparisonMode::None
            , const TextureAddressMode& UaddressMode = TextureAddressMode::Wrap
            , const TextureAddressMode& VaddressMode = TextureAddressMode::Wrap
            , const TextureAddressMode& WaddressMode = TextureAddressMode::Wrap
            , const Rgba& borderColor = Rgba::WHITE
            , const ComparisonFunction& compareFunc = ComparisonFunction::Never
            , unsigned int maxAnisotropicLevel = 1
            , float mipmapLODBias = 0.0f
            , float minLOD = 0.0f
            , float maxLOD = 0.0f
    );
    explicit Sampler(RHIDevice* device, const SamplerDesc& desc);
    explicit Sampler(RHIDevice* device, const XMLElement& element);
    ~Sampler();
    ID3D11SamplerState* GetDxSampler() const;
protected:
    bool LoadFromXml(RHIDevice* device, const XMLElement& element);
    bool CreateSamplerState(RHIDevice* device, const SamplerDesc& desc = SamplerDesc());
    bool CreateSamplerState(RHIDevice* device
                            , const FilterMode& min_filter = FilterMode::Point
                            , const FilterMode& mag_filter = FilterMode::Point
                            , const FilterMode& mip_filter = FilterMode::Point
                            , const FilterComparisonMode& compare_mode = FilterComparisonMode::None
                            , const TextureAddressMode& UaddressMode = TextureAddressMode::Wrap
                            , const TextureAddressMode& VaddressMode = TextureAddressMode::Wrap
                            , const TextureAddressMode& WaddressMode = TextureAddressMode::Wrap
                            , const Rgba& borderColor = Rgba::WHITE
                            , const ComparisonFunction& compareFunc = ComparisonFunction::Never
                            , unsigned int maxAnisotropicLevel = 1
                            , float mipmapLODBias = 0.0f
                            , float minLOD = 0.0f
                            , float maxLOD = 0.0f);
private:
    ID3D11SamplerState* _dx_state = nullptr;
};