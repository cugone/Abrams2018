#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <string>

class Config {
public:
    Config() = default;
    ~Config() = default;

    void GetValue(const std::string& key, char& value);
    void GetValue(const std::string& key, unsigned char& value);
    void GetValue(const std::string& key, signed char& value);
    void GetValue(const std::string& key, bool& value);
    void GetValue(const std::string& key, unsigned int& value);
    void GetValue(const std::string& key, int& value);
    void GetValue(const std::string& key, long& value);
    void GetValue(const std::string& key, unsigned long& value);
    void GetValue(const std::string& key, long long& value);
    void GetValue(const std::string& key, unsigned long long& value);
    void GetValue(const std::string& key, float& value);
    void GetValue(const std::string& key, double& value);
    void GetValue(const std::string& key, long double& value);
    void GetValue(const std::string& key, std::string& value);

    void SetValue(const std::string& key, const char& value);
    void SetValue(const std::string& key, const unsigned char& value);
    void SetValue(const std::string& key, const signed char& value);
    void SetValue(const std::string& key, const bool& value);
    void SetValue(const std::string& key, const unsigned int& value);
    void SetValue(const std::string& key, const int& value);
    void SetValue(const std::string& key, const long& value);
    void SetValue(const std::string& key, const unsigned long& value);
    void SetValue(const std::string& key, const long long& value);
    void SetValue(const std::string& key, const unsigned long long& value);
    void SetValue(const std::string& key, const float& value);
    void SetValue(const std::string& key, const double& value);
    void SetValue(const std::string& key, const long double& value);
    void SetValue(const std::string& key, const std::string& value);
    void SetValue(const std::string& key, const char* value);

    void PrintConfigs(std::ostream& output /*= std::cout*/) const;
    friend std::ostream& operator<<(std::ostream& output, const Config& config);
    friend std::istream& operator>>(std::istream& input, Config& config);


protected:
private:
    bool Parse(std::string& input);
    bool Parse(std::ifstream& input);
    bool Parse(std::istream& input);
    bool ParseMultiParams(const std::string& input);
    void ConvertFromMultiParam(std::string &whole_line);
    void CollapseMultiParamWhitespace(std::string &whole_line);
    std::map<std::string, std::string> _config{};
    
};
