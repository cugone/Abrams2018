#pragma once

#include <string>
#include <vector>

namespace FileUtils {

bool WriteBufferToFile(void* buffer, std::size_t size, const std::string& filePath);
bool ReadBufferFromFile(std::vector<unsigned char>& out_buffer, const std::string& filePath);
bool CreateFolders(const std::string& filepath);
std::string GetAppDataPath();

} //End FileUtils