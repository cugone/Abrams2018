#include "Engine/Core/FileUtils.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <ShlObj.h>


namespace FileUtils {

bool WriteBufferToFile(void* buffer, std::size_t size, const std::string& filePath) {

    namespace FS = std::filesystem;
    FS::path p(filePath);
    p.make_preferred();
    bool not_valid_path = FS::is_directory(p);
    if(not_valid_path) {
        return false;
    }

    std::ofstream ofs;
    ofs.open(p.string(), std::ios_base::binary);
    if(ofs.write(reinterpret_cast<const char*>(buffer), size)) {
        ofs.close();
        return true;
    }
    ofs.close();
    return false;
}

bool ReadBufferFromFile(std::vector<unsigned char>& out_buffer, const std::string& filePath) {

    namespace FS = std::filesystem;
    FS::path p(filePath);
    p.make_preferred();
    bool path_is_directory = FS::is_directory(p);
    bool path_not_exist = !FS::exists(p);
    bool not_valid_path = path_is_directory || path_not_exist;
    if(not_valid_path) {
        return false;
    }

    std::ifstream ifs;
    ifs.open(p, std::ios_base::binary);
    ifs.seekg(0, std::ios_base::end);
    auto byte_size = ifs.tellg();
    ifs.seekg(0, std::ios_base::beg);
    ifs.clear();
    out_buffer.resize(static_cast<std::size_t>(byte_size));
    bool success = false;
    if(ifs.read(reinterpret_cast<char*>(out_buffer.data()), out_buffer.size())) {
        success = true;
    }
    ifs.close();
    return success;
}

bool CreateFolders(const std::string& filepath) {
    namespace FS = std::filesystem;

    FS::path p(filepath);
    p.make_preferred();
    return FS::create_directories(p);
}

std::filesystem::path GetAppDataPath() {
    namespace FS = std::filesystem;
    FS::path p{};
    {
        PWSTR ppszPath = nullptr;
        bool success = SUCCEEDED(::SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_DEFAULT, nullptr, &ppszPath));
        if(success) {
            p = FS::path(ppszPath);
            ::CoTaskMemFree(ppszPath);
            p.make_preferred();
        }
    }
    return p;
}

std::filesystem::path GetExePath() {
    namespace FS = std::filesystem;
    FS::path result{};
    {
        TCHAR filename[MAX_PATH];
        ::GetModuleFileName(nullptr, filename, MAX_PATH);
        result = FS::path(filename);
        result.make_preferred();
    }
    return result;
}

void IterateFileInFolders(const std::filesystem::path& folderpath, const std::string& validExtensionList /*= std::string{}*/, const std::function<void(const std::filesystem::path&)>& callback /*= [](const std::filesystem::path& p) { (void*)p; }*/, bool recursive /*= false*/) {
    namespace FS = std::filesystem;
    auto preferred_folderpath = folderpath;
    preferred_folderpath.make_preferred();
    bool exists = FS::exists(preferred_folderpath);
    bool is_directory = FS::is_directory(preferred_folderpath);
    bool is_folder = exists && is_directory;
    if(!is_folder) {
        return;
    }
    auto validExtensions = StringUtils::Split(StringUtils::ToLowerCase(validExtensionList));
    if(!recursive) {
        for(auto iter = FS::directory_iterator{ preferred_folderpath }; iter != FS::directory_iterator{}; ++iter) {
            auto cur_path = iter->path();
            auto my_extension = StringUtils::ToLowerCase(cur_path.extension().string());
            auto valid_file_by_extension = std::find(std::begin(validExtensions), std::end(validExtensions), my_extension) != std::end(validExtensions);
            if(validExtensions.empty() == false) {
                if(valid_file_by_extension) {
                    callback(cur_path);
                }
            } else {
                callback(cur_path);
            }
        }
    } else {
        for(auto iter = FS::recursive_directory_iterator{ preferred_folderpath }; iter != FS::recursive_directory_iterator{}; ++iter) {
            auto cur_path = iter->path();
            if(FS::is_directory(cur_path)) {
                continue;
            }
            auto my_extension = StringUtils::ToLowerCase(cur_path.extension().string());
            auto valid_file_by_extension = std::find(std::begin(validExtensions), std::end(validExtensions), my_extension) != std::end(validExtensions);
            if(validExtensions.empty() == false) {
                if(valid_file_by_extension) {
                    callback(cur_path);
                }
            } else {
                callback(cur_path);
            }
        }
    }
}

int CountFilesInFolders(const std::filesystem::path& folderpath, const std::string& validExtensionList /*= std::string{}*/, bool recursive /*= false*/) {
    int count = 0;
    auto cb = [&count](const std::filesystem::path& /*p*/)->void { ++count; };
    IterateFileInFolders(folderpath, validExtensionList, cb, recursive);
    return count;
}

void FileUtils::RemoveExceptMostRecentFiles(const std::filesystem::path& folderpath, int mostRecentCountToKeep) {
    namespace FS = std::filesystem;
    namespace Chrono = std::chrono;
    using Clock = Chrono::steady_clock;
    auto now = Clock::now();
    if(mostRecentCountToKeep < CountFilesInFolders(folderpath)) {
        std::vector<FS::path> paths{};
        auto add_path_cb = [&paths](const FS::path& p) { paths.push_back(p); };
        IterateFileInFolders(folderpath, std::string{}, add_path_cb, false);
        auto sort_pred = [](const FS::path& a, const FS::path& b) { return FS::last_write_time(a) > FS::last_write_time(b); };
        std::sort(std::begin(paths), std::end(paths), sort_pred);
        paths.erase(std::begin(paths), std::begin(paths) + mostRecentCountToKeep);
        for(auto& p : paths) {
            FS::remove(p);
        }
    }
}

} //End FileUtils