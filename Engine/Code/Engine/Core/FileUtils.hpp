#pragma once

#include <string>
#include <vector>

namespace FileUtils {

bool WriteBufferToFile(void* buffer, std::size_t size, const std::wstring& filePath);
bool ReadBufferFromFile(std::vector<unsigned char>& out_buffer, const std::wstring& filePath);
bool CreateFolders(const std::wstring& filepath);

} //End FileUtils