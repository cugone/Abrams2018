#include "Engine/Core/Config.hpp"

#include "Engine/Core/ArgumentParser.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

#include <algorithm>
#include <locale>
#include <sstream>

void Config::GetValue(const std::string& key, char& value) {
    auto found = _config.find(key);
    if(found != _config.end()) {
        value = *(found->second.begin());
    }
}

void Config::GetValue(const std::string& key, unsigned char& value) {
    auto found = _config.find(key);
    if(found != _config.end()) {
        value = static_cast<unsigned char>(std::stoul(found->second));
    }
}

void Config::GetValue(const std::string& key, signed char& value) {
    auto found = _config.find(key);
    if(found != _config.end()) {
        value = static_cast<signed char>(std::stoi(found->second));
    }
}

void Config::GetValue(const std::string& key, bool& value) {
    auto found = _config.find(key);
    if(found != _config.end()) {
        int keyAsInt = -9999;
        try {
            keyAsInt = std::stoi(found->second);
            value = keyAsInt != 0;
        } catch(...) {
            std::string keyAsString = StringUtils::ToLowerCase(found->second);
            if(found->second == "true") {
                value = true;
            } else if(found->second == "false") {
                value = false;
            } else {
                value = false;
            }
        }
    }
}

void Config::GetValue(const std::string& key, unsigned int& value) {
    auto found = _config.find(key);
    if(found != _config.end()) {
        value = static_cast<unsigned int>(std::stoul(found->second));
    }
}

void Config::GetValue(const std::string& key, int& value) {
    auto found = _config.find(key);
    if(found != _config.end()) {
        value = std::stoi(found->second);
    }
}

void Config::GetValue(const std::string& key, long& value) {
    auto found = _config.find(key);
    if(found != _config.end()) {
        value = std::stol(found->second);
    }
}

void Config::GetValue(const std::string& key, unsigned long& value) {
    auto found = _config.find(key);
    if(found != _config.end()) {
        value = std::stoul(found->second);
    }
}

void Config::GetValue(const std::string& key, long long& value) {
    auto found = _config.find(key);
    if(found != _config.end()) {
        value = std::stoll(found->second);
    }
}

void Config::GetValue(const std::string& key, unsigned long long& value) {
    auto found = _config.find(key);
    if(found != _config.end()) {
        value = std::stoull(found->second);
    }
}

void Config::GetValue(const std::string& key, float& value) {
    auto found = _config.find(key);
    if(found != _config.end()) {
        value = std::stof(found->second);
    }
}

void Config::GetValue(const std::string& key, double& value) {
    auto found = _config.find(key);
    if(found != _config.end()) {
        value = std::stod(found->second);
    }
}

void Config::GetValue(const std::string& key, long double& value) {
    auto found = _config.find(key);
    if(found != _config.end()) {
        value = std::stold(found->second);
    }
}

void Config::GetValue(const std::string& key, std::string& value) {
    auto found = _config.find(key);
    if(found != _config.end()) {
        value = found->second;
    }
}

void Config::SetValue(const std::string& key, const char& value) {
    _config[key] = value;
}

void Config::SetValue(const std::string& key, const unsigned char& value) {
    _config[key] = value;
}

void Config::SetValue(const std::string& key, const signed char& value) {
    _config[key] = value;
}

void Config::SetValue(const std::string& key, const bool& value) {
    _config[key] = value ? "true" : "false";
}

void Config::SetValue(const std::string& key, const unsigned int& value) {
    _config[key] = std::to_string(value);
}

void Config::SetValue(const std::string& key, const int& value) {
    _config[key] = std::to_string(value);
}

void Config::SetValue(const std::string& key, const long& value) {
    _config[key] = std::to_string(value);
}

void Config::SetValue(const std::string& key, const unsigned long& value) {
    _config[key] = std::to_string(value);
}

void Config::SetValue(const std::string& key, const long long& value) {
    _config[key] = std::to_string(value);
}

void Config::SetValue(const std::string& key, const unsigned long long& value) {
    _config[key] = std::to_string(value);
}

void Config::SetValue(const std::string& key, const float& value) {
    _config[key] = std::to_string(value);
}

void Config::SetValue(const std::string& key, const double& value) {
    _config[key] = std::to_string(value);
}

void Config::SetValue(const std::string& key, const long double& value) {
    _config[key] = std::to_string(value);
}

void Config::SetValue(const std::string& key, const std::string& value) {
    _config[key] = value;
}

void Config::SetValue(const std::string& key, const char* value) {
    SetValue(key, std::string(value));
}

bool Config::Parse(std::string& input) {
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

bool Config::ParseMultiParams(const std::string& input) {
    std::string whole_line = input;
    CollapseMultiParamWhitespace(whole_line);
    ConvertFromMultiParam(whole_line);
    std::istringstream ss(whole_line);
    return Parse(ss);
}

void Config::ConvertFromMultiParam(std::string &whole_line) {
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

void Config::CollapseMultiParamWhitespace(std::string &whole_line) {
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

bool Config::Parse(std::ifstream& input) {
    if(input.is_open() == false) {
        DebuggerPrintf("\nFile stream not initially open.");
        return false;
    }
    if(input.good() == false) {
        DebuggerPrintf("\nFile stream not initially in a good state.");
        return false;
    }
    return Parse(static_cast<std::istream&>(input));
}

bool Config::Parse(std::istream& input) {
    std::string cur_line;
    while(std::getline(input, cur_line)) {
        bool did_parse = Parse(cur_line);
        if(!did_parse) {
            return false;
        }
    }
    return true;
}

void Config::PrintConfigs(std::ostream& output /*= std::cout*/) const {
    for(auto iter = _config.begin(); iter != _config.end(); ++iter) {
        bool value_has_space = iter->second.find(' ') != std::string::npos;
        output << iter->first << '=';
        if(value_has_space) {
            output << '"';
        }
        output << iter->second;
        if(value_has_space) {
            output << '"';
        }
        output << '\n';
    }
}

std::ostream& operator<<(std::ostream& output, const Config& config) {
    config.PrintConfigs(output);
    return output;
}

std::istream& operator >> (std::istream& input, Config& config) {
    config.Parse(input);
    return input;
}