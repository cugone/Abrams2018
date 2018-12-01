#pragma once

#include <string>
#include <vector>

class Vector2;
class Vector3;
class Vector4;
class Matrix4;
class Rgba;

namespace StringUtils {

std::string to_string(const Vector2& v);
std::string to_string(const Vector3& v);
std::string to_string(const Vector4& v);
std::string to_string(const Matrix4& m);
std::string to_string(const Rgba& clr);

const std::string Stringf(const char* format, ...);
const std::string Stringf(const int maxLength, const char* format, ...);

std::vector<std::string> Split(const std::string& string, char delim = ',', bool skip_empty = true);
std::string Join(const std::vector<std::string>& strings, char delim, bool skip_empty = true);
std::string Join(const std::vector<std::string>& strings, bool skip_empty = true);

std::string ToUpperCase(std::string string);
std::string ToLowerCase(std::string string);

std::string ConvertUnicodeToMultiByte(const std::wstring& unicode_string);
std::wstring ConvertMultiByteToUnicode(const std::string& filePath);

bool StartsWith(const std::string& string, const std::string& start);
bool EndsWith(const std::string& string, const std::string& end);

std::string TrimWhitespace(std::string string);

constexpr const uint32_t FourCC(const char* id) {
    return static_cast<uint32_t>((((id[0] << 24) & 0xFF000000) | ((id[1] << 16) & 0x00FF0000) | ((id[2] << 8) & 0x0000FF00) | ((id[3] << 0) & 0x000000FF)));
}

}