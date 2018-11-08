#include "Engine/Core/Base64.hpp"

#include <array>
#include <bitset>
#include <sstream>

namespace FileUtils::Base64 {

std::string Encode(const std::string& input) {
    std::istringstream ss(input);
    return Encode(ss);
}

std::string Encode(const std::vector<unsigned char>& input) {
    std::stringstream ss;
    ss.setf(std::ios_base::binary | std::ios_base::in);
    ss.write(reinterpret_cast<const char*>(input.data()), input.size());
    return Encode(ss);
}

std::string Encode(std::istream& input) {
    std::array<unsigned char, 3> bits{0,0,0};
    std::string output(1024, 0);
    std::size_t i = 0u;
    while(input.read(reinterpret_cast<char*>(bits.data()), bits.size())) {
        if(i >= 1023) {
            output.push_back('\0');
            output.push_back('\0');
            output.push_back('\0');
            output.push_back('\0');
        }
        uint32_t sextets = (0x00 << 24) | (bits[0] << 16) | (bits[1] << 8) | (bits[2] << 0);
        output[i + 0] = detail::base64encodingtable[(sextets >> 18) & 0b0011'1111];
        output[i + 1] = detail::base64encodingtable[(sextets >> 12) & 0b0011'1111];
        output[i + 2] = detail::base64encodingtable[(sextets >> 6) & 0b0011'1111];
        output[i + 3] = detail::base64encodingtable[(sextets >> 0) & 0b0011'1111];
        i += 4;
        bits[0] = 0;
        bits[1] = 0;
        bits[2] = 0;
    }
    if(bits[0] == 0) {
        if(i == 0) { //empty string
            return {};
        }
        //multiple of 3 bytes read
        return output.substr(0, i);
    } else if(bits[1] == 0) { //one byte
        uint32_t sextets = (0x00 << 24) | (bits[0] << 16) | (bits[1] << 8) | (bits[2] << 0);
        if(i == 0) { //exactly
            output[0] = detail::base64encodingtable[(sextets >> 18) & 0b0011'1111];
            output[1] = detail::base64encodingtable[(sextets >> 12) & 0b0011'1111];
            output[2] = detail::base64paddingchar;
            output[3] = detail::base64paddingchar;
        } else {
            output[i + 0] = detail::base64encodingtable[(sextets >> 18) & 0b0011'1111];
            output[i + 1] = detail::base64encodingtable[(sextets >> 12) & 0b0011'1111];
            output[i + 2] = detail::base64paddingchar;
            output[i + 3] = detail::base64paddingchar;
        }
        i += 4;
    } else if(bits[2] == 0) { //two bytes
        uint32_t sextets = (0x00 << 24) | (bits[0] << 16) | (bits[1] << 8) | (bits[2] << 0);
        if(i == 0) { //exactly
            output[0] = detail::base64encodingtable[(sextets >> 18) & 0b0011'1111];
            output[1] = detail::base64encodingtable[(sextets >> 12) & 0b0011'1111];
            output[2] = detail::base64encodingtable[(sextets >> 6) & 0b0011'1111];
            output[3] = detail::base64paddingchar;
        } else {
            output[i + 0] = detail::base64encodingtable[(sextets >> 18) & 0b0011'1111];
            output[i + 1] = detail::base64encodingtable[(sextets >> 12) & 0b0011'1111];
            output[i + 2] = detail::base64encodingtable[(sextets >> 6) & 0b0011'1111];
            output[i + 3] = detail::base64paddingchar;
        }
        i += 4;
    }
    return output.substr(0, i);
}

std::string Decode(const std::string& input) {
    std::istringstream ss(input);
    return Decode(ss);
}

void Decode(const std::string& input, std::vector<unsigned char>& output) {
    std::stringstream ss(std::ios_base::binary);
    ss.write(reinterpret_cast<const char*>(input.data()), input.size());
    output = detail::DecodeBinary(input);
}

std::string Decode(std::istream& input) {
    std::array<char, 4> bits_array{ 0,0,0,0 };
    std::string output(1024, 0);
    std::size_t i = 0u;
    while(input.read(reinterpret_cast<char*>(bits_array.data()), bits_array.size())) {
        if(i >= 1023) {
            output.push_back('\0');
            output.push_back('\0');
            output.push_back('\0');
        }
        auto bits_array_copy = bits_array;
        for(auto& c : bits_array_copy) {
            if(c == '=') {
                c = 0;
                continue;
            }
            if(c == '+') {
                c = 62;
                continue;
            }
            if(c == '/') {
                c = 63;
                continue;
            }
            if(c >= 'A' && c <= 'Z') {
                c = (c - 'A') + 0;
                continue;
            }
            if(c >= 'a' && c <= 'z') {
                c = (c - 'a') + 26;
                continue;
            }
            if(c >= '0' && c <= '9') {
                c = (c - '0') + 52;
                continue;
            }
        }
        uint32_t bits = (
            ((bits_array_copy[0] & 0b0011'1111) << 18)
            | ((bits_array_copy[1] & 0b0011'1111) << 12)
            | ((bits_array_copy[2] & 0b0011'1111) << 6)
            | ((bits_array_copy[3] & 0b0011'1111) << 0)
            );
        output[i + 0] = static_cast<char>((bits >> 16) & 0xFF);
        output[i + 1] = static_cast<char>((bits >> 8)  & 0xFF);
        output[i + 2] = static_cast<char>((bits >> 0)  & 0xFF);
        i += 3;
    }
    if(bits_array[2] == '=') {
        return output.substr(0, i - 2);
    } else if(bits_array[3] == '=') {
        return output.substr(0, i - 1);
    }

    return output.substr(0, i);
}

std::vector<unsigned char> detail::DecodeBinary(std::istream& input) {
    std::array<char, 4> bits_array{ 0,0,0,0 };
    std::vector<unsigned char> output(1024, 0);
    std::size_t i = 0u;
    while(input.read(reinterpret_cast<char*>(bits_array.data()), bits_array.size())) {
        if(i >= 1023) {
            output.push_back('\0');
            output.push_back('\0');
            output.push_back('\0');
        }
        auto bits_array_copy = bits_array;
        for(auto& c : bits_array_copy) {
            if(c == '=') {
                c = 0;
                continue;
            }
            if(c == '.') {
                c = 62;
                continue;
            }
            if(c == '-') {
                c = 63;
                continue;
            }
            if(c >= 'A' && c <= 'Z') {
                c = (c - 'A') + 0;
                continue;
            }
            if(c >= 'a' && c <= 'z') {
                c = (c - 'a') + 26;
                continue;
            }
            if(c >= '0' && c <= '9') {
                c = (c - '0') + 52;
                continue;
            }
        }
        uint32_t bits = (
            ((bits_array_copy[0] & 0b0011'1111) << 18)
            | ((bits_array_copy[1] & 0b0011'1111) << 12)
            | ((bits_array_copy[2] & 0b0011'1111) << 6)
            | ((bits_array_copy[3] & 0b0011'1111) << 0)
            );
        output[i + 0] = static_cast<char>((bits >> 16) & 0xFF);
        output[i + 1] = static_cast<char>((bits >> 8) & 0xFF);
        output[i + 2] = static_cast<char>((bits >> 0) & 0xFF);
        i += 3;
    }
    if(bits_array[2] == '=') {
        decltype(output) result(i - 2, 0);
        std::copy(std::begin(output), std::begin(output) + i - 2, std::begin(result));
        return result;
    } else if(bits_array[3] == '=') {
        decltype(output) result(i - 1, 0);
        std::copy(std::begin(output), std::begin(output) + i - 1, std::begin(result));
        return result;
    }
    decltype(output) result(i, 0);
    std::copy(std::begin(output), std::begin(output) + i, std::begin(result));
    return result;
}

std::vector<unsigned char> detail::DecodeBinary(const std::string& input) {
    std::stringstream ss;
    ss.write(reinterpret_cast<const char*>(input.data()), input.size());
    return detail::DecodeBinary(ss);
}

}