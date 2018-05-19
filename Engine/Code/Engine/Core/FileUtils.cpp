#include "Engine/Core/FileUtils.hpp"

#include <cstdio>
#include <filesystem>

#include <fstream>

#include "Engine/Core/StringUtils.hpp"

namespace FileUtils {

bool WriteBufferToFile(void* buffer, std::size_t size, const std::string& filePath) {

    namespace FS = std::experimental::filesystem;
    FS::path p(filePath);
    bool not_valid_path = !FS::is_directory(p);
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

    namespace FS = std::experimental::filesystem;
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
    namespace FS = std::experimental::filesystem;

    FS::path p(filepath);
    FS::path parent_path = (FS::is_directory(p) ? p : p.parent_path());
    if(!FS::exists(parent_path)) {
        FS::create_directories(parent_path);
        return true;
    }
    return false;
}

} //End FileUtils