#pragma once

#include "Engine/Core/Vertex3D.hpp"

#include <atomic>
#include <condition_variable>
#include <filesystem>
#include <mutex>
#include <thread>
#include <string>
#include <vector>

namespace FileUtils {

class Obj {
public:
    Obj() = default;
    Obj(const Obj& other) = default;
    Obj(Obj&& other) = default;
    Obj& operator=(const Obj& rhs) = default;
    Obj& operator=(Obj&& rhs) = default;
    explicit Obj(const std::string& filepath);
    ~Obj();

    const std::vector<Vertex3D>& GetVbo() const;
    const std::vector<unsigned int>& GetIbo() const;

    bool Load(const std::string& filepath);
    bool Save(const std::string& filepath);
    bool IsLoaded() const;
protected:
private:
    Obj(const std::filesystem::path& filepath);

    bool Load(const std::filesystem::path& filepath);
    bool Save(const std::filesystem::path& filepath);
    bool Parse(const std::filesystem::path& filepath);

    std::vector<Vertex3D> _vbo{};
    std::vector<unsigned int> _ibo{};
    std::vector<Vector3> _verts{};
    std::vector<Vector3> _tex_coords{};
    std::vector<Vector3> _normals{};
    bool _is_loaded = false;
};

} //End FileUtils