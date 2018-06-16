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
protected:
private:
    bool LoadFromXml(const XMLElement& element);
    void AddTextureSlots(std::size_t count);
    void AddTextureSlot();

    constexpr static unsigned int CUSTOM_TEXTURE_INDEX_OFFSET = 6;
    constexpr static unsigned int MAX_CUSTOM_TEXTURE_COUNT = 58;
    std::string _name = "MATERIAL";
    Renderer* _renderer = nullptr;
    std::vector<Texture*> _textures = {};
    Shader* _shader = nullptr;
};