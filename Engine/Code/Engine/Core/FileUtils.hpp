#pragma once

#include <filesystem>
#include <functional>
#include <string>
#include <vector>

namespace FileUtils {

bool WriteBufferToFile(void* buffer, std::size_t size, const std::string& filePath);
bool ReadBufferFromFile(std::vector<unsigned char>& out_buffer, const std::string& filePath);
bool CreateFolders(const std::string& filepath);
std::filesystem::path GetAppDataPath();
std::filesystem::path GetExePath();
void IterateFileInFolders(const std::filesystem::path& folderpath, const std::string& validExtensionList = std::string{}, const std::function<void(const std::filesystem::path&)>& callback = [](const std::filesystem::path& /*p*/) { /* DO NOTHING */ }, bool recursive = false);
int CountFilesInFolders(const std::filesystem::path& folderpath, const std::string& validExtensionList = std::string{}, bool recursive = false);
void RemoveExceptMostRecentFiles(const std::filesystem::path& folderpath, int mostRecentCountToKeep);

} //End FileUtils