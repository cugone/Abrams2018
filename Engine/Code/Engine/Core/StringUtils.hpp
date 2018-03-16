#pragma once

#include <string>
#include <vector>

namespace StringUtils {

std::vector<std::string> Split(const std::string& string, char delim = ',', bool skip_empty = true);
std::string Join(const std::vector<std::string>& strings, char delim, bool skip_empty = true);
std::string Join(const std::vector<std::string>& strings, bool skip_empty = true);

std::string ToUpperCase(std::string string);
std::string ToLowerCase(std::string string);

}