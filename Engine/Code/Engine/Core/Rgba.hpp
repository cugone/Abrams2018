#pragma once

#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Vector3.hpp"

#include <string>
#include <vector>

class Rgba {
public:

    static const Rgba WHITE;
    static const Rgba BLACK;
    static const Rgba RED;
    static const Rgba GREEN;
    static const Rgba FORESTGREEN;
    static const Rgba BLUE;
    static const Rgba NAVY_BLUE;
    static const Rgba CYAN;
    static const Rgba YELLOW;
    static const Rgba MAGENTA;
    static const Rgba ORANGE;
    static const Rgba VIOLET;
    static const Rgba LIGHT_GREY;
    static const Rgba LIGHT_GRAY;
    static const Rgba GREY;
    static const Rgba GRAY;
    static const Rgba DARK_GREY;
    static const Rgba DARK_GRAY;
    static const Rgba OLIVE;
    static const Rgba SKY_BLUE;
    static const Rgba LIME;
    static const Rgba TEAL;
    static const Rgba TURQUOISE;
    static const Rgba PERIWINKLE;
    static const Rgba NORMAL_Z;
    static const Rgba NOALPHA;

    static Rgba Random();
    static Rgba RandomWithAlpha();

    Rgba() = default;
    Rgba(const Rgba& rhs) = default;
    Rgba(Rgba&& rhs) = default;
    Rgba& operator=(const Rgba& rhs) = default;
    Rgba& operator=(Rgba&& rhs) = default;
    ~Rgba() = default;

    explicit Rgba(std::string name);
    explicit Rgba(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha = 0xFF);

    void SetAsBytes(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);
    void SetAsFloats(float normalized_red, float normalized_green, float normalized_blue, float normalized_alpha);
    void GetAsFloats(float& out_normalized_red, float& out_normalized_green, float& out_normalized_blue, float& out_normalized_alpha) const;
    Vector4 GetRgbaAsFloats() const;
    Vector3 GetRgbAsFloats() const;
    void ScaleRGB(float scale);
    void ScaleAlpha(float scale);

    unsigned int GetAsRawValue() const;
    void SetFromRawValue(unsigned long value);
    void SetRGBAFromRawValue(unsigned long value);
    void SetRGBFromRawValue(unsigned long value);

    bool IsRgbEqual(const Rgba& rhs) const;
    bool operator==(const Rgba& rhs) const;
    bool operator!=(const Rgba& rhs) const;

    unsigned char r = 255;
    unsigned char g = 255;
    unsigned char b = 255;
    unsigned char a = 255;

protected:
private:
    void SetValueFromName(std::string name);
};