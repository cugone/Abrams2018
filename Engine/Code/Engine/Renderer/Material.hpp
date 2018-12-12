#pragma once

#include "Engine/Core/DataUtils.hpp"

#include "Engine/Renderer/DirectX/DX11.hpp"

class Renderer;
class Shader;
class Texture;

class Material {
public:
    enum class TextureID : std::size_t {
        Diffuse
        ,Normal
        ,Displacement
        ,Specular
        ,Occlusion
        ,Emissive
        ,Custom1
        ,Custom2
        ,Custom3
        ,Custom4
        ,Custom5
        ,Custom6
        ,Custom7
        ,Custom8
        ,Custom9
        ,Custom10
        ,Custom11
        ,Custom12
        ,Custom13
        ,Custom14
        ,Custom15
        ,Custom16
        ,Custom17
        ,Custom18
        ,Custom19
        ,Custom20
        ,Custom21
        ,Custom22
        ,Custom23
        ,Custom24
        ,Custom25
        ,Custom26
        ,Custom27
        ,Custom28
        ,Custom29
        ,Custom30
        ,Custom31
        ,Custom32
        ,Custom33
        ,Custom34
        ,Custom35
        ,Custom36
        ,Custom37
        ,Custom38
        ,Custom39
        ,Custom40
        ,Custom41
        ,Custom42
        ,Custom43
        ,Custom44
        ,Custom45
        ,Custom46
        ,Custom47
        ,Custom48
        ,Custom49
        ,Custom50
        ,Custom51
        ,Custom52
        ,Custom53
        ,Custom54
        ,Custom55
        ,Custom56
        ,Custom57
        ,Custom58
        ,Custom60
        ,Custom61
        ,Custom62
        ,Custom63
        ,Custom64
        ,Custom65
        ,Custom66
        ,Custom67
        ,Custom68
        ,Custom69
        ,Custom70
        ,Custom71
        ,Custom72
        ,Custom73
        ,Custom74
        ,Custom75
        ,Custom76
        ,Custom77
        ,Custom78
        ,Custom79
        ,Custom80
        ,Custom81
        ,Custom82
        ,Custom83
        ,Custom84
        ,Custom85
        ,Custom86
        ,Custom87
        ,Custom88
        ,Custom89
        ,Custom90
        ,Custom91
        ,Custom92
        ,Custom93
        ,Custom94
        ,Custom95
        ,Custom96
        ,Custom97
        ,Custom98
        ,Custom99
        ,Custom100
        ,Custom101
        ,Custom102
        ,Custom103
        ,Custom104
        ,Custom105
        ,Custom106
        ,Custom107
        ,Custom108
        ,Custom109
        ,Custom110
        ,Custom111
        ,Custom112
        ,Custom113
        ,Custom114
        ,Custom115
        ,Custom116
        ,Custom117
        ,Custom118
        ,Custom119
        ,Custom120
        ,Custom121
        ,Custom122
    };
    Material(Renderer* renderer);
    Material(Renderer* renderer, const XMLElement& element);
    ~Material() = default;

    std::string GetName() const;
    Shader * GetShader() const;
    std::size_t GetTextureCount() const;
    Texture* GetTexture(std::size_t i) const;
    Texture* GetTexture(const TextureID& id) const;
    float GetSpecularIntensity() const;
    float GetGlossyFactor() const;
    float GetEmissiveFactor() const;
    Vector3 GetSpecGlossEmitFactors() const;

protected:
private:
    bool LoadFromXml(const XMLElement& element);
    void AddTextureSlots(std::size_t count);
    void AddTextureSlot();

    constexpr static std::size_t CustomTextureIndexOffset = 6u;
    constexpr static std::size_t MaxCustomTextureCount = (D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT / 2) - CustomTextureIndexOffset;
    float _specularIntensity = 1.0f;
    float _specularPower = 8.0f;
    float _emissiveFactor = 0.0f;
    std::string _name = "MATERIAL";
    Renderer* _renderer = nullptr;
    std::vector<Texture*> _textures = {};
    Shader* _shader = nullptr;
};