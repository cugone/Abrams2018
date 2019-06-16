#pragma once

#include "Engine/Core/Vertex3D.hpp"

#include <atomic>
#include <condition_variable>
#include <filesystem>
#include <mutex>
#include <thread>
#include <tuple>
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
        Obj(std::filesystem::path filepath);
        ~Obj() = default;

        const std::vector<Vertex3D>& GetVbo() const;
        const std::vector<unsigned int>& GetIbo() const;

        void Unload();
        bool Load(std::filesystem::path filepath);
        bool Save(std::filesystem::path filepath);
        bool IsLoaded() const;
        bool IsLoading() const;
        bool IsSaving() const;
        bool IsSaved() const;
    protected:
    private:
        bool Parse(const std::filesystem::path& filepath);

        void PrintErrorToDebugger(std::filesystem::path filepath, const std::string& elementType, unsigned long long line_index) const;

        std::vector<Vertex3D> _vbo{};
        std::vector<unsigned int> _ibo{};
        std::vector<Vector3> _verts{};
        std::vector<Vector3> _tex_coords{};
        std::vector<Vector3> _normals{};
        std::vector<std::tuple<std::size_t, std::size_t, std::size_t>> _face_idxs{};
        std::atomic_bool _is_loaded = false;
        std::atomic_bool _is_loading = false;
        std::atomic_bool _is_saving = false;
        std::atomic_bool _is_saved = false;
};

} //End FileUtils