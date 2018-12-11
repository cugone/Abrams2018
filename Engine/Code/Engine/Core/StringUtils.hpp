#pragma once

#include <string>
#include <vector>

class Vector2;
class Vector3;
class Vector4;
class Matrix4;
class Rgba;
namespace System::Cpu {
enum class ProcessorArchitecture;
}

namespace StringUtils {

std::string to_string(const Vector2& v);
std::string to_string(const Vector3& v);
std::string to_string(const Vector4& v);
std::string to_string(const Matrix4& m);
std::string to_string(const Rgba& clr);
std::string to_string(const System::Cpu::ProcessorArchitecture& architecture);

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
std::string ReplaceAll(std::string string, const std::string& from, const std::string& to);
std::string TrimWhitespace(std::string string);

constexpr const uint32_t FourCC(const char* id) {
    return static_cast<uint32_t>((((id[0] << 24) & 0xFF000000) | ((id[1] << 16) & 0x00FF0000) | ((id[2] << 8) & 0x0000FF00) | ((id[3] << 0) & 0x000000FF)));
}

namespace Encryption {

//NOT USEFUL AS TRUE ENCRYPTION!! DO NOT USE IF SERIOUS ENCRYPTION IS NEEDED!!!
std::string ROT13(std::string text);

struct encode_tag {};
struct decode_tag {};

//NOT USEFUL AS TRUE ENCRYPTION!! DO NOT USE IF SERIOUS ENCRYPTION IS NEEDED!!!
template<int key, typename Op = encode_tag>
std::string CaesarShift(std::string plaintext) {
    auto caesarshift = [](unsigned char a) {
        bool lower = 'a' <= a && a <= 'z';
        bool upper = 'A' <= a && a <= 'Z';
        char base = lower ? 'a' : upper ? 'A' : 0;
        if(!base) {
            return a;
        }
        int shift_result = 0;
        if(std::is_same_v<Op, encode_tag>) {
            shift_result = (a - base + key) % 26;
        } else if(std::is_same_v<Op, decode_tag>) {
            shift_result = (a - base - key) % 26;
        }
        if(shift_result < 0) {
            shift_result += 26;
        }
        if(25 < shift_result) {
            shift_result -= 26;
        }
        return static_cast<unsigned char>(static_cast<char>(base + shift_result));
    };
    std::transform(std::begin(plaintext), std::end(plaintext), std::begin(plaintext), caesarshift);
    return plaintext;
}

} //End Encryption

} //End StringUtils