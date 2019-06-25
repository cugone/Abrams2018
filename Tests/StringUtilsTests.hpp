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

TEST(StringUtilsFunctions, ToUpperCase) {
    auto result = StringUtils::ToUpperCase(std::string{});
    auto expected = std::string{};
    EXPECT_EQ(expected, result);

    result = StringUtils::ToUpperCase(std::string{ "a" });
    expected = std::string{ "A" };
    EXPECT_EQ(expected, result);

    result = StringUtils::ToUpperCase(std::string{ "A" });
    expected = std::string{ "A" };
    EXPECT_EQ(expected, result);

    result = StringUtils::ToUpperCase(std::string{ "ab" });
    expected = std::string{ "AB" };
    EXPECT_EQ(expected, result);

    result = StringUtils::ToUpperCase(std::string{ "Ab" });
    expected = std::string{ "AB" };
    EXPECT_EQ(expected, result);

    result = StringUtils::ToUpperCase(std::string{ "aB" });
    expected = std::string{ "AB" };
    EXPECT_EQ(expected, result);

    result = StringUtils::ToUpperCase(std::string{ "AB" });
    expected = std::string{ "AB" };
    EXPECT_EQ(expected, result);

    result = StringUtils::ToUpperCase(std::string{ "abc" });
    expected = std::string{ "ABC" };
    EXPECT_EQ(expected, result);

    result = StringUtils::ToUpperCase(std::string{ "abC" });
    EXPECT_EQ(expected, result);

    result = StringUtils::ToUpperCase(std::string{ "aBc" });
    EXPECT_EQ(expected, result);

    result = StringUtils::ToUpperCase(std::string{ "aBC" });
    EXPECT_EQ(expected, result);

    result = StringUtils::ToUpperCase(std::string{ "Abc" });
    EXPECT_EQ(expected, result);

    result = StringUtils::ToUpperCase(std::string{ "AbC" });
    EXPECT_EQ(expected, result);

    result = StringUtils::ToUpperCase(std::string{ "ABc" });
    EXPECT_EQ(expected, result);

    result = StringUtils::ToUpperCase(std::string{ "ABC" });
    EXPECT_EQ(expected, result);

}


TEST(StringUtilsFunctions, WToUpperCase) {
    auto result = StringUtils::ToUpperCase(std::wstring{});
    auto expected = std::wstring{};
    EXPECT_EQ(expected, result);

    result = StringUtils::ToUpperCase(std::wstring{ L"a" });
    expected = std::wstring{ L"A" };
    EXPECT_EQ(expected, result);

    result = StringUtils::ToUpperCase(std::wstring{ L"A" });
    expected = std::wstring{ L"A" };
    EXPECT_EQ(expected, result);

    result = StringUtils::ToUpperCase(std::wstring{ L"ab" });
    expected = std::wstring{ L"AB" };
    EXPECT_EQ(expected, result);

    result = StringUtils::ToUpperCase(std::wstring{ L"Ab" });
    expected = std::wstring{ L"AB" };
    EXPECT_EQ(expected, result);

    result = StringUtils::ToUpperCase(std::wstring{ L"aB" });
    expected = std::wstring{ L"AB" };
    EXPECT_EQ(expected, result);

    result = StringUtils::ToUpperCase(std::wstring{ L"AB" });
    expected = std::wstring{ L"AB" };
    EXPECT_EQ(expected, result);

    result = StringUtils::ToUpperCase(std::wstring{ L"abc" });
    expected = std::wstring{ L"ABC" };
    EXPECT_EQ(expected, result);

    result = StringUtils::ToUpperCase(std::wstring{ L"abC" });
    EXPECT_EQ(expected, result);

    result = StringUtils::ToUpperCase(std::wstring{ L"aBc" });
    EXPECT_EQ(expected, result);

    result = StringUtils::ToUpperCase(std::wstring{ L"aBC" });
    EXPECT_EQ(expected, result);

    result = StringUtils::ToUpperCase(std::wstring{ L"Abc" });
    EXPECT_EQ(expected, result);

    result = StringUtils::ToUpperCase(std::wstring{ L"AbC" });
    EXPECT_EQ(expected, result);

    result = StringUtils::ToUpperCase(std::wstring{ L"ABc" });
    EXPECT_EQ(expected, result);

    result = StringUtils::ToUpperCase(std::wstring{ L"ABC" });
    EXPECT_EQ(expected, result);

}


TEST(StringUtilsFunctions, ToLowerCase) {
    auto result = StringUtils::ToLowerCase(std::string{});
    auto expected = std::string{};
    EXPECT_EQ(expected, result);

    result = StringUtils::ToLowerCase(std::string{ "a" });
    expected = std::string{ "a" };
    EXPECT_EQ(expected, result);

    result = StringUtils::ToLowerCase(std::string{ "A" });
    expected = std::string{ "a" };
    EXPECT_EQ(expected, result);

    result = StringUtils::ToLowerCase(std::string{ "ab" });
    expected = std::string{ "ab" };
    EXPECT_EQ(expected, result);

    result = StringUtils::ToLowerCase(std::string{ "Ab" });
    expected = std::string{ "ab" };
    EXPECT_EQ(expected, result);

    result = StringUtils::ToLowerCase(std::string{ "aB" });
    expected = std::string{ "ab" };
    EXPECT_EQ(expected, result);

    result = StringUtils::ToLowerCase(std::string{ "AB" });
    expected = std::string{ "ab" };
    EXPECT_EQ(expected, result);

    result = StringUtils::ToLowerCase(std::string{ "abc" });
    expected = std::string{ "abc" };
    EXPECT_EQ(expected, result);

    result = StringUtils::ToLowerCase(std::string{ "abC" });
    EXPECT_EQ(expected, result);

    result = StringUtils::ToLowerCase(std::string{ "aBc" });
    EXPECT_EQ(expected, result);

    result = StringUtils::ToLowerCase(std::string{ "aBC" });
    EXPECT_EQ(expected, result);

    result = StringUtils::ToLowerCase(std::string{ "Abc" });
    EXPECT_EQ(expected, result);

    result = StringUtils::ToLowerCase(std::string{ "AbC" });
    EXPECT_EQ(expected, result);

    result = StringUtils::ToLowerCase(std::string{ "ABc" });
    EXPECT_EQ(expected, result);

    result = StringUtils::ToLowerCase(std::string{ "ABC" });
    EXPECT_EQ(expected, result);

}


TEST(StringUtilsFunctions, WToLowerCase) {
    auto result = StringUtils::ToLowerCase(std::wstring{});
    auto expected = std::wstring{};
    EXPECT_EQ(expected, result);

    result = StringUtils::ToLowerCase(std::wstring{ L"a" });
    expected = std::wstring{ L"a" };
    EXPECT_EQ(expected, result);

    result = StringUtils::ToLowerCase(std::wstring{ L"A" });
    expected = std::wstring{ L"a" };
    EXPECT_EQ(expected, result);

    result = StringUtils::ToLowerCase(std::wstring{ L"ab" });
    expected = std::wstring{ L"ab" };
    EXPECT_EQ(expected, result);

    result = StringUtils::ToLowerCase(std::wstring{ L"Ab" });
    expected = std::wstring{ L"ab" };
    EXPECT_EQ(expected, result);

    result = StringUtils::ToLowerCase(std::wstring{ L"aB" });
    expected = std::wstring{ L"ab" };
    EXPECT_EQ(expected, result);

    result = StringUtils::ToLowerCase(std::wstring{ L"AB" });
    expected = std::wstring{ L"ab" };
    EXPECT_EQ(expected, result);

    result = StringUtils::ToLowerCase(std::wstring{ L"abc" });
    expected = std::wstring{ L"abc" };
    EXPECT_EQ(expected, result);

    result = StringUtils::ToLowerCase(std::wstring{ L"abC" });
    EXPECT_EQ(expected, result);

    result = StringUtils::ToLowerCase(std::wstring{ L"aBc" });
    EXPECT_EQ(expected, result);

    result = StringUtils::ToLowerCase(std::wstring{ L"aBC" });
    EXPECT_EQ(expected, result);

    result = StringUtils::ToLowerCase(std::wstring{ L"Abc" });
    EXPECT_EQ(expected, result);

    result = StringUtils::ToLowerCase(std::wstring{ L"AbC" });
    EXPECT_EQ(expected, result);

    result = StringUtils::ToLowerCase(std::wstring{ L"ABc" });
    EXPECT_EQ(expected, result);

    result = StringUtils::ToLowerCase(std::wstring{ L"ABC" });
    EXPECT_EQ(expected, result);

}

TEST(StringUtilsFunctions, StartsWith) {
    EXPECT_TRUE(StringUtils::StartsWith(std::string{ "Hello" }, std::string{ "He" }));
    EXPECT_FALSE(StringUtils::StartsWith(std::string{ "Hello" }, std::string{ "he" }));
    EXPECT_FALSE(StringUtils::StartsWith(std::string{ "Hello" }, std::string{ "ll" }));
    EXPECT_FALSE(StringUtils::StartsWith(std::string{ "Hello" }, std::string{ "lO" }));
    EXPECT_FALSE(StringUtils::StartsWith(std::string{ "Hello" }, std::string{ "lo" }));

    EXPECT_TRUE(StringUtils::StartsWith(std::string{ "Hello" }, 'H'));
    EXPECT_FALSE(StringUtils::StartsWith(std::string{ "Hello" }, 'h'));
    EXPECT_FALSE(StringUtils::StartsWith(std::string{ "Hello" }, 'e'));
    EXPECT_FALSE(StringUtils::StartsWith(std::string{ "Hello" }, 'l'));
    EXPECT_FALSE(StringUtils::StartsWith(std::string{ "Hello" }, 'l'));
    EXPECT_FALSE(StringUtils::StartsWith(std::string{ "Hello" }, 'o'));

}

TEST(StringUtilsFunctions, WStartsWith) {
    EXPECT_TRUE(StringUtils::StartsWith( std::wstring{ L"Hello" }, std::wstring{ L"He" }));
    EXPECT_FALSE(StringUtils::StartsWith(std::wstring{ L"Hello" }, std::wstring{ L"he" }));
    EXPECT_FALSE(StringUtils::StartsWith(std::wstring{ L"Hello" }, std::wstring{ L"ll" }));
    EXPECT_FALSE(StringUtils::StartsWith(std::wstring{ L"Hello" }, std::wstring{ L"lO" }));
    EXPECT_FALSE(StringUtils::StartsWith( std::wstring{ L"Hello" }, std::wstring{ L"lo" }));

    EXPECT_TRUE( StringUtils::StartsWith(std::wstring{ L"Hello" }, L'H'));
    EXPECT_FALSE(StringUtils::StartsWith(std::wstring{ L"Hello" }, L'h'));
    EXPECT_FALSE(StringUtils::StartsWith(std::wstring{ L"Hello" }, L'e'));
    EXPECT_FALSE(StringUtils::StartsWith(std::wstring{ L"Hello" }, L'l'));
    EXPECT_FALSE(StringUtils::StartsWith(std::wstring{ L"Hello" }, L'l'));
    EXPECT_FALSE(StringUtils::StartsWith(std::wstring{ L"Hello" }, L'o'));

}


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

