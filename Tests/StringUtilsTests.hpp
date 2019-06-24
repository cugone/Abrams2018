#pragma once

#include "pch.h"

#include "Engine/Core/StringUtils.hpp"

#include <vector>

TEST(StringUtilsFunctions, Split) {
    EXPECT_EQ(std::vector{"a","b","c"}, Split(std::string{"a,b,c"}));
}

//std::vector<std::string> Split(const std::string& string, char delim = ',', bool skip_empty = true) noexcept;
//std::vector<std::wstring> Split(const std::wstring& string, wchar_t delim = ',', bool skip_empty = true) noexcept;
//std::vector<std::string> SplitOnUnquoted(const std::string& string, char delim = ',', bool skip_empty = true) noexcept;
//std::vector<std::wstring> SplitOnUnquoted(const std::wstring& string, wchar_t delim = ',', bool skip_empty = true) noexcept;
//std::pair<std::string, std::string> SplitOnFirst(const std::string& string, char delim) noexcept;
//std::pair<std::wstring, std::wstring> SplitOnFirst(const std::wstring& string, wchar_t delim) noexcept;
//std::pair<std::string, std::string> SplitOnLast(const std::string& string, char delim) noexcept;
//std::pair<std::wstring, std::wstring> SplitOnLast(const std::wstring& string, wchar_t delim) noexcept;
//
//std::string Join(const std::vector<std::string>& strings, char delim, bool skip_empty = true) noexcept;
//std::wstring Join(const std::vector<std::wstring>& strings, wchar_t delim, bool skip_empty = true) noexcept;
//std::string Join(const std::vector<std::string>& strings, bool skip_empty = true) noexcept;
//std::wstring Join(const std::vector<std::wstring>& strings, bool skip_empty = true) noexcept;
//
//template<typename T, typename... U>
//T Join(char delim, const T& arg, const U& ... args) noexcept {
//    return detail::Join(delim, arg, args ...);
//}
//
//template<typename T, typename... U>
//T JoinSkipEmpty(char delim, const T& arg, const U& ... args) noexcept {
//    return detail::JoinSkipEmpty(delim, arg, args ...);
//}
//
//template<typename T, typename... U>
//T Join(const T& arg, const U& ... args) noexcept {
//    return detail::Join(arg, args ...);
//}
//
//template<typename T, typename... U>
//T JoinSkipEmpty(const T& arg, const U& ... args) noexcept {
//    return detail::JoinSkipEmpty(arg, args ...);
//}
//
//std::string ToUpperCase(std::string string) noexcept;
//std::wstring ToUpperCase(std::wstring string) noexcept;
//std::string ToLowerCase(std::string string) noexcept;
//std::wstring ToLowerCase(std::wstring string) noexcept;
//
//std::string ConvertUnicodeToMultiByte(const std::wstring& unicode_string) noexcept;
//std::wstring ConvertMultiByteToUnicode(const std::string& multi_byte_string) noexcept;
//
//bool StartsWith(const std::string& string, const std::string& start) noexcept;
//bool StartsWith(const std::wstring& string, const std::wstring& start) noexcept;
//bool StartsWith(const std::string& string, char start) noexcept;
//
//bool EndsWith(const std::string& string, const std::string& end) noexcept;
//bool EndsWith(const std::wstring& string, const std::wstring& end) noexcept;
//bool EndsWith(const std::string& string, char end) noexcept;
//
//std::string ReplaceAll(std::string string, const std::string& from, const std::string& to) noexcept;
//std::wstring ReplaceAll(std::wstring string, const std::wstring& from, const std::wstring& to) noexcept;
//
//std::vector<std::size_t> FindAll(std::string string, const char c) noexcept;
//std::vector<std::size_t> FindAll(std::wstring string, const wchar_t c) noexcept;
//std::vector<std::size_t> FindAll(std::string string, const std::string& sequence) noexcept;
//std::vector<std::size_t> FindAll(std::wstring string, const std::wstring& sequence) noexcept;
//
//std::string TrimWhitespace(std::string string) noexcept;
//std::wstring TrimWhitespace(std::wstring string) noexcept;
//
//constexpr const uint32_t FourCC(const char* id) noexcept {
//    return static_cast<uint32_t>((((id[0] << 24) & 0xFF000000) | ((id[1] << 16) & 0x00FF0000) | ((id[2] << 8) & 0x0000FF00) | ((id[3] << 0) & 0x000000FF)));
//}
//
//void CopyFourCC(char* destFCC, const char* srcFCC) noexcept;
//std::string FourCCToString(const char* id) noexcept;
//
//namespace Encryption {
//
//    //NOT USEFUL AS TRUE ENCRYPTION!! DO NOT USE IF SERIOUS ENCRYPTION IS NEEDED!!!
//    std::string ROT13(std::string text) noexcept;
//
//    //NOT USEFUL AS TRUE ENCRYPTION!! DO NOT USE IF SERIOUS ENCRYPTION IS NEEDED!!!
//    std::string CaesarShiftEncode(int key, std::string plaintext) noexcept;
//
//    //NOT USEFUL AS TRUE ENCRYPTION!! DO NOT USE IF SERIOUS ENCRYPTION IS NEEDED!!!
//    std::string CaesarShiftDecode(int key, std::string ciphertext) noexcept;
//
//} //End Encryption
//
//} //End StringUtils
