#pragma once

#include "pch.h"

#include "Engine/Core/StringUtils.hpp"

#include <string>
#include <vector>

TEST(StringUtilsFunctions, SplitDefault) {
    auto input = std::string{"a,b,c"};
    auto result = StringUtils::Split(input);
    auto expected = std::vector<std::string>{ "a","b","c" };
    EXPECT_EQ(expected, result);
    
    input = std::string{"abc"};
    result = StringUtils::Split(input);
    expected = std::vector<std::string>{ "abc" };
    EXPECT_EQ(expected, result);
    
    input = std::string{",abc"};
    result = StringUtils::Split(input);
    expected = std::vector<std::string>{ "abc" };
    EXPECT_EQ(expected, result);
    
    input = std::string{"a,bc"};
    result = StringUtils::Split(input);
    expected = std::vector<std::string>{ "a", "bc" };
    EXPECT_EQ(expected, result);
    
    input = std::string{"ab,c"};
    result = StringUtils::Split(input);
    expected = std::vector<std::string>{ "ab", "c" };
    EXPECT_EQ(expected, result);
    
    input = std::string{"abc,"};
    result = StringUtils::Split(input);
    expected = std::vector<std::string>{ "abc" };
    EXPECT_EQ(expected, result);

}

TEST(StringUtilsFunctions, SplitExplicitDelimiter) {
    auto input = std::string{"a.b.c"};
    auto result = StringUtils::Split(input, '.');
    auto expected = std::vector<std::string>{ "a","b","c" };
    EXPECT_EQ(expected, result);
    
    input = std::string{"abc"};
    result = StringUtils::Split(input, '.');
    expected = std::vector<std::string>{ "abc" };
    EXPECT_EQ(expected, result);
    
    input = std::string{".abc"};
    result = StringUtils::Split(input, '.');
    expected = std::vector<std::string>{ "abc" };
    EXPECT_EQ(expected, result);
    
    input = std::string{"a.bc"};
    result = StringUtils::Split(input, '.');
    expected = std::vector<std::string>{ "a", "bc" };
    EXPECT_EQ(expected, result);
    
    input = std::string{"ab.c"};
    result = StringUtils::Split(input, '.');
    expected = std::vector<std::string>{ "ab", "c" };
    EXPECT_EQ(expected, result);
    
    input = std::string{"abc."};
    result = StringUtils::Split(input, '.');
    expected = std::vector<std::string>{ "abc" };
    EXPECT_EQ(expected, result);

}

TEST(StringUtilsFunctions, SplitNoSkipEmpty) {
    auto input = std::string{"a.b.c"};
    auto result = StringUtils::Split(input, '.', false);
    auto expected = std::vector<std::string>{ "a","b","c" };
    EXPECT_EQ(expected, result);
    
    input = std::string{"abc"};
    result = StringUtils::Split(input, '.', false);
    expected = std::vector<std::string>{ "abc" };
    EXPECT_EQ(expected, result);
    
    input = std::string{".abc"};
    result = StringUtils::Split(input, '.', false);
    expected = std::vector<std::string>{ "", "abc" };
    EXPECT_EQ(expected, result);
    
    input = std::string{"a..bc"};
    result = StringUtils::Split(input, '.', false);
    expected = std::vector<std::string>{ "a", "", "bc" };
    EXPECT_EQ(expected, result);
    
    input = std::string{"a.bc"};
    result = StringUtils::Split(input, '.', false);
    expected = std::vector<std::string>{ "a", "bc" };
    EXPECT_EQ(expected, result);
    
    input = std::string{"ab.c"};
    result = StringUtils::Split(input, '.', false);
    expected = std::vector<std::string>{ "ab", "c" };
    EXPECT_EQ(expected, result);
    
    input = std::string{"abc."};
    result = StringUtils::Split(input, '.', false);
    expected = std::vector<std::string>{ "abc", "" };
    EXPECT_EQ(expected, result);

}


TEST(StringUtilsFunctions, WSplitDefault) {
    auto input = std::wstring{ L"a,b,c" };
    auto result = StringUtils::Split(input);
    auto expected = std::vector<std::wstring>{ L"a", L"b", L"c" };
    EXPECT_EQ(expected, result);

    input = std::wstring{ L"abc" };
    result = StringUtils::Split(input);
    expected = std::vector<std::wstring>{ L"abc" };
    EXPECT_EQ(expected, result);

    input = std::wstring{ L",abc" };
    result = StringUtils::Split(input);
    expected = std::vector<std::wstring>{ L"abc" };
    EXPECT_EQ(expected, result);

    input = std::wstring{ L"a,bc" };
    result = StringUtils::Split(input);
    expected = std::vector<std::wstring>{ L"a", L"bc" };
    EXPECT_EQ(expected, result);

    input = std::wstring{ L"ab,c" };
    result = StringUtils::Split(input);
    expected = std::vector<std::wstring>{ L"ab", L"c" };
    EXPECT_EQ(expected, result);

    input = std::wstring{ L"abc," };
    result = StringUtils::Split(input);
    expected = std::vector<std::wstring>{ L"abc" };
    EXPECT_EQ(expected, result);

}

TEST(StringUtilsFunctions, WSplitExplicitDelimiter) {
    auto input = std::wstring{ L"a.b.c" };
    auto result = StringUtils::Split(input, L'.');
    auto expected = std::vector<std::wstring>{ L"a", L"b", L"c" };
    EXPECT_EQ(expected, result);

    input = std::wstring{ L"abc" };
    result = StringUtils::Split(input, L'.');
    expected = std::vector<std::wstring>{ L"abc" };
    EXPECT_EQ(expected, result);

    input = std::wstring{ L".abc" };
    result = StringUtils::Split(input, L'.');
    expected = std::vector<std::wstring>{ L"abc" };
    EXPECT_EQ(expected, result);

    input = std::wstring{ L"a.bc" };
    result = StringUtils::Split(input, L'.');
    expected = std::vector<std::wstring>{ L"a", L"bc" };
    EXPECT_EQ(expected, result);

    input = std::wstring{ L"ab.c" };
    result = StringUtils::Split(input, L'.');
    expected = std::vector<std::wstring>{ L"ab", L"c" };
    EXPECT_EQ(expected, result);

    input = std::wstring{ L"abc." };
    result = StringUtils::Split(input, L'.');
    expected = std::vector<std::wstring>{ L"abc" };
    EXPECT_EQ(expected, result);

}

TEST(StringUtilsFunctions, WSplitNoSkipEmpty) {
    auto input = std::wstring{ L"a.b.c" };
    auto result = StringUtils::Split(input, '.', false);
    auto expected = std::vector<std::wstring>{ L"a", L"b", L"c" };
    EXPECT_EQ(expected, result);

    input = std::wstring{ L"abc" };
    result = StringUtils::Split(input, L'.', false);
    expected = std::vector<std::wstring>{ L"abc" };
    EXPECT_EQ(expected, result);

    input = std::wstring{ L".abc" };
    result = StringUtils::Split(input, L'.', false);
    expected = std::vector<std::wstring>{ L"", L"abc" };
    EXPECT_EQ(expected, result);

    input = std::wstring{ L"a..bc" };
    result = StringUtils::Split(input, L'.', false);
    expected = std::vector<std::wstring>{ L"a", L"", L"bc" };
    EXPECT_EQ(expected, result);

    input = std::wstring{ L"a.bc" };
    result = StringUtils::Split(input, L'.', false);
    expected = std::vector<std::wstring>{ L"a", L"bc" };
    EXPECT_EQ(expected, result);

    input = std::wstring{ L"ab.c" };
    result = StringUtils::Split(input, L'.', false);
    expected = std::vector<std::wstring>{ L"ab", L"c" };
    EXPECT_EQ(expected, result);

    input = std::wstring{ L"abc." };
    result = StringUtils::Split(input, L'.', false);
    expected = std::vector<std::wstring>{ L"abc", L"" };
    EXPECT_EQ(expected, result);

}

TEST(StringUtilsFunctions, SplitOnFirst) {
    auto input = std::string{ "command and then arguments" };
    auto result = StringUtils::SplitOnFirst(input, ' ');
    auto expected = std::make_pair(std::string{"command"}, std::string{"and then arguments"});
    EXPECT_EQ(expected, result);
}

TEST(StringUtilsFunctions, WSplitOnFirst) {
    auto input = std::wstring{ L"command and then arguments" };
    auto result = StringUtils::SplitOnFirst(input, L' ');
    auto expected = std::make_pair(std::wstring{L"command"}, std::wstring{L"and then arguments"});
    EXPECT_EQ(expected, result);
}

TEST(StringUtilsFunctions, SplitOnLast) {
    auto input = std::string{ "command and then arguments" };
    auto result = StringUtils::SplitOnLast(input, ' ');
    auto expected = std::make_pair(std::string{"command and then"}, std::string{"arguments"});
    EXPECT_EQ(expected, result);
}

TEST(StringUtilsFunctions, WSplitOnLast) {
    auto input = std::wstring{ L"command and then arguments" };
    auto result = StringUtils::SplitOnLast(input, L' ');
    auto expected = std::make_pair(std::wstring{L"command and then"}, std::wstring{L"arguments"});
    EXPECT_EQ(expected, result);
}

//std::pair<std::string, std::string> SplitOnFirst(const std::string& string, char delim) noexcept;
//std::pair<std::wstring, std::wstring> SplitOnFirst(const std::wstring& string, wchar_t delim) noexcept;
//std::pair<std::string, std::string> SplitOnLast(const std::string& string, char delim) noexcept;
//std::pair<std::wstring, std::wstring> SplitOnLast(const std::wstring& string, wchar_t delim) noexcept;
//std::vector<std::string> SplitOnUnquoted(const std::string& string, char delim = ',', bool skip_empty = true) noexcept;
//std::vector<std::wstring> SplitOnUnquoted(const std::wstring& string, wchar_t delim = ',', bool skip_empty = true) noexcept;
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

