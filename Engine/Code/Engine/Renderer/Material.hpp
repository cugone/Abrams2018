#pragma once

#include "Engine/Core/DataUtils.hpp"

class Renderer;
class Shader;
class Texture;

class Material {
public:

    Material(Renderer* renderer);
    Material(Renderer* renderer, const XMLElement& element);
    ~Material() = default;

    std::string GetName() const;
    Shader * GetShader() const;
    std::size_t GetTextureCount() const;
    Texture* GetTexture(std::size_t i) const;
    float GetSpecularIntensity() const;
    float GetGlossyFactor() const;
    float GetEmissiveFactor() const;
    Vector3 GetSpecGlossEmitFactors() const;

protected:
private:
    bool LoadFromXml(const XMLElement& element);
    void AddTextureSlots(std::size_t count);
    void AddTextureSlot();

    constexpr static std::size_t CUSTOM_TEXTURE_INDEX_OFFSET = 6u;
    constexpr static std::size_t MAX_CUSTOM_TEXTURE_COUNT = 58u;
    float _specularIntensity = 1.0f;
    float _specularPower = 8.0f;
    float _emissiveFactor = 0.0f;
    std::string _name = "MATERIAL";
    Renderer* _renderer = nullptr;
    std::vector<Texture*> _textures = {};
    Shader* _shader = nullptr;
};