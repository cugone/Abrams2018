#include "Engine/Core/Rgba.hpp"

#include "Engine/Core/StringUtils.hpp"

#include "Engine/Math/MathUtils.hpp"

#include "Engine/Profiling/ProfileLogScope.hpp"

const Rgba Rgba::WHITE(255, 255, 255, 255);
const Rgba Rgba::BLACK(0, 0, 0, 255);
const Rgba Rgba::RED(255, 0, 0, 255);
const Rgba Rgba::GREEN(0, 255, 0, 255);
const Rgba Rgba::FOREST_GREEN(34, 139, 34, 255);
const Rgba Rgba::BLUE(0, 0, 255, 255);
const Rgba Rgba::NAVY_BLUE(0, 0, 128, 255);
const Rgba Rgba::CYAN(0, 255, 255, 255);
const Rgba Rgba::YELLOW(255, 255, 0, 255);
const Rgba Rgba::MAGENTA(255, 0, 255, 255);
const Rgba Rgba::ORANGE(255, 165, 0, 255);
const Rgba Rgba::VIOLET(143, 0, 255, 255);
const Rgba Rgba::GREY(128, 128, 128, 255);
const Rgba Rgba::GRAY(128, 128, 128, 255);
const Rgba Rgba::LIGHT_GREY(192, 192, 192, 255);
const Rgba Rgba::LIGHT_GRAY(192, 192, 192, 255);
const Rgba Rgba::DARK_GREY(64, 64, 64, 255);
const Rgba Rgba::DARK_GRAY(64, 64, 64, 255);
const Rgba Rgba::OLIVE(107, 142, 35, 255);
const Rgba Rgba::SKY_BLUE(45, 185, 255, 255);
const Rgba Rgba::LIME(227, 255, 0, 255);
const Rgba Rgba::TEAL(0, 128, 128, 255);
const Rgba Rgba::TURQUOISE(64, 224, 208, 255);
const Rgba Rgba::PERIWINKLE(204, 204, 255, 255);
const Rgba Rgba::NORMAL_Z(128, 128, 255, 255);
const Rgba Rgba::NOALPHA(0, 0, 0, 0);

Rgba Rgba::Random() {
    return Rgba(static_cast<unsigned char>(MathUtils::GetRandomIntLessThan(256))
                , static_cast<unsigned char>(MathUtils::GetRandomIntLessThan(256))
                , static_cast<unsigned char>(MathUtils::GetRandomIntLessThan(256))
                ,255);
}

Rgba Rgba::RandomGreyscale() {
    auto r = static_cast<unsigned char>(MathUtils::GetRandomIntLessThan(256));
    return Rgba(r, r, r, 255);
}

Rgba Rgba::RandomGrayscale() {
    return RandomGreyscale();
}

Rgba Rgba::RandomWithAlpha() {
    return Rgba(static_cast<unsigned char>(MathUtils::GetRandomIntLessThan(256))
                , static_cast<unsigned char>(MathUtils::GetRandomIntLessThan(256))
                , static_cast<unsigned char>(MathUtils::GetRandomIntLessThan(256))
                , static_cast<unsigned char>(MathUtils::GetRandomIntLessThan(256)));
}

Rgba::Rgba(std::string name) {
    name = StringUtils::ToUpperCase(name);

    std::size_t hash_loc = name.find_first_of('#');
    if(hash_loc != std::string::npos) {
        name.replace(hash_loc, 1, "0X");
        std::size_t char_count = 0;
        unsigned long value_int = std::stoul(name, &char_count, 16);
        if(char_count == 10) { //0xRRGGBBAA
            SetRGBAFromRawValue(value_int);
        } else if(char_count == 8) { //0xRRGGBB
            SetRGBFromRawValue(value_int);
            a = 255;
        } else {
            /* DO NOTHING */
        }
    } else {
        std::vector<std::string> v = StringUtils::Split(name);
        std::size_t v_s = v.size();
        if(v_s > 1) {
            if(!(v_s < 3)) {
                r = static_cast<unsigned char>(std::stoi(v[0].data()));
                g = static_cast<unsigned char>(std::stoi(v[1].data()));
                b = static_cast<unsigned char>(std::stoi(v[2].data()));
                if(v_s > 3) {
                    a = static_cast<unsigned char>(std::stoi(v[3].data()));
                }
            }
        } else {
            SetValueFromName(v_s ? v[0] : "");
        }
    }
}

Rgba::Rgba(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha /*= 0xFF*/)
    : r(red)
    , g(green)
    , b(blue)
    , a(alpha)
{
    /* DO NOTHING */
}

void Rgba::SetAsBytes(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha) {
    r = red;
    g = green;
    b = blue;
    a = alpha;
}

void Rgba::SetAsFloats(float normalized_red, float normalized_green, float normalized_blue, float normalized_alpha) {
    r = static_cast<unsigned char>(normalized_red * 255.0f);
    g = static_cast<unsigned char>(normalized_green * 255.0f);
    b = static_cast<unsigned char>(normalized_blue * 255.0f);
    a = static_cast<unsigned char>(normalized_alpha * 255.0f);
}

void Rgba::GetAsFloats(float& out_normalized_red, float& out_normalized_green, float& out_normalized_blue, float& out_normalized_alpha) const {
    out_normalized_red   = r / 255.0f;
    out_normalized_green = g / 255.0f;
    out_normalized_blue  = b / 255.0f;
    out_normalized_alpha = a / 255.0f;
}

Vector4 Rgba::GetRgbaAsFloats() const {
    return Vector4{r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f};
}

Vector3 Rgba::GetRgbAsFloats() const {
    return Vector3{ r / 255.0f, g / 255.0f, b / 255.0f};
}

void Rgba::ScaleRGB(float scale) {
    float scaled_red = static_cast<float>(r) * scale;
    float scaled_green = static_cast<float>(g) * scale;
    float scaled_blue = static_cast<float>(b) * scale;
    r = static_cast<unsigned char>(MathUtils::Clamp<float>(scaled_red, 0.0f, 255.0f));
    g = static_cast<unsigned char>(MathUtils::Clamp<float>(scaled_green, 0.0f, 255.0f));
    b = static_cast<unsigned char>(MathUtils::Clamp<float>(scaled_blue, 0.0f, 255.0f));

}

void Rgba::ScaleAlpha(float scale) {
    float scaled_alpha = static_cast<float>(a) * scale;
    a = static_cast<unsigned char>(MathUtils::Clamp<float>(scaled_alpha, 0.0f, 255.0f));
}

unsigned int Rgba::GetAsRawValue() const {
    return static_cast<unsigned int>(  ((static_cast<unsigned int>(r) << 24) & 0xFF000000)
                                     | ((static_cast<unsigned int>(g) << 16) & 0x00FF0000)
                                     | ((static_cast<unsigned int>(b) << 8)  & 0x0000FF00)
                                     | ((static_cast<unsigned int>(a) << 0)  & 0x000000FF));
}

void Rgba::SetFromRawValue(unsigned long value) {
    SetRGBAFromRawValue(value);
}

void Rgba::SetRGBAFromRawValue(unsigned long value) {
    r = static_cast<unsigned char>((value & 0xFF000000ul) >> 24);
    g = static_cast<unsigned char>((value & 0x00FF0000ul) >> 16);
    b = static_cast<unsigned char>((value & 0x0000FF00ul) >> 8);
    a = static_cast<unsigned char>((value & 0x000000FFul) >> 0);
}

void Rgba::SetRGBFromRawValue(unsigned long value) {
    r = static_cast<unsigned char>((value & 0xff0000ul) >> 16);
    g = static_cast<unsigned char>((value & 0x00ff00ul) >> 8);
    b = static_cast<unsigned char>((value & 0x0000fful) >> 0);
}

void Rgba::SetValueFromName(std::string name) {
    name = StringUtils::ToUpperCase(name);
    if(name == "WHITE") {
        SetFromRawValue(Rgba::WHITE.GetAsRawValue());
    } else if(name == "BLACK") {
        SetFromRawValue(Rgba::BLACK.GetAsRawValue());
    } else if(name == "RED") {
        SetFromRawValue(Rgba::RED.GetAsRawValue());
    } else if(name == "GREEN") {
        SetFromRawValue(Rgba::GREEN.GetAsRawValue());
    } else if(name == "FOREST_GREEN") {
        SetFromRawValue(Rgba::FOREST_GREEN.GetAsRawValue());
    } else if(name == "NAVY_BLUE") {
        SetFromRawValue(Rgba::NAVY_BLUE.GetAsRawValue());
    } else if(name == "CYAN") {
        SetFromRawValue(Rgba::CYAN.GetAsRawValue());
    } else if(name == "YELLOW") {
        SetFromRawValue(Rgba::YELLOW.GetAsRawValue());
    } else if(name == "MAGENTA") {
        SetFromRawValue(Rgba::MAGENTA.GetAsRawValue());
    } else if(name == "ORANGE") {
        SetFromRawValue(Rgba::ORANGE.GetAsRawValue());
    } else if(name == "GREY") {
        SetFromRawValue(Rgba::GREY.GetAsRawValue());
    } else if(name == "GRAY") {
        SetFromRawValue(Rgba::GRAY.GetAsRawValue());
    } else if(name == "LIGHT_GREY") {
        SetFromRawValue(Rgba::LIGHT_GREY.GetAsRawValue());
    } else if(name == "LIGHT_GRAY") {
        SetFromRawValue(Rgba::LIGHT_GRAY.GetAsRawValue());
    } else if(name == "DARK_GREY") {
        SetFromRawValue(Rgba::DARK_GREY.GetAsRawValue());
    } else if(name == "DARK_GRAY") {
        SetFromRawValue(Rgba::DARK_GRAY.GetAsRawValue());
    } else if(name == "OLIVE") {
        SetFromRawValue(Rgba::OLIVE.GetAsRawValue());
    } else if(name == "SKY_BLUE") {
        SetFromRawValue(Rgba::SKY_BLUE.GetAsRawValue());
    } else if(name == "LIME") {
        SetFromRawValue(Rgba::LIME.GetAsRawValue());
    } else if(name == "TEAL") {
        SetFromRawValue(Rgba::TEAL.GetAsRawValue());
    } else if(name == "TURQUOISE") {
        SetFromRawValue(Rgba::TURQUOISE.GetAsRawValue());
    } else if(name == "PERIWINKLE") {
        SetFromRawValue(Rgba::PERIWINKLE.GetAsRawValue());
    } else if(name == "NORMAL_Z") {
        SetFromRawValue(Rgba::NORMAL_Z.GetAsRawValue());
    } else if(name == "NOALPHA") {
        SetFromRawValue(Rgba::NOALPHA.GetAsRawValue());
    }

}

bool Rgba::IsRgbEqual(const Rgba& rhs) const {
    return r == rhs.r && g == rhs.g && b == rhs.b;
}

bool Rgba::operator<(const Rgba& rhs) const {
    return GetAsRawValue() < rhs.GetAsRawValue();
}

bool Rgba::operator!=(const Rgba& rhs) const {
    return !(*this == rhs);
}

bool Rgba::operator==(const Rgba& rhs) const {
    return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a;
}
