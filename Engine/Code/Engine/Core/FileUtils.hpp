#pragma once

#include <filesystem>
#include <functional>
#include <string>
#include <vector>

namespace FileUtils {

bool WriteBufferToFile(void* buffer, std::size_t size, const std::string& filePath);
bool ReadBufferFromFile(std::vector<unsigned char>& out_buffer, const std::string& filePath);
bool CreateFolders(const std::string& filepath);
std::string GetAppDataPath();
void IterateFileInFolders(const std::filesystem::path& folderpath, const std::string& validExtension = std::string{}, const std::function<void(const std::filesystem::path&)>& callback = [](const std::filesystem::path& /*p*/) { /* DO NOTHING */ }, bool recursive = false);

} //End FileUtils