#include "Engine/Core/StringUtils.hpp"

#include <stdarg.h>

#include <algorithm>
#include <locale>
#include <sstream>

#include "Engine/Core/Win.hpp"

const int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;

namespace StringUtils {

const std::string Stringf(const char* format, ...) {
    char textLiteral[STRINGF_STACK_LOCAL_TEMP_LENGTH];
    va_list variableArgumentList;
    va_start(variableArgumentList, format);
    vsnprintf_s(textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList);
    va_end(variableArgumentList);
    textLiteral[STRINGF_STACK_LOCAL_TEMP_LENGTH - 1] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

    return std::string(textLiteral);
}

const std::string Stringf(const int maxLength, const char* format, ...) {
    char textLiteralSmall[STRINGF_STACK_LOCAL_TEMP_LENGTH];
    char* textLiteral = textLiteralSmall;
    if(maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH)
        textLiteral = new char[maxLength];

    va_list variableArgumentList;
    va_start(variableArgumentList, format);
    vsnprintf_s(textLiteral, maxLength, _TRUNCATE, format, variableArgumentList);
    va_end(variableArgumentList);
    textLiteral[maxLength - 1] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

    std::string returnValue(textLiteral);
    if(maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH)
        delete[] textLiteral;

    return returnValue;
}

std::vector<std::string> Split(const std::string& string, char delim /*= ','*/, bool skip_empty /*= true*/) {

    if(string.empty()) {
        return {};
    }

    std::size_t potential_count = 1 + std::count(string.begin(), string.end(), delim);
    std::vector<std::string> result;
    result.reserve(potential_count);
    std::string curString;

    std::stringstream ss;
    ss.str(string);
    ss.seekg(0);
    ss.seekp(0);
    ss.clear();

    while(std::getline(ss, curString, delim)) {
        if(skip_empty && curString.empty()) continue;
        result.push_back(curString);
    }
    result.shrink_to_fit();
    return result;
}

std::string Join(const std::vector<std::string>& strings, char delim, bool skip_empty /*= true*/) {
    std::size_t total_size = 0;
    for(auto& s : strings) {
        if(s.empty()) continue;
        total_size += 1 + s.size();
    }
    std::string result;
    result.reserve(total_size);
    for(auto iter = strings.begin(); iter != strings.end(); ++iter) {
        if(skip_empty && (*iter).empty()) continue;
        result += (*iter);
        if(iter != strings.end() - 1) {
            result.push_back(delim);
        }
    }
    result.shrink_to_fit();
    return result;
}

std::string Join(const std::vector<std::string>& strings, bool skip_empty /*= true*/) {
    std::size_t total_size = 0;
    for(auto& s : strings) {
        if(s.empty()) continue;
        total_size += s.size();
    }
    std::string result;
    result.reserve(total_size);
    for(auto iter = strings.begin(); iter != strings.end(); ++iter) {
        if(skip_empty && (*iter).empty()) continue;
        result += (*iter);
    }
    result.shrink_to_fit();
    return result;
}

std::string ToUpperCase(std::string string) {
    std::transform(string.begin(), string.end(), string.begin(), [](unsigned char c) -> unsigned char { return std::toupper(c, std::locale("")); });
    return string;
}

std::string ToLowerCase(std::string string) {
    std::transform(string.begin(), string.end(), string.begin(), [](unsigned char c) -> unsigned char { return std::tolower(c, std::locale("")); });
    return string;
}

std::string ConvertUnicodeToMultiByte(const std::wstring& unicode_string) {
    char* buf = nullptr;
    auto buf_size = ::WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, unicode_string.data(), -1, buf, 0, nullptr, nullptr);
    std::string mb_string;
    mb_string.assign(buf, buf_size);
    return mb_string;
}

std::wstring ConvertMultiByteToUnicode(const std::string& multi_byte_string) {
    wchar_t* buf = nullptr;
    auto buf_size = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, multi_byte_string.data(), -1, buf, 0);
    std::wstring unicode_string;
    unicode_string.assign(buf, buf_size);
    return unicode_string;
}

bool StartsWith(const std::string& string, const std::string& search) {
    auto found_loc = string.find(search);
    return found_loc != std::string::npos && found_loc == 0;
}

bool EndsWith(const std::string& string, const std::string& search) {
    auto found_loc = string.find(search);
    return found_loc != std::string::npos && found_loc == string.size() - search.size();
}

std::string TrimWhitespace(std::string string) {
    auto first_non_space = string.find_first_not_of(" \n\t\v\f\r");
    auto last_non_space = string.find_last_not_of(" \n\t\v\f\r");
    return string.substr(first_non_space, last_non_space - first_non_space + 1);
}

} //End StringUtils