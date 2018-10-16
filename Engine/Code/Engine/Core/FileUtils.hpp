#pragma once

#include <filesystem>
#include <functional>
#include <string>
#include <vector>

namespace FileUtils {

bool WriteBufferToFile(void* buffer, std::size_t size, const std::string& filePath);
bool WriteBufferToFile(const std::string& buffer, const std::string& filePath);
bool ReadBufferFromFile(std::vector<unsigned char>& out_buffer, const std::string& filePath);
bool ReadBufferFromFile(std::string& out_buffer, const std::string& filepath);
bool CreateFolders(const std::string& filepath);
std::filesystem::path GetAppDataPath();
std::filesystem::path GetExePath();
void IterateFilesInFolders(const std::filesystem::path& folderpath, const std::string& validExtensionList = std::string{}, const std::function<void(const std::filesystem::path&)>& callback = [](const std::filesystem::path& /*p*/) { /* DO NOTHING */ }, bool recursive = false);
int CountFilesInFolders(const std::filesystem::path& folderpath, const std::string& validExtensionList = std::string{}, bool recursive = false);
void RemoveExceptMostRecentFiles(const std::filesystem::path& folderpath, int mostRecentCountToKeep);

namespace detail {
    template<typename DirectoryIteratorType>
    void IterateFileInFolders(const std::filesystem::path& preferred_folderpath, const std::vector<std::string>& validExtensions, const std::function<void(const std::filesystem::path&)>& callback) {
        if(validExtensions.empty()) {
            std::for_each(DirectoryIteratorType{ preferred_folderpath }, DirectoryIteratorType{},
                [&callback](const std::filesystem::directory_entry& entry) {
                    const auto& cur_path = entry.path();
                    callback(cur_path);
                });
            return;
        }
        std::for_each(DirectoryIteratorType{ preferred_folderpath }, DirectoryIteratorType{},
            [&validExtensions, &callback](const std::filesystem::directory_entry& entry) {
                const auto& cur_path = entry.path();
                std::string my_extension = StringUtils::ToLowerCase(cur_path.extension().string());
                bool valid_file_by_extension = std::find(std::begin(validExtensions), std::end(validExtensions), my_extension) != std::end(validExtensions);
                if(valid_file_by_extension) {
                    callback(cur_path);
                }
            });
    }
} //End detail

} //End FileUtils