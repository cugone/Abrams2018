#include "Engine/Core/KerningFont.hpp"

#include "Engine/Core/DataUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"

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

bool KerningFont::LoadFromFile(const std::string& filepath) {
    {
        namespace FS = std::experimental::filesystem;
        FS::path path{ filepath };
        bool path_exists = FS::exists(path);
        bool is_not_directory = !FS::is_directory(path);
        bool is_file = FS::is_regular_file(path);
        bool is_fnt = path.has_extension() && path.extension() == ".fnt";
        bool is_valid = path_exists && is_not_directory && is_file && is_fnt;
        if(!is_valid) {
            return false;
        }
    }
    std::vector<unsigned char> out_buffer{};
    FileUtils::ReadBufferFromFile(out_buffer, filepath);
    if(out_buffer.size() < 4) {
        return false;
    }

    bool is_binary = out_buffer[0] == 66 && out_buffer[1] == 77 && out_buffer[2] == 70;
    bool is_text = out_buffer[0] == 105 && out_buffer[1] == 110 && out_buffer[2] == 102 && out_buffer[3] == 111;
    if(is_binary) {
        return LoadFromBinary(out_buffer);
    } else if(is_text) {
        return LoadFromText(out_buffer);
    } else {
        return LoadFromXml(out_buffer);
    }
}

bool KerningFont::LoadFromText(std::vector<unsigned char>& buffer) {
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
        if(cur_line.find_first_of("info") == 0) {
            if(!ParseInfoLine(cur_line)) {
                return false;
            }
        }
        if(cur_line.find_first_of("common") == 0) {
            if(!ParseCommonLine(cur_line)) {
                return false;
            }
        }
        if(cur_line.find_first_of("page") == 0) {
            if(!ParsePageLine(cur_line)) {
                return false;
            }
        }
        if(cur_line.find_first_of("chars") == 0) {
            if(!ParseCharsLine(cur_line)) {
                return false;
            }
        }
        if(cur_line.find_first_of("char") == 0) {
            if(!ParseCharLine(cur_line)) {
                return false;
            }
        }
        if(cur_line.find_first_of("kernings") == 0) {
            if(!ParseKerningsLine(cur_line)) {
                return false;
            }
        }
        if(cur_line.find_first_of("kerning") == 0) {
            if(!ParseKerningLine(cur_line)) {
                return false;
            }
        }
    }
    return true;
}

bool KerningFont::LoadFromXml(std::vector<unsigned char>& buffer) {
    tinyxml2::XMLDocument doc;
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
        return false;
    }
    bool is_correct_version = IsCorrectBmfVersionBinary(cur_position, CURRENT_BMF_VERSION);
    if(!is_correct_version) {
        buffer.clear();
        buffer.shrink_to_fit();
        ERROR_AND_DIE("BMF VERSION HAS CHANGED. See http://www.angelcode.com/products/bmfont/doc/file_format.html#bin for updates!");
    }
    cur_position = ParseInfoBlockBinary(cur_position);
    buffer.clear();
    buffer.shrink_to_fit();
    return false;
/*
Block type 2: common
field	size	type	pos	comment
lineHeight 	2 	uint 	0
base 	2 	uint 	2
scaleW 	2 	uint 	4
scaleH 	2 	uint 	6
pages 	2 	uint 	8
bitField 	1 	bits 	10 	bits 0-6: reserved, bit 7: packed
alphaChnl 	1 	uint 	11
redChnl 	1 	uint 	12
greenChnl 	1 	uint 	13
blueChnl 	1 	uint 	14
*/

/*
Block type 3: pages
field	size	type	pos	comment
pageNames 	p*(n+1) 	strings 	0 	p null terminated strings, each with length n

This block gives the name of each texture file with the image data for the characters. The string pageNames holds the names separated and terminated by null chars. Each filename has the same length, so once you know the size of the first name, you can easily determine the position of each of the names. The id of each page is the zero-based index of the string name.
*/

/*
Block type 4: chars
field	size	type	pos	comment
id 	4 	uint 	0+c*20 	These fields are repeated until all characters have been described
x 	2 	uint 	4+c*20
y 	2 	uint 	6+c*20
width 	2 	uint 	8+c*20
height 	2 	uint 	10+c*20
xoffset 	2 	int 	12+c*20
yoffset 	2 	int 	14+c*20
xadvance 	2 	int 	16+c*20
page 	1 	uint 	18+c*20
chnl 	1 	uint 	19+c*20

The number of characters in the file can be computed by taking the size of the block and dividing with the size of the charInfo structure, i.e.: numChars = charsBlock.blockSize/20.
*/

/*
Block type 5: kerning pairs
field	size	type	pos	comment
first 	4 	uint 	0+c*10 	These fields are repeated until all kerning pairs have been described
second 	4 	uint 	4+c*10
amount 	2 	int 	8+c*6

This block is only in the file if there are any kerning pairs with amount differing from 0.
*/

    //return false;
}

uint8_t* KerningFont::ParseInfoBlockBinary(uint8_t*& cur_position) {
    uint8_t block_type = *((uint8_t*)cur_position);
    if(block_type != 1) {
        ERROR_AND_DIE("Not and info block.");
    }
    cur_position += 1;
    int32_t block_size = *((int32_t*)cur_position);
    block_size;//UNUSED
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
    _info.stretch_height = stretch_height;
    
    return cur_position;
}

bool KerningFont::IsCorrectBmfVersionBinary(uint8_t*& cur_position, const uint8_t CURRENT_BMF_VERSION) {
    bool is_correct_ver = (*cur_position) == CURRENT_BMF_VERSION;
    cur_position++;
    return is_correct_ver;
}

bool KerningFont::IsBmfFileBinary(uint8_t*& cur_position) {
    bool is_bmf = (*cur_position) == 'B' && (*(cur_position + 1)) == 'M' && (*(cur_position + 2)) == 'F';
    cur_position += 3;
    return is_bmf;
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
    }
    return true;
}
