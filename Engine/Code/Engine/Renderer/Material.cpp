#include "Engine/Renderer/Material.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Win.hpp"

#include "Engine/Renderer/Renderer.hpp"

#include <algorithm>
#include <filesystem>
#include <sstream>

Material::Material(Renderer* renderer)
    : _renderer(renderer)
    , _textures(CustomTextureIndexSlotOffset, nullptr)
{
    _textures[0] = _renderer->GetTexture("__diffuse");
    _textures[1] = _renderer->GetTexture("__normal");
    _textures[2] = _renderer->GetTexture("__displacement");
    _textures[3] = _renderer->GetTexture("__specular");
    _textures[4] = _renderer->GetTexture("__occlusion");
    _textures[5] = _renderer->GetTexture("__emissive");

    std::size_t count = _renderer->GetMaterialCount();
    std::ostringstream ss;
    ss << '_' << count;
    _name += ss.str();
}

Material::Material(Renderer* renderer, const XMLElement& element)
    : _renderer(renderer)
    , _textures(CustomTextureIndexSlotOffset, nullptr)
{
    _textures[0] = _renderer->GetTexture("__diffuse");
    _textures[1] = _renderer->GetTexture("__normal");
    _textures[2] = _renderer->GetTexture("__displacement");
    _textures[3] = _renderer->GetTexture("__specular");
    _textures[4] = _renderer->GetTexture("__occlusion");
    _textures[5] = _renderer->GetTexture("__emissive");

    std::size_t count = _renderer->GetMaterialCount();
    std::ostringstream ss;
    ss << '_' << count;
    _name += ss.str();

    LoadFromXml(element);
}

bool Material::LoadFromXml(const XMLElement& element) {
    namespace FS = std::filesystem;

    DataUtils::ValidateXmlElement(element, "material", "shader", "name", "lighting,textures");

    _name = DataUtils::ParseXmlAttribute(element, "name", _name);

    {
        auto xml_shader = element.FirstChildElement("shader");
        DataUtils::ValidateXmlElement(*xml_shader, "shader", "", "src");
        auto file = DataUtils::ParseXmlAttribute(*xml_shader, "src", "");
        FS::path p(file);
        p.make_preferred();
        if(auto shader = _renderer->GetShader(p.string())) {
            _shader = shader;
        } else {
            std::ostringstream ss;
            ss << "Shader referenced in Material file \"" << _name << "\" does not already exist.";
            ERROR_AND_DIE(ss.str().c_str());
            return false;
        }
    }

    if(auto xml_lighting = element.FirstChildElement("lighting")) {
        DataUtils::ValidateXmlElement(*xml_lighting, "lighting", "", "", "specularIntensity,specularFactor,specularPower,glossFactor,emissiveFactor");
        //specularIntensity and specularFactor are synonyms
        if(auto xml_specInt = xml_lighting->FirstChildElement("specularIntensity")) {
            _specularIntensity = DataUtils::ParseXmlElementText(*xml_specInt, _specularIntensity);
        }
        if(auto xml_specFactor = xml_lighting->FirstChildElement("specularFactor")) {
            _specularIntensity = DataUtils::ParseXmlElementText(*xml_specFactor, _specularIntensity);
        }
        //specularPower and glossFactor are synonyms
        if(auto xml_specPower = xml_lighting->FirstChildElement("specularPower")) {
            _specularPower = DataUtils::ParseXmlElementText(*xml_specPower, _specularPower);
        }
        if(auto xml_glossFactor = xml_lighting->FirstChildElement("glossFactor")) {
            _specularPower = DataUtils::ParseXmlElementText(*xml_glossFactor, _specularPower);
        }
        if(auto xml_emissiveFactor = xml_lighting->FirstChildElement("emissiveFactor")) {
            _emissiveFactor = DataUtils::ParseXmlElementText(*xml_emissiveFactor, _emissiveFactor);
        }
    }

    if(auto xml_textures = element.FirstChildElement("textures")) {
        const auto& loaded_textures = _renderer->GetLoadedTextures();
        auto invalid_tex = _renderer->GetTexture("__invalid");

        if(auto xml_diffuse = xml_textures->FirstChildElement("diffuse")) {
            auto file = DataUtils::ParseXmlAttribute(*xml_diffuse, "src", "");
            FS::path p(file);
            p.make_preferred();
            const auto& p_str = p.string();
            bool empty_path = p.empty();
            bool texture_not_exist = !empty_path && loaded_textures.find(p_str) == loaded_textures.end();
            bool invalid_src = empty_path || texture_not_exist;
            auto tex = invalid_src ? invalid_tex : (_renderer->GetTexture(p_str));
            _textures[0] = tex;
        }

        if(auto xml_normal = xml_textures->FirstChildElement("normal")) {
            auto file = DataUtils::ParseXmlAttribute(*xml_normal, "src", "");
            FS::path p(file);
            p.make_preferred();
            const auto& p_str = p.string();
            bool empty_path = p.empty();
            bool texture_not_exist = !empty_path && loaded_textures.find(p_str) == loaded_textures.end();
            bool invalid_src = empty_path || texture_not_exist;
            auto tex = invalid_src ? invalid_tex : (_renderer->GetTexture(p_str));
            _textures[1] = tex;
        }

        if(auto xml_displacement = xml_textures->FirstChildElement("displacement")) {
            auto file = DataUtils::ParseXmlAttribute(*xml_displacement, "src", "");
            FS::path p(file);
            p.make_preferred();
            const auto& p_str = p.string();
            bool empty_path = p.empty();
            bool texture_not_exist = !empty_path && loaded_textures.find(p_str) == loaded_textures.end();
            bool invalid_src = empty_path || texture_not_exist;
            auto tex = invalid_src ? invalid_tex : (_renderer->GetTexture(p_str));
            _textures[2] = tex;
        }

        if(auto xml_specular = xml_textures->FirstChildElement("specular")) {
            auto file = DataUtils::ParseXmlAttribute(*xml_specular, "src", "");
            FS::path p(file);
            p.make_preferred();
            const auto& p_str = p.string();
            bool empty_path = p.empty();
            bool texture_not_exist = !empty_path && loaded_textures.find(p_str) == loaded_textures.end();
            bool invalid_src = empty_path || texture_not_exist;
            auto tex = invalid_src ? invalid_tex : (_renderer->GetTexture(p_str));
            _textures[3] = tex;
        }

        if(auto xml_occlusion = xml_textures->FirstChildElement("occlusion")) {
            auto file = DataUtils::ParseXmlAttribute(*xml_occlusion, "src", "");
            FS::path p(file);
            p.make_preferred();
            const auto& p_str = p.string();
            bool empty_path = p.empty();
            bool texture_not_exist = !empty_path && loaded_textures.find(p_str) == loaded_textures.end();
            bool invalid_src = empty_path || texture_not_exist;
            auto tex = invalid_src ? invalid_tex : (_renderer->GetTexture(p_str));
            _textures[4] = tex;
        }

        if(auto xml_emissive = xml_textures->FirstChildElement("emissive")) {
            auto file = DataUtils::ParseXmlAttribute(*xml_emissive, "src", "");
            FS::path p(file);
            p.make_preferred();
            const auto& p_str = p.string();
            bool empty_path = p.empty();
            bool texture_not_exist = !empty_path && loaded_textures.find(p_str) == loaded_textures.end();
            bool invalid_src = empty_path || texture_not_exist;
            auto tex = invalid_src ? invalid_tex : (_renderer->GetTexture(p_str));
            _textures[5] = tex;
        }
        {
            auto numTextures = DataUtils::GetChildElementCount(*xml_textures, "texture");
            if(numTextures >= MaxCustomTextureSlotCount) {
                DebuggerPrintf("Max custom texture count exceeded. Cannot bind more than %i custom textures.", MaxCustomTextureSlotCount);
            }
            AddTextureSlots(numTextures);
        }

        DataUtils::IterateAllChildElements(*xml_textures, "texture",
        [this, &loaded_textures, &invalid_tex](const XMLElement& elem) {
            DataUtils::ValidateXmlElement(elem, "texture", "", "index,src");
            std::size_t index = CustomTextureIndexSlotOffset + DataUtils::ParseXmlAttribute(elem, std::string("index"), 0u);
            if(index >= CustomTextureIndexSlotOffset + MaxCustomTextureSlotCount) {
                return;
            }
            auto file = DataUtils::ParseXmlAttribute(elem, "src", "");
            FS::path p(file);
            p.make_preferred();
            const auto& p_str = p.string();
            bool empty_path = p.empty();
            bool texture_not_exist = !empty_path && loaded_textures.find(p_str) == loaded_textures.end();
            bool invalid_src = empty_path || texture_not_exist;
            auto tex = invalid_src ? invalid_tex : (_renderer->GetTexture(p_str));
            _textures[index] = tex;
        });
    }
    return true;
}

void Material::AddTextureSlots(std::size_t count) {
    std::size_t old_size = _textures.size();
    std::size_t new_size = (std::min)(old_size + MaxCustomTextureSlotCount, old_size + (std::min)(MaxCustomTextureSlotCount, count));
    _textures.resize(new_size);
    for(std::size_t i = old_size; i < new_size; ++i) {
        _textures[i] = nullptr;
    }
}

void Material::AddTextureSlot() {
    AddTextureSlots(1);
}

std::string Material::GetName() const {
    return _name;
}

Shader * Material::GetShader() const {
    return _shader;
}

std::size_t Material::GetTextureCount() const {
    return _textures.size();
}

Texture* Material::GetTexture(std::size_t i) const {
    return _textures[i];
}

Texture* Material::GetTexture(const TextureID& id) const {
    return GetTexture(std::underlying_type_t<TextureID>(id));
}

float Material::GetSpecularIntensity() const {
    return _specularIntensity;
}

float Material::GetGlossyFactor() const {
    return _specularPower;
}

float Material::GetEmissiveFactor() const {
    return _emissiveFactor;
}

Vector3 Material::GetSpecGlossEmitFactors() const {
    return Vector3(GetSpecularIntensity(), GetGlossyFactor(), GetEmissiveFactor());
}
