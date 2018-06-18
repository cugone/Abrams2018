#include "Engine/Core/FileUtils.hpp"

#include "Engine/Core/StringUtils.hpp"

#include <cstdio>
#include <fstream>
#include <ShlObj.h>


namespace FileUtils {

bool WriteBufferToFile(void* buffer, std::size_t size, const std::string& filePath) {

    namespace FS = std::filesystem;
    FS::path p(filePath);
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
    bool not_valid_path = FS::is_directory(p) || !FS::exists(p);
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
    return FS::create_directories(p);
}

std::string GetAppDataPath() {
    namespace FS = std::filesystem;
    PWSTR ppszPath = nullptr;
    bool success = SUCCEEDED(::SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_DEFAULT, nullptr, &ppszPath));
    if(success) {
        FS::path p(ppszPath);
        ::CoTaskMemFree(ppszPath);
        return p.string();
    }
    return std::string{};
}

void IterateFileInFolders(const std::filesystem::path& folderpath, const std::string& validExtension /*= std::string{}*/, const std::function<void(const std::filesystem::path&)>& callback /*= [](const std::filesystem::path& p) { (void*)p; }*/, bool recursive /*= false*/) {
    namespace FS = std::filesystem;
    bool exists = FS::exists(folderpath);
    bool is_folder = exists && FS::is_directory(folderpath);
    if(!is_folder) {
        return;
    }
    if(!recursive) {
        for(auto iter = FS::directory_iterator{ folderpath }; iter != FS::directory_iterator{}; ++iter) {
            auto cur_path = iter->path();
            auto my_extension = StringUtils::ToLowerCase(cur_path.extension().string());
            auto valid_extension = StringUtils::ToLowerCase(validExtension);
            bool valid_file_by_extension = my_extension == valid_extension;
            if(valid_extension.empty() == false) {
                if(valid_file_by_extension) {
                    callback(cur_path);
                }
            } else {
                callback(cur_path);
            }
        }
    } else {
        for(auto iter = FS::recursive_directory_iterator{ folderpath }; iter != FS::recursive_directory_iterator{}; ++iter) {
            auto cur_path = iter->path();
            auto my_extension = StringUtils::ToLowerCase(cur_path.extension().string());
            auto valid_extension = StringUtils::ToLowerCase(validExtension);
            bool valid_file_by_extension = my_extension == valid_extension;
            if(valid_extension.empty() == false) {
                if(valid_file_by_extension) {
                    callback(cur_path);
                }
            } else {
                callback(cur_path);
            }
        }
    }
}

} //End FileUtils