#include "Engine/Core/KerningFont.hpp"

#include "Engine/Core/DataUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"

#include "Engine/Renderer/Material.hpp"

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <sstream>

KerningFont::KerningFont(Renderer* renderer)
    : _renderer(renderer)
{
    /* DO NOTHING */
}

float KerningFont::CalculateTextWidth(const KerningFont& font, const std::string& text, float scale /*= 1.0f*/) {
    float cursor_x = 0.0f;

    for(auto char_iter = text.begin(); char_iter != text.end(); ++char_iter) {
        KerningFont::CharDef current_char_def = font.GetCharDef(*char_iter);
        auto previous_char = char_iter++;
        float kern_value = 0.0f;
        if(char_iter != text.end()) {
            auto kern_iter = font._kernmap.find(std::make_pair(*previous_char, *char_iter));
            if(kern_iter != font._kernmap.end()) {
                kern_value = static_cast<float>(kern_iter->second);
            }
            cursor_x += current_char_def.xadvance + kern_value;
        } else {
            KerningFont::CharDef previous_char_def = font.GetCharDef(*previous_char);
            cursor_x += previous_char_def.xadvance;
        }
    }

    return cursor_x * scale;
}

float KerningFont::CalculateTextWidth(const std::string& text, float scale /*= 1.0f*/) const {
    return CalculateTextWidth(*this, text, scale);
}

float KerningFont::CalculateTextHeight(const KerningFont& font, const std::string& text, float scale /*= 1.0f*/) {
    return (1.0f + static_cast<float>(std::count(text.begin(), text.end(), '\n'))) * font.GetLineHeight() * scale;
}

float KerningFont::CalculateTextHeight(const std::string& text, float scale /*= 1.0f*/) const {
    return CalculateTextHeight(*this, text, scale);
}

float KerningFont::GetLineHeight() const {
    return static_cast<float>(_common.line_height);
}

float KerningFont::GetLineHeightAsUV() const {
    return GetLineHeight() / static_cast<float>(_common.scale.y);
}

const std::string& KerningFont::GetName() const {
    return _name;
}

KerningFont::CharDef KerningFont::GetCharDef(int ch) const {
    auto chardef_iter = _charmap.find(ch);
    if(chardef_iter == _charmap.end()) {
        chardef_iter = _charmap.find(-1);
        if(chardef_iter == _charmap.end()) {
            return CharDef{};
        }
    }
    return chardef_iter->second;
}

const KerningFont::CommonDef& KerningFont::GetCommonDef() const {
    return _common;
}

const KerningFont::InfoDef& KerningFont::GetInfoDef() const {
    return _info;
}

const std::vector<std::string>& KerningFont::GetImagePaths() const {
    return _image_paths;
}

const std::string& KerningFont::GetFilePath() const {
    return _filepath;
}

bool KerningFont::LoadFromFile(const std::string& filepath) {
    {
        namespace FS = std::experimental::filesystem;
        FS::path path{ filepath };
        bool path_exists = FS::exists(path);
        bool is_not_directory = !FS::is_directory(path);
        bool is_file = FS::is_regular_file(path);
        bool is_fnt = path.has_extension() && path.extension() == ".fnt";
        bool is_valid = path_exists && is_not_directory && is_file && is_fnt;
        std::string pathAsStr = path.string();
        if(!is_valid) {
            std::ostringstream ss;
            ss << pathAsStr << " is not a BMFont file.\n";
            DebuggerPrintf(ss.str().c_str());
            return false;
        }
        if(_is_loaded) {
            std::ostringstream ss;
            ss << pathAsStr << " is already loaded.\n";
            DebuggerPrintf(ss.str().c_str());
            return false;
        }
        _filepath = pathAsStr;
    }
    std::vector<unsigned char> out_buffer{};
    FileUtils::ReadBufferFromFile(out_buffer, _filepath);
    if(out_buffer.size() < 4) {
        std::ostringstream ss;
        ss << _filepath << " is not a BMFont file.\n";
        DebuggerPrintf(ss.str().c_str());
        return false;
    }

    bool is_binary = out_buffer[0] == 66 && out_buffer[1] == 77 && out_buffer[2] == 70;
    bool is_text = out_buffer[0] == 105 && out_buffer[1] == 110 && out_buffer[2] == 102 && out_buffer[3] == 111;
    if(is_binary) {
        _is_loaded = LoadFromBinary(out_buffer);
    } else if(is_text) {
        _is_loaded = LoadFromText(out_buffer);
    } else {
        _is_loaded = LoadFromXml(out_buffer);
    }
    return _is_loaded;
}

Material* KerningFont::GetMaterial() const {
    return _material;
}

void KerningFont::SetMaterial(Material* mat) {
    _material = mat;
}

int KerningFont::GetKerningValue(int first, int second) {
    auto iter = _kernmap.find(std::make_pair(first, second));
    if(iter != _kernmap.end()) {
        return (*iter).second;
    }
    return 0;
}

bool KerningFont::LoadFromText(std::vector<unsigned char>& buffer) {
    for(std::size_t i = 0; i < buffer.size() - 1; ++i) {
        if(buffer[i] == '\r' && buffer[i + 1] == '\n') {
            buffer.erase(buffer.begin() + i);
        }
    }
    std::string bufferAsStr = std::string(buffer.begin(), buffer.end());
    buffer.clear();
    buffer.shrink_to_fit();
    std::istringstream ss;
    ss.str(bufferAsStr);
    std::string cur_line{};
    while(std::getline(ss, cur_line)) {
        if(cur_line.empty()) {
            continue;
        }
        if(IsInfoLine(cur_line)) {
            if(!ParseInfoLine(cur_line)) {
                return false;
            }
        } else
            if(IsCommonLine(cur_line)) {
            if(!ParseCommonLine(cur_line)) {
                return false;
            }
        } else
        if(IsPageLine(cur_line)) {
            if(!ParsePageLine(cur_line)) {
                return false;
            }
        } else
        if(IsCharsLine(cur_line)) {
            if(!ParseCharsLine(cur_line)) {
                return false;
            }
        } else
        if(IsCharLine(cur_line)) {
            if(!ParseCharLine(cur_line)) {
                return false;
            }
        } else
        if(IsKerningsLine(cur_line)) {
            if(!ParseKerningsLine(cur_line)) {
                return false;
            }
        } else
        if(IsKerningLine(cur_line)) {
            if(!ParseKerningLine(cur_line)) {
                return false;
            }
        }
    }
    return true;
}

bool KerningFont::IsInfoLine(const std::string& cur_line) {
    return StringUtils::StartsWith(cur_line, "info");
}

bool KerningFont::IsCommonLine(const std::string& cur_line) {
    return StringUtils::StartsWith(cur_line, "common");
}
bool KerningFont::IsPageLine(const std::string& cur_line) {
    return StringUtils::StartsWith(cur_line, "page");
}

bool KerningFont::IsCharsLine(const std::string& cur_line) {
    return StringUtils::StartsWith(cur_line, "chars");
}

bool KerningFont::IsCharLine(const std::string& cur_line) {
    return StringUtils::StartsWith(cur_line, "char");
}

bool KerningFont::IsKerningsLine(const std::string& cur_line) {
    return StringUtils::StartsWith(cur_line, "kernings");
}

bool KerningFont::IsKerningLine(const std::string& cur_line) {
    return StringUtils::StartsWith(cur_line, "kerning");
}

bool KerningFont::ParseInfoLine(const std::string& infoLine) {
    auto key_values = StringUtils::Split(infoLine, ' ', true);
    std::string key{};
    std::string value{};
    for(auto& key_value : key_values) {
        auto cur_key_value = StringUtils::Split(key_value, '=', true);
        if(cur_key_value.size() < 2) {
            continue;
        }
        key = cur_key_value[0];
        value = cur_key_value[1];
        try {
            if(key == "face") {
                _info.face = value;
            }
            if(key == "size") {
                _info.em_size = std::stoi(value);
            }
            if(key == "bold") {
                _info.is_bold = std::stoi(value) != 0;
            }
            if(key == "italic") {
                _info.is_italic = std::stoi(value) != 0;
            }
            if(key == "charset") {
                _info.charset = value;
            }
            if(key == "unicode") {
                _info.charset = std::stoi(value) != 0;
            }
            if(key == "stretchH") {
                _info.stretch_height = std::stof(value) / 100.0f;
            }
            if(key == "smooth") {
                _info.is_smoothed = std::stoi(value) != 0;
            }
            if(key == "aa") {
                _info.is_aliased = std::stoi(value);
            }
            if(key == "padding") {
                auto pads = StringUtils::Split(value, ',', true);
                _info.padding.x = std::stoi(pads[0]);
                _info.padding.y = std::stoi(pads[1]);
                _info.padding.z = std::stoi(pads[2]);
                _info.padding.w = std::stoi(pads[3]);
            }
            if(key == "spacing") {
                auto spaces = StringUtils::Split(value, ',', true);
                _info.spacing.x = std::stoi(spaces[0]);
                _info.spacing.y = std::stoi(spaces[1]);
            }
            if(key == "outline") {
                _info.outline = std::stoi(value);
            }
        } catch(...) {
            return false;
        }
    }
    std::ostringstream ss;
    ss << _info.face << _info.em_size;
    _name = ss.str();
    return true;
}

bool KerningFont::ParseCommonLine(const std::string& commonLine) {
    auto key_values = StringUtils::Split(commonLine, ' ', true);
    std::string key{};
    std::string value{};
    for(auto& key_value : key_values) {
        auto cur_key_value = StringUtils::Split(key_value, '=', true);
        if(cur_key_value.size() < 2) {
            continue;
        }
        key = cur_key_value[0];
        value = cur_key_value[1];
        try {
            if(key == "lineHeight") {
                _common.line_height = std::stoi(value);
            }
            if(key == "base") {
                _common.base = std::stoi(value);
            }
            if(key == "scaleW") {
                _common.scale.x = std::stoi(value);
            }
            if(key == "scaleH") {
                _common.scale.y = std::stoi(value);
            }
            if(key == "pages") {
                _common.page_count = std::stoi(value);
            }
            if(key == "packed") {
                _common.is_packed = std::stoi(value) != 0;
            }
            if(key == "alphaChnl") {
                _common.alpha_channel = std::stoi(value);
            }
            if(key == "redChnl") {
                _common.red_channel = std::stoi(value);
            }
            if(key == "greenChnl") {
                _common.green_channel = std::stoi(value);
            }
            if(key == "blueChnl") {
                _common.blue_channel = std::stoi(value);
            }
        } catch(...) {
            return false;
        }
    }
    _image_paths.resize(_common.page_count);
    return true;
}

bool KerningFont::ParsePageLine(const std::string& pageLine) {
    auto key_values = StringUtils::Split(pageLine, ' ', true);
    std::string key{};
    std::string value{};
    std::size_t id = 0;
    std::string file{};
    for(auto& key_value : key_values) {
        auto cur_key_value = StringUtils::Split(key_value, '=', true);
        if(cur_key_value.size() < 2) {
            continue;
        }
        key = cur_key_value[0];
        value = cur_key_value[1];
        try {
            if(key == "id") {
                id = std::stoi(value);
            }
            if(key == "file") {
                if(*value.begin() == '"') {
                    value.erase(value.begin());
                }
                if(value.back() == '"') {
                    value.pop_back();
                }
                file = value;
            }
        } catch(...) {
            return false;
        }
    }
    _image_paths[id] = file;
    return true;
}

bool KerningFont::ParseCharsLine(const std::string& charsLine) {
    auto key_values = StringUtils::Split(charsLine, ' ', true);
    std::string key{};
    std::string value{};
    for(auto& key_value : key_values) {
        auto cur_key_value = StringUtils::Split(key_value, '=', true);
        if(cur_key_value.size() < 2) {
            continue;
        }
        key = cur_key_value[0];
        value = cur_key_value[1];
        try {
            if(key == "count") {
                _char_count = std::stoi(value);
            }
        } catch(...) {
            return false;
        }
    }
    return true;
}

bool KerningFont::ParseCharLine(const std::string& charLine) {
    auto key_values = StringUtils::Split(charLine, ' ', true);
    std::string key{};
    std::string value{};
    CharDef def{};
    for(auto& key_value : key_values) {
        auto cur_key_value = StringUtils::Split(key_value, '=', true);
        if(cur_key_value.size() < 2) {
            continue;
        }
        key = cur_key_value[0];
        value = cur_key_value[1];
        try {
            if(key == "id") {
                def.id = std::stoi(value);
            }
            if(key == "x") {
                def.position.x = std::stoi(value);
            }
            if(key == "y") {
                def.position.y = std::stoi(value);
            }
            if(key == "width") {
                def.dimensions.x = std::stoi(value);
            }
            if(key == "height") {
                def.dimensions.y = std::stoi(value);
            }
            if(key == "xoffset") {
                def.offsets.x = std::stoi(value);
            }
            if(key == "yoffset") {
                def.offsets.y = std::stoi(value);
            }
            if(key == "xadvance") {
                def.xadvance = std::stoi(value);
            }
            if(key == "page") {
                def.page_id = std::stoi(value);
            }
            if(key == "chnl") {
                def.channel_id = std::stoi(value);
            }
        } catch(...) {
            return false;
        }
    }
    _charmap.insert_or_assign(def.id, def);
    return true;
}

bool KerningFont::ParseKerningsLine(const std::string& kerningsLine) {
    auto key_values = StringUtils::Split(kerningsLine, ' ', true);
    std::string key{};
    std::string value{};
    for(auto& key_value : key_values) {
        auto cur_key_value = StringUtils::Split(key_value, '=', true);
        if(cur_key_value.size() < 2) {
            continue;
        }
        key = cur_key_value[0];
        value = cur_key_value[1];
        try {
            if(key == "count") {
                _kerns_count = std::stoi(value);
            }
        } catch(...) {
            return false;
        }
    }
    return true;

}

bool KerningFont::ParseKerningLine(const std::string& kerningLine) {
    auto key_values = StringUtils::Split(kerningLine, ' ', true);
    std::string key{};
    std::string value{};
    KerningDef def{};
    for(auto& key_value : key_values) {
        auto cur_key_value = StringUtils::Split(key_value, '=', true);
        if(cur_key_value.size() < 2) {
            continue;
        }
        key = cur_key_value[0];
        value = cur_key_value[1];
        try {
            if(key == "first") {
                def.first = std::stoi(value);
            }
            if(key == "second") {
                def.second = std::stoi(value);
            }
            if(key == "amount") {
                def.amount = std::stoi(value);
            }
        } catch(...) {
            return false;
        }
        _kernmap.insert_or_assign(std::make_pair(def.first, def.second), def.amount);
    }
    return true;
}

bool KerningFont::LoadFromXml(std::vector<unsigned char>& buffer) {
    tinyxml2::XMLDocument doc;
    for(std::size_t i = 0; i < buffer.size() - 1; ++i) {
        if(buffer[i] == '\r' && buffer[i + 1] == '\n') {
            buffer.erase(buffer.begin() + i);
        }
    }
    std::string file(buffer.begin(), buffer.end());
    buffer.clear();
    buffer.shrink_to_fit();
    auto result = doc.Parse(file.c_str(), file.size());
    if(result != tinyxml2::XML_SUCCESS) {
        return false;
    }

    auto xml_root = doc.RootElement();
    if(xml_root == nullptr) {
        return false;
    }

    DataUtils::ValidateXmlElement(*xml_root, "font", "info,common,pages,chars", "", "kernings");

    auto xml_info = xml_root->FirstChildElement("info");
    if(xml_info == nullptr) {
        return false;
    }

    _info.face = DataUtils::ParseXmlAttribute(*xml_info, "face", _info.face);
    _info.em_size = DataUtils::ParseXmlAttribute(*xml_info, "size", _info.em_size);
    _info.is_bold = DataUtils::ParseXmlAttribute(*xml_info, "bold", _info.is_bold);
    _info.is_italic = DataUtils::ParseXmlAttribute(*xml_info, "italic", _info.is_italic);
    _info.charset = DataUtils::ParseXmlAttribute(*xml_info, "charset", _info.charset);
    _info.is_unicode = DataUtils::ParseXmlAttribute(*xml_info, "unicode", _info.is_unicode);
    _info.stretch_height = DataUtils::ParseXmlAttribute(*xml_info, "stretchH", _info.stretch_height) / 100.0f;
    _info.is_smoothed = DataUtils::ParseXmlAttribute(*xml_info, "smooth", _info.is_smoothed);
    
    {
    std::ostringstream ss;
    ss << _info.face << _info.em_size;
    _name = ss.str();
    }

    {
        std::string padding_str{};
        auto padding = StringUtils::Split(DataUtils::ParseXmlAttribute(*xml_info, "padding", padding_str));
        ASSERT_OR_DIE(padding.size() == 4, "FONT FORMAT INFO PADDING CHANGED");
        _info.padding.x = std::stoi(padding[0]);
        _info.padding.y = std::stoi(padding[1]);
        _info.padding.z = std::stoi(padding[2]);
        _info.padding.w = std::stoi(padding[3]);
    }

    {
        std::string spacing_str{};
        auto spacing = StringUtils::Split(DataUtils::ParseXmlAttribute(*xml_info, "spacing", spacing_str));
        ASSERT_OR_DIE(spacing.size() == 2, "FONT FORMAT INFO SPACING CHANGED");
        _info.spacing.x = std::stoi(spacing[0]);
        _info.spacing.y = std::stoi(spacing[1]);
    }

    _info.outline = DataUtils::ParseXmlAttribute(*xml_info, "outline", _info.outline);


    auto xml_common = xml_root->FirstChildElement("common");
    if(xml_common == nullptr) {
        return false;
    }

    _common.line_height = DataUtils::ParseXmlAttribute(*xml_common, "lineHeight", _common.line_height);
    _common.base = DataUtils::ParseXmlAttribute(*xml_common, "base", _common.base);
    _common.scale.x = DataUtils::ParseXmlAttribute(*xml_common, "scaleW", _common.scale.x);
    _common.scale.y = DataUtils::ParseXmlAttribute(*xml_common, "scaleH", _common.scale.y); 
    _common.page_count = DataUtils::ParseXmlAttribute(*xml_common, "pages", _common.page_count);
    _common.is_packed = DataUtils::ParseXmlAttribute(*xml_common, "packed", _common.is_packed);
    _common.alpha_channel = DataUtils::ParseXmlAttribute(*xml_common, "alphaChnl", _common.alpha_channel);
    _common.red_channel = DataUtils::ParseXmlAttribute(*xml_common, "redChnl", _common.red_channel);
    _common.green_channel = DataUtils::ParseXmlAttribute(*xml_common, "greenChnl", _common.green_channel);
    _common.blue_channel = DataUtils::ParseXmlAttribute(*xml_common, "blueChnl", _common.blue_channel);

    auto xml_pages = xml_root->FirstChildElement("pages");
    if(xml_pages == nullptr) {
        return false;
    }

    _image_paths.resize(_common.page_count);
    {//Scope constraint
        namespace FS = std::experimental::filesystem;
        FS::path p{};
        for(auto xml_page = xml_pages->FirstChildElement("page");
            xml_page != nullptr;
            xml_page = xml_page->NextSiblingElement("page"))
        {
            unsigned int page_id = DataUtils::ParseXmlAttribute(*xml_page, "id", 0u);
            FS::path page_file = xml_page->Attribute("file");
            p += page_file;
            _image_paths[page_id] = p.string();
        }
    }

    auto xml_chars = xml_root->FirstChildElement("chars");
    if(xml_chars == nullptr) {
        return false;
    }
    _char_count = DataUtils::ParseXmlAttribute(*xml_chars, "count", _char_count);

    for(auto xml_char = xml_chars->FirstChildElement("char");
        xml_char != nullptr;
        xml_char = xml_char->NextSiblingElement("char"))
    {
        int id = DataUtils::ParseXmlAttribute(*xml_char, "id", 0u);
        CharDef t;
        t.position.x = DataUtils::ParseXmlAttribute(*xml_char, "x", t.position.x);
        t.position.y = DataUtils::ParseXmlAttribute(*xml_char, "y", t.position.y);
        t.dimensions.x = DataUtils::ParseXmlAttribute(*xml_char, "width", t.dimensions.x);
        t.dimensions.y = DataUtils::ParseXmlAttribute(*xml_char, "height", t.dimensions.y);
        t.offsets.x = DataUtils::ParseXmlAttribute(*xml_char, "xoffset", t.offsets.x);
        t.offsets.y = DataUtils::ParseXmlAttribute(*xml_char, "yoffset", t.offsets.y);
        t.xadvance = DataUtils::ParseXmlAttribute(*xml_char, "xadvance", t.xadvance);
        t.page_id = DataUtils::ParseXmlAttribute(*xml_char, "page", t.page_id);
        t.channel_id = DataUtils::ParseXmlAttribute(*xml_char, "chnl", t.channel_id);

        _charmap.insert_or_assign(id, t);
    }


    auto xml_kernings = xml_root->FirstChildElement("kernings");
    if(xml_kernings) {
        _kerns_count = DataUtils::ParseXmlAttribute(*xml_kernings, "count", _char_count);
        for(auto xml_kern = xml_kernings->FirstChildElement("kerning"); xml_kern != nullptr; xml_kern = xml_kern->NextSiblingElement("kerning")) {
            KerningDef def{};
            def.first = DataUtils::ParseXmlAttribute(*xml_kernings, "first", def.first);
            def.second = DataUtils::ParseXmlAttribute(*xml_kernings, "second", def.second);
            def.amount = DataUtils::ParseXmlAttribute(*xml_kernings, "amount", def.amount);

            _kernmap.insert_or_assign(std::make_pair(def.first, def.second), def.amount);
        }
    }

    return true;
}

//TODO: LoadFontFromBinary
bool KerningFont::LoadFromBinary(std::vector<unsigned char>& buffer) {
    //See http://www.angelcode.com/products/bmfont/doc/file_format.html#bin
    //for specifics regarding layout

    constexpr const uint8_t CURRENT_BMF_VERSION = 3;

    uint8_t* cur_position = buffer.data();
    bool is_bmf = IsBmfFileBinary(cur_position);
    if(!is_bmf) {
        buffer.clear();
        buffer.shrink_to_fit();
        std::ostringstream ss;
        ss << _filepath << " is not a BMFont file.\n";
        DebuggerPrintf(ss.str().c_str());
        return false;
    }
    cur_position += 3;
    bool is_correct_version = IsCorrectBmfVersionBinary(cur_position, CURRENT_BMF_VERSION);
    if(!is_correct_version) {
        buffer.clear();
        buffer.shrink_to_fit();
        DebuggerPrintf("BMF VERSION HAS CHANGED. See http://www.angelcode.com/products/bmfont/doc/file_format.html#bin for updates!\n");
        return false;
    }
    cur_position += 1;
    int32_t info_block_size = 0;
    int32_t common_block_size = 0;
    int32_t pages_block_size = 0;
    int32_t chars_block_size = 0;
    //int32_t kerning_block_size = 0;
    cur_position = ParseInfoBlockBinary(cur_position, info_block_size);
    cur_position = ParseCommonBlockBinary(cur_position, common_block_size);
    cur_position = ParsePagesBlockBinary(cur_position, pages_block_size, (uint16_t)_common.page_count);
    cur_position = ParseCharsBlockBinary(cur_position, chars_block_size);
    buffer.clear();
    buffer.shrink_to_fit();
    cur_position = nullptr;
    return false;
    //std::size_t current_size = info_block_size + common_block_size + pages_block_size + chars_block_size;
    //std::size_t total_size = buffer.size();
    //if(current_size < total_size) {
    //    cur_position = ParseKerningBlockBinary(cur_position, kerning_block_size);
    //}
    //buffer.clear();
    //buffer.shrink_to_fit();
    //cur_position = nullptr;
    //return false;
}

bool KerningFont::IsBmfFileBinary(uint8_t*& cur_position) {
    bool is_bmf = (*cur_position) == 'B' && (*(cur_position + 1)) == 'M' && (*(cur_position + 2)) == 'F';
    return is_bmf;
}

bool KerningFont::IsCorrectBmfVersionBinary(uint8_t*& cur_position, const uint8_t CURRENT_BMF_VERSION) {
    bool is_correct_ver = (*cur_position) == CURRENT_BMF_VERSION;
    return is_correct_ver;
}

uint8_t* KerningFont::ParseInfoBlockBinary(uint8_t*& cur_position, int32_t& blockSize) {
    uint8_t block_type = *((uint8_t*)cur_position);
    if(block_type != 1) {
        std::ostringstream ss;
        ss << __FUNCTION__ << ": block is not an info block.\n";
        DebuggerPrintf(ss.str().c_str());
        return cur_position;
    }
    cur_position += 1;
    int32_t block_size = *((int32_t*)cur_position);
    blockSize = block_size;
    cur_position = (uint8_t*)(((int32_t*)cur_position) + 1);
    int16_t font_size = *((int16_t*)cur_position);
    cur_position = (uint8_t*)(((int16_t*)cur_position) + 1);
    uint8_t bit_field = *((uint8_t*)cur_position);
    cur_position += 1;
    uint8_t char_set = *((uint8_t*)cur_position);
    cur_position += 1;
    uint16_t stretch_height = *((uint16_t*)cur_position);
    cur_position = (uint8_t*)(((uint16_t*)cur_position) + 1);
    uint8_t aliasing = *((uint8_t*)cur_position);
    cur_position += 1;
    uint8_t paddingUp = *((uint8_t*)cur_position);
    cur_position += 1;
    uint8_t paddingRight = *((uint8_t*)cur_position);
    cur_position += 1;
    uint8_t paddingDown = *((uint8_t*)cur_position);
    cur_position += 1;
    uint8_t paddingLeft = *((uint8_t*)cur_position);
    cur_position += 1;
    uint8_t spacingHoriz = *((uint8_t*)cur_position);
    cur_position += 1;
    uint8_t spacingVert = *((uint8_t*)cur_position);
    cur_position += 1;
    uint8_t outline = *((uint8_t*)cur_position);
    cur_position += 1;
    std::size_t font_name_length = 0;
    uint8_t* font_name_start = cur_position;
    while(*cur_position != '\0') {
        ++font_name_length;
        ++cur_position;
    }
    ++cur_position;
    std::string font_name = std::string(font_name_start, font_name_start + font_name_length);

    _info.face = font_name;
    _info.charset = char_set;
    _info.em_size = font_size;
    _info.is_aliased = aliasing != 0;
    _info.is_smoothed = (bit_field & 0b1000'0000) != 0;
    _info.is_unicode = (bit_field & 0b0100'0000) != 0;
    _info.is_italic = (bit_field & 0b0010'0000) != 0;
    _info.is_bold = (bit_field & 0b0001'0000) != 0;
    _info.is_fixedHeight = (bit_field & 0b0000'1000) != 0;
    _info.padding = IntVector4(paddingUp, paddingRight, paddingDown, paddingLeft);
    _info.spacing = IntVector2(spacingHoriz, spacingVert);
    _info.outline = outline;
    _info.stretch_height = stretch_height / 100.0f;

    {
    std::ostringstream ss;
    ss << _info.face << _info.em_size;
    _name = ss.str();
    }

    return cur_position;
}

uint8_t* KerningFont::ParseCommonBlockBinary(uint8_t*& cur_position, int32_t& blockSize) {
    uint8_t block_type = *((uint8_t*)cur_position);
    if(block_type != 2) {
        std::ostringstream ss;
        ss << __FUNCTION__ << ": block is not a common block.\n";
        DebuggerPrintf(ss.str().c_str());
        return cur_position;
    }
    cur_position += 1;
    int32_t block_size = *((int32_t*)cur_position);
    blockSize = block_size;
    cur_position = (uint8_t*)(((uint32_t*)cur_position) + 1);

    uint16_t line_height = *((uint16_t*)cur_position);
    cur_position = (uint8_t*)(((uint16_t*)cur_position) + 1);
    uint16_t base = *((uint16_t*)cur_position);
    cur_position = (uint8_t*)(((uint16_t*)cur_position) + 1);
    uint16_t scaleW = *((uint16_t*)cur_position);
    cur_position = (uint8_t*)(((uint16_t*)cur_position) + 1);
    uint16_t scaleH = *((uint16_t*)cur_position);
    cur_position = (uint8_t*)(((uint16_t*)cur_position) + 1);
    uint16_t pages = *((uint16_t*)cur_position);
    cur_position = (uint8_t*)(((uint16_t*)cur_position) + 1);
    uint8_t bit_field = *((uint8_t*)cur_position);
    cur_position += 1;
    uint8_t alpha_channel = *((uint8_t*)cur_position);
    cur_position += 1;
    uint8_t red_channel = *((uint8_t*)cur_position);
    cur_position += 1;
    uint8_t green_channel = *((uint8_t*)cur_position);
    cur_position += 1;
    uint8_t blue_channel = *((uint8_t*)cur_position);
    cur_position += 1;

    _common.alpha_channel = alpha_channel;
    _common.base = base;
    _common.blue_channel = blue_channel;
    _common.green_channel = green_channel;
    _common.is_packed = (bit_field & 0b00000001) != 0;
    _common.line_height = line_height;
    _common.page_count = pages;
    _common.red_channel = red_channel;
    _common.scale.x = scaleW;
    _common.scale.y = scaleH;

    return cur_position;
}

uint8_t* KerningFont::ParsePagesBlockBinary(uint8_t* cur_position, int32_t& pages_block_size, uint16_t page_count) {
    pages_block_size = 0;
    _image_paths.resize(page_count);
    if(page_count == 0) {
        std::ostringstream ss;
        ss << __FUNCTION__ << ": No pages to parse.\n";
        DebuggerPrintf(ss.str().c_str());
        return cur_position;
    }

    uint8_t block_type = *((uint8_t*)cur_position);
    if(block_type != 3) {
        std::ostringstream ss;
        ss << __FUNCTION__ << ": Not a pages block.\n";
        DebuggerPrintf(ss.str().c_str());
        return cur_position;
    }
    cur_position += 1;
    int32_t block_size = *((int32_t*)cur_position);
    cur_position = (uint8_t*)(((int32_t*)cur_position) + 1);
    pages_block_size = block_size;
    uint8_t page_name_length = (uint8_t)(((uint32_t)block_size / (uint32_t)page_count) - (uint32_t)1);
    uint8_t* page_start = cur_position;
    for(std::size_t i = 0; i < page_count; ++i) {
        _image_paths[i] = std::string(page_start, page_start + page_name_length);
        cur_position += (page_name_length + 1); //Account for null-terminator
        page_start = cur_position;
    }
    return cur_position;
}

uint8_t* KerningFont::ParseCharsBlockBinary(uint8_t*& cur_position, int32_t& chars_block_size) {
    uint8_t block_type = *((uint8_t*)cur_position);
    if(block_type != 4) {
        std::ostringstream ss;
        ss << __FUNCTION__ << ": Not a chars block.\n";
        DebuggerPrintf(ss.str().c_str());
        return cur_position;
    }

    cur_position += 1;
    int32_t block_size = *((int32_t*)cur_position);
    cur_position = (uint8_t*)(((int32_t*)cur_position) + 1);
    chars_block_size = block_size;
    _char_count = static_cast<std::size_t>(block_size / (uint32_t)20);
    
    for(std::size_t i = 0; i < _char_count; ++i) {
        uint32_t id = *(uint32_t*)(cur_position);
        cur_position = (uint8_t*)(((uint32_t*)cur_position) + 1);
        uint16_t x = *(uint16_t*)(cur_position);
        cur_position = (uint8_t*)(((uint16_t*)cur_position) + 1);
        uint16_t y = *(uint16_t*)(cur_position);
        cur_position = (uint8_t*)(((uint16_t*)cur_position) + 1);
        uint16_t width = *(uint16_t*)(cur_position);
        cur_position = (uint8_t*)(((uint16_t*)cur_position) + 1);
        uint16_t height = *(uint16_t*)(cur_position);
        cur_position = (uint8_t*)(((uint16_t*)cur_position) + 1);
        int16_t xoffset = *(int16_t*)(cur_position);
        cur_position = (uint8_t*)(((int16_t*)cur_position) + 1);
        int16_t yoffset = *(int16_t*)(cur_position);
        cur_position = (uint8_t*)(((int16_t*)cur_position) + 1);
        int16_t xadvance = *(int16_t*)(cur_position);
        cur_position = (uint8_t*)(((uint16_t*)cur_position) + 1);
        uint8_t page = *(uint8_t*)(cur_position);
        cur_position = (uint8_t*)(((uint8_t*)cur_position) + 1);
        uint8_t channel = *(uint8_t*)(cur_position);
        cur_position = (uint8_t*)(((uint8_t*)cur_position) + 1);

        CharDef d{};
        d.id = id;
        d.position = IntVector2(x, y);
        d.dimensions = IntVector2(width, height);
        d.offsets = IntVector2(xoffset, yoffset);
        d.xadvance = xadvance;
        d.page_id = page;
        d.channel_id = channel;

        _charmap.insert_or_assign(d.id, d);
    }
    return cur_position;
}

uint8_t* KerningFont::ParseKerningBlockBinary(uint8_t*& cur_position, int32_t& kerning_block_size) {
    uint8_t block_type = *((uint8_t*)cur_position);
    if(block_type != 5) {
        std::ostringstream ss;
        ss << __FUNCTION__ << ": Not a kernings block.\n";
        DebuggerPrintf(ss.str().c_str());
        return cur_position;
    }

    cur_position += 1;
    int32_t block_size = *((int32_t*)cur_position);
    cur_position = (uint8_t*)(((int32_t*)cur_position) + 1);
    kerning_block_size = block_size;
    _kerns_count = static_cast<std::size_t>(block_size / (uint32_t)10);
    for(std::size_t i = 0; i < _kerns_count; ++i) {
        uint32_t first = *(uint32_t*)cur_position;
        cur_position = (uint8_t*)(((uint32_t*)cur_position) + 1);
        uint32_t second = *(uint32_t*)cur_position;
        cur_position = (uint8_t*)(((uint32_t*)cur_position) + 1);
        int16_t amount = *(int16_t*)cur_position;
        cur_position = (uint8_t*)(((uint16_t*)cur_position) + 1);
        _kernmap.insert_or_assign(std::make_pair(first, second), amount);
    }

    return cur_position;
}
