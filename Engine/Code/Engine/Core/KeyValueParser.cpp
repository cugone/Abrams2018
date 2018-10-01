#include "Engine/Core/KeyValueParser.hpp"

#include "Engine/Core/StringUtils.hpp"

#include <algorithm>
#include <locale>
#include <sstream>

KeyValueParser::KeyValueParser(const std::string& str) {
    Parse(str);
}

KeyValueParser::KeyValueParser(std::ifstream& file_input) {
    Parse(file_input);
}

KeyValueParser::KeyValueParser(std::istream& input) {
    Parse(input);
}

bool KeyValueParser::HasKey(const std::string& key) const {
    return _kv_pairs.find(key) != _kv_pairs.end();
}

bool KeyValueParser::Parse(const std::string& input) {
    //Strip comments.
    std::string cur_line = input.substr(0, input.find_first_of('#'));
    if(cur_line.empty()) {
        return true;
    }
    std::size_t eq_count = std::count(cur_line.begin(), cur_line.end(), '=');
    std::size_t true_count = std::count(cur_line.begin(), cur_line.end(), '+');
    std::size_t false_count = std::count(cur_line.begin(), cur_line.end(), '-');
    std::size_t nl_count = std::count(cur_line.begin(), cur_line.end(), '\n');
    bool probably_multiline = nl_count == 0 && (eq_count > 1 || true_count > 1 || false_count > 1);
    if(probably_multiline) {
        return ParseMultiParams(cur_line);
    }


    //Get raw key-value pairs split on first equals.
    auto key_value = StringUtils::Split(cur_line, '=');
    auto cur_line_eq = cur_line.find_first_of('=');
    std::string key = cur_line.substr(0, cur_line_eq);
    std::string value = cur_line.substr(cur_line_eq + 1);
    //Trim whitespace
    key = StringUtils::TrimWhitespace(key);
    value = StringUtils::TrimWhitespace(value);

    //Shorthand cases
    if(StringUtils::StartsWith(key, "-")) {
        std::string sub_key = key.substr(1);
        SetValue(sub_key, false);
        return true;
    }
    if(StringUtils::StartsWith(key, "+")) {
        std::string sub_key = key.substr(1);
        SetValue(sub_key, true);
        return true;
    }

    if(key.find('"') != std::string::npos) {
        key = key.substr(key.find_first_not_of("\""), key.find_last_not_of("\""));
    }
    if(value.find('"') != std::string::npos) {
        auto ffno = value.find_first_not_of('"');
        auto flno = value.find_last_not_of('"');
        if(ffno == std::string::npos || flno == std::string::npos) {
            if(ffno == std::string::npos) {
                value = value.substr(1);
            }
            if(flno == std::string::npos) {
                value.pop_back();
            }
        } else {
            value = value.substr(ffno, flno);
        }
    }

    SetValue(key, value);
    return true;
}

bool KeyValueParser::Parse(std::ifstream& input) {
    if(input.is_open() == false) {
        return false;
    }
    if(input.good() == false) {
        return false;
    }
    return Parse(static_cast<std::istream&>(input));
}

bool KeyValueParser::Parse(std::istream& input) {
    std::string cur_line;
    while(std::getline(input, cur_line)) {
        bool did_parse = Parse(cur_line);
        if(!did_parse) {
            return false;
        }
    }
    return true;
}

std::map<std::string, std::string>&& KeyValueParser::Release() {
    return std::move(_kv_pairs);
}

bool KeyValueParser::ParseMultiParams(const std::string& input) {
    std::string whole_line = input;
    CollapseMultiParamWhitespace(whole_line);
    ConvertFromMultiParam(whole_line);
    std::istringstream ss(whole_line);
    return Parse(ss);
}

void KeyValueParser::ConvertFromMultiParam(std::string& whole_line) {
    //Replace space-delimited KVPs with newlines;
    bool inQuote = false;
    for(auto iter = whole_line.begin(); iter != whole_line.end(); ++iter) {
        if(*iter == '"') {
            inQuote = !inQuote;
            continue;
        }
        if(!inQuote) {
            if(*iter == ' ') {
                whole_line.replace(iter, iter + 1, "\n");
            }
        }
    }
}

void KeyValueParser::CollapseMultiParamWhitespace(std::string& whole_line) {
    //Remove spaces around equals
    auto eq_loc = std::find(whole_line.begin(), whole_line.end(), '=');
    while(eq_loc != std::end(whole_line)) {
        auto left_space_eq = eq_loc - 1;
        while(std::isspace(*left_space_eq, std::locale(""))) {
            left_space_eq--;
        }
        eq_loc = whole_line.erase(left_space_eq + 1, eq_loc);
        auto right_space_eq = eq_loc + 1;
        while(std::isspace(*right_space_eq, std::locale(""))) {
            right_space_eq++;
        }
        eq_loc = whole_line.erase(eq_loc + 1, right_space_eq);
        eq_loc = std::find(eq_loc + 1, whole_line.end(), '=');
    }
    //Remove consecutive spaces
    whole_line.erase(std::unique(whole_line.begin(), whole_line.end(),
                                 [](char lhs, char rhs) {
        return lhs == rhs && std::isspace(lhs, std::locale(""));
    }),
                     whole_line.end());
}

void KeyValueParser::SetValue(const std::string& key, const std::string& value) {
    _kv_pairs[key] = value;
}

void KeyValueParser::SetValue(const std::string& key, const bool& value) {
    _kv_pairs[key] = value ? std::string{ "true" } : std::string{ "false" };
}
