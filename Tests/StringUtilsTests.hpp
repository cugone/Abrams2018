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

TEST(StringUtilsFunctions, SplitOnUnquotedDefault) {
    auto input = std::string{ "command,and,\"then,arguments\",Hello,World" };
    auto result = StringUtils::SplitOnUnquoted(input);
    auto expected = std::vector<std::string>{"command", "and", "\"then,arguments\"","Hello","World" };
    EXPECT_EQ(expected, result);
}

TEST(StringUtilsFunctions, WSplitOnUnquotedDefault) {
    auto input = std::wstring{ L"command,and,\"then,arguments\",Hello,World" };
    auto result = StringUtils::SplitOnUnquoted(input);
    auto expected = std::vector<std::wstring>{ L"command", L"and", L"\"then,arguments\"",L"Hello",L"World" };
    EXPECT_EQ(expected, result);
}

TEST(StringUtilsFunctions, SplitOnUnquotedExplicitDelimiter) {
    auto input = std::string{ "key1=value1\nkey2=\"value2\nvalue3\"\nkey3=value4\nkey5=value5" };
    auto result = StringUtils::SplitOnUnquoted(input, '\n');
    auto expected = std::vector<std::string>{ "key1=value1", "key2=\"value2\nvalue3\"", "key3=value4", "key5=value5" };
    EXPECT_EQ(expected, result);
}

TEST(StringUtilsFunctions, WSplitOnUnquotedExplicitDelimiter) {
    auto input = std::wstring{ L"key1=value1\nkey2=\"value2\nvalue3\"\nkey3=value4\nkey5=value5" };
    auto result = StringUtils::SplitOnUnquoted(input, L'\n');
    auto expected = std::vector<std::wstring>{ L"key1=value1", L"key2=\"value2\nvalue3\"", L"key3=value4", L"key5=value5" };
    EXPECT_EQ(expected, result);
}

TEST(StringUtilsFunctions, SplitOnUnquotedNoSkipEmpty) {
    auto input = std::string{ "key1=value1\n\n\n\n\nkey2=\"value2\nvalue3\"\nkey3=value4\nkey5=value5" };
    auto result = StringUtils::SplitOnUnquoted(input, '\n', false);
    auto expected = std::vector<std::string>{ "key1=value1", "", "", "", "", "key2=\"value2\nvalue3\"", "key3=value4", "key5=value5" };
    EXPECT_EQ(expected, result);
}

TEST(StringUtilsFunctions, WSplitOnUnquotedNoSkipEmpty) {
    auto input = std::wstring{ L"key1=value1\n\n\n\n\nkey2=\"value2\nvalue3\"\nkey3=value4\nkey5=value5" };
    auto result = StringUtils::SplitOnUnquoted(input, L'\n', false);
    auto expected = std::vector<std::wstring>{ L"key1=value1", L"", L"", L"", L"", L"key2=\"value2\nvalue3\"", L"key3=value4", L"key5=value5" };
    EXPECT_EQ(expected, result);
}

TEST(StringUtilsFunctions, JoinDefault) {
    auto input = std::vector<std::string>{};
    auto result = StringUtils::Join(input);
    auto expected = std::string{};
    EXPECT_EQ(expected, result);

    input = std::vector<std::string>{ "a" };
    result = StringUtils::Join(input);
    expected = std::string{ "a" };
    EXPECT_EQ(expected, result);

    input = std::vector<std::string>{ "a","b" };
    result = StringUtils::Join(input);
    expected = std::string{ "ab" };
    EXPECT_EQ(expected, result);

    input = std::vector<std::string>{ "a","b","c" };
    result = StringUtils::Join(input);
    expected = std::string{ "abc" };
    EXPECT_EQ(expected, result);

}

TEST(StringUtilsFunctions, JoinDefaultNoSkipEmpty) {
    auto input = std::vector<std::string>{ "a","b","","c" };
    auto result = StringUtils::Join(input, false);
    auto expected = std::string{ "abc" };
    EXPECT_EQ(expected, result);

}

TEST(StringUtilsFunctions, WJoinDefault) {
    auto input = std::vector<std::wstring>{ L"a",L"b",L"c" };
    auto result = StringUtils::Join(input);
    auto expected = std::wstring{ L"abc" };
    EXPECT_EQ(expected, result);

}

TEST(StringUtilsFunctions, WJoinDefaultNoSkipEmpty) {
    auto input = std::vector<std::wstring>{ L"a",L"b",L"",L"c" };
    auto result = StringUtils::Join(input, false);
    auto expected = std::wstring{ L"abc" };
    EXPECT_EQ(expected, result);

}

TEST(StringUtilsFunctions, JoinExplicitDelimiter) {
    auto input = std::vector<std::string>{ "a","b","c" };
    auto result = StringUtils::Join(input, '.');
    auto expected = std::string{ "a.b.c" };
    EXPECT_EQ(expected, result);
}

TEST(StringUtilsFunctions, WJoinExplicitDelimiter) {
    auto input = std::vector<std::wstring>{ L"a",L"b",L"c" };
    auto result = StringUtils::Join(input, L'.');
    auto expected = std::wstring{ L"a.b.c" };
    EXPECT_EQ(expected, result);
}

TEST(StringUtilsFunctions, JoinExplicitDelimiterNoSkipEmpty) {
    auto input = std::vector<std::string>{ "a","b","c" };
    auto result = StringUtils::Join(input, '.', false);
    auto expected = std::string{ "a.b.c" };
    EXPECT_EQ(expected, result);
}

TEST(StringUtilsFunctions, WJoinExplicitDelimiterNoSkipEmpty) {
    auto input = std::vector<std::wstring>{ L"a",L"b",L"c" };
    auto result = StringUtils::Join(input, L'.', false);
    auto expected = std::wstring{ L"a.b.c" };
    EXPECT_EQ(expected, result);
}

TEST(StringUtilsFunctions, JoinVariadicDefault) {
    auto result = StringUtils::Join(std::string{"a"}, std::string{"b"}, std::string{"c"});
    auto expected = std::string{ "abc" };
    EXPECT_EQ(expected, result);

    result = StringUtils::Join(std::string{ "a" }, std::string{}, std::string{ "c" });
    expected = std::string{ "ac" };
    EXPECT_EQ(expected, result);

}

TEST(StringUtilsFunctions, WJoinVariadicDefault) {
    auto result = StringUtils::Join(std::wstring{L"a"}, std::wstring{L"b"}, std::wstring{L"c"});
    auto expected = std::wstring{ L"abc" };
    EXPECT_EQ(expected, result);

    result = StringUtils::Join(std::wstring{L"a"}, std::wstring{}, std::wstring{L"c"});
    expected = std::wstring{ L"ac" };
    EXPECT_EQ(expected, result);

}

TEST(StringUtilsFunctions, JoinVariadicExplicitDelimiter) {

    auto result = StringUtils::Join(',', std::string{ "a" });
    auto expected = std::string{ "a" };
    EXPECT_EQ(expected, result);

    result = StringUtils::Join(',', std::string{ "a" }, std::string{ "b" });
    expected = std::string{ "a,b" };
    EXPECT_EQ(expected, result);

    result = StringUtils::Join(',', std::string{ "a" }, std::string{ "b" }, std::string{ "c" });
    expected = std::string{ "a,b,c" };
    EXPECT_EQ(expected, result);

    result = StringUtils::Join(',', std::string{"a"}, std::string{}, std::string{"c"});
    expected = std::string{ "a,,c" };
    EXPECT_EQ(expected, result);

    result = StringUtils::Join(',', std::string{ "a" }, std::string{ "b" }, std::string{ "c" }, std::string{});
    expected = std::string{ "a,b,c," };
    EXPECT_EQ(expected, result);

    result = StringUtils::Join(',', std::string{}, std::string{ "a" }, std::string{ "b" }, std::string{ "c" });
    expected = std::string{ ",a,b,c" };
    EXPECT_EQ(expected, result);

}

TEST(StringUtilsFunctions, WJoinVariadicExplicitDelimiter) {
    auto result = StringUtils::Join(L',', std::wstring{L"a"}, std::wstring{L"b"}, std::wstring{L"c"});
    auto expected = std::wstring{ L"a,b,c" };
    EXPECT_EQ(expected, result);

    result = StringUtils::Join(L',', std::wstring{ L"a" }, std::wstring{}, std::wstring{ L"c" });
    expected = std::wstring{ L"a,,c" };
    EXPECT_EQ(expected, result);

    result = StringUtils::Join(L',', std::wstring{ L"a" }, std::wstring{ L"b" }, std::wstring{ L"c" }, std::wstring{});
    expected = std::wstring{ L"a,b,c," };
    EXPECT_EQ(expected, result);

}

TEST(StringUtilsFunctions, JoinVariadicSkipEmpty) {
    auto result = StringUtils::JoinSkipEmpty(std::string{ "a" }, std::string{ "b" }, std::string{ "c" });
    auto expected = std::string{ "abc" };
    EXPECT_EQ(expected, result);

    result = StringUtils::JoinSkipEmpty(std::string{ "a" }, std::string{}, std::string{ "c" });
    expected = std::string{ "ac" };
    EXPECT_EQ(expected, result);

}

TEST(StringUtilsFunctions, WJoinVariadicSkipEmpty) {
    auto result = StringUtils::JoinSkipEmpty(std::wstring{ L"a" }, std::wstring{ L"b" }, std::wstring{ L"c" });
    auto expected = std::wstring{ L"abc" };
    EXPECT_EQ(expected, result);

    result = StringUtils::JoinSkipEmpty(std::wstring{ L"a" }, std::wstring{}, std::wstring{ L"c" });
    expected = std::wstring{ L"ac" };
    EXPECT_EQ(expected, result);

}

TEST(StringUtilsFunctions, JoinVariadicSkipEmptyExplicitDelimiter) {

    auto result = StringUtils::JoinSkipEmpty(',', std::string{ "a" });
    auto expected = std::string{ "a" };
    EXPECT_EQ(expected, result);

    result = StringUtils::JoinSkipEmpty(',', std::string{ "a" }, std::string{ "c" });
    expected = std::string{ "a,c" };
    EXPECT_EQ(expected, result);

    result = StringUtils::JoinSkipEmpty(',', std::string{ "a" }, std::string{"b"}, std::string{ "c" });
    expected = std::string{ "a,b,c" };
    EXPECT_EQ(expected, result);

    result = StringUtils::JoinSkipEmpty(',', std::string{ "a" }, std::string{}, std::string{ "c" });
    expected = std::string{ "a,c" };
    EXPECT_EQ(expected, result);

    result = StringUtils::JoinSkipEmpty(',', std::string{ "a" }, std::string{ "b" }, std::string{ "c" }, std::string{});
    expected = std::string{ "a,b,c" };
    EXPECT_EQ(expected, result);

    result = StringUtils::JoinSkipEmpty(',', std::string{}, std::string{"a"}, std::string{ "b" }, std::string{ "c" });
    expected = std::string{ "a,b,c" };
    EXPECT_EQ(expected, result);

}

TEST(StringUtilsFunctions, WJoinVariadicSkipEmptyExplicitDelimiter) {
    auto result = StringUtils::JoinSkipEmpty(L',', std::wstring{ L"a" }, std::wstring{ L"b" }, std::wstring{ L"c" });
    auto expected = std::wstring{ L"a,b,c" };
    EXPECT_EQ(expected, result);

    result = StringUtils::JoinSkipEmpty(L',', std::wstring{ L"a" }, std::wstring{}, std::wstring{ L"c" });
    expected = std::wstring{ L"a,c" };
    EXPECT_EQ(expected, result);

    result = StringUtils::JoinSkipEmpty(L',', std::wstring{ L"a" }, std::wstring{ L"b" }, std::wstring{ L"c" }, std::wstring{});
    expected = std::wstring{ L"a,b,c" };
    EXPECT_EQ(expected, result);

    result = StringUtils::JoinSkipEmpty(L',', std::wstring{}, std::wstring{ L"a" }, std::wstring{ L"b" }, std::wstring{ L"c" });
    expected = std::wstring{ L"a,b,c" };
    EXPECT_EQ(expected, result);

}



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

