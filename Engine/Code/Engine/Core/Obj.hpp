#pragma once

#include "Engine/Core/Vertex3D.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace FileUtils {

class Obj {
public:
    Obj(const std::string& filepath);
    ~Obj();

    void Render() const;

protected:
private:
    Obj(const std::filesystem::path& filepath);
    std::vector<Vertex3D> _vbo{};
    std::vector<unsigned int> _ibo{};
};

} //End FileUtils