#pragma once

#include "Engine/Core/DataUtils.hpp"

#include "Engine/RHI/RHITypes.hpp"

class RHIDevice;
struct ID3D11SamplerState;

struct SamplerDesc {
    FilterMode min_filter = FilterMode::Point;
    FilterMode mag_filter = FilterMode::Point;
    FilterMode mip_filter = FilterMode::Point;
    FilterComparisonMode compare_mode = FilterComparisonMode::None;
    TextureAddressMode UaddressMode = TextureAddressMode::Wrap;
    TextureAddressMode VaddressMode = TextureAddressMode::Wrap;
    TextureAddressMode WaddressMode = TextureAddressMode::Wrap;
    Rgba borderColor = Rgba::WHITE;
    ComparisonFunction compareFunc = ComparisonFunction::Never;
    unsigned int maxAnisotropicLevel = 1;
    float mipmapLODBias = 0.0f;
    float minLOD = 0.0f;
    float maxLOD = 0.0f;
    SamplerDesc() = default;
    explicit SamplerDesc(const XMLElement& element);
};

class Sampler {
public:
    explicit Sampler(RHIDevice* device, const SamplerDesc& desc);
    explicit Sampler(RHIDevice* device, const XMLElement& element);
    ~Sampler();
    ID3D11SamplerState* GetDxSampler() const;
    void SetDebugName([[maybe_unused]] const std::string& name) const noexcept;
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
    SamplerDesc _desc{};
    ID3D11SamplerState* _dx_state = nullptr;
};