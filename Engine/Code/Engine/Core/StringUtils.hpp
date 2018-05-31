#pragma once

#include <string>
#include <vector>

namespace StringUtils {

const std::string Stringf(const char* format, ...);
const std::string Stringf(const int maxLength, const char* format, ...);

std::vector<std::string> Split(const std::string& string, char delim = ',', bool skip_empty = true);
std::string Join(const std::vector<std::string>& strings, char delim, bool skip_empty = true);
std::string Join(const std::vector<std::string>& strings, bool skip_empty = true);

std::string ToUpperCase(std::string string);
std::string ToLowerCase(std::string string);

std::string ConvertUnicodeToMultiByte(const std::wstring& unicode_string);
std::wstring ConvertMultiByteToUnicode(const std::string& filePath);

bool StartsWith(const std::string& string, const std::string& search);
bool EndsWith(const std::string& string, const std::string& search);

}