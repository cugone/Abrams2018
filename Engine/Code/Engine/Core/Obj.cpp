#include "Engine/Core/Obj.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"

#include <numeric>
#include <string>
#include <sstream>

namespace FileUtils {

Obj::Obj(const std::string& filepath)
    : Obj(std::filesystem::path(filepath))
{
    /* DO NOTHING */    
}

Obj::Obj(const std::filesystem::path& filepath) {
    auto path_copy = filepath;
    path_copy.make_preferred();
}

//Run only as an asynchronous operation highly recommended.
bool Obj::Load(const std::string& filepath) {
    namespace FS = std::filesystem;
    FS::path p(filepath);
    p.make_preferred();
    return Load(p);
}

bool Obj::Load(const std::filesystem::path& filepath) {
    namespace FS = std::filesystem;
    bool not_exist = !FS::exists(filepath);
    std::string valid_extension = ".obj";
    bool not_obj = StringUtils::ToLowerCase(filepath.extension().string()) != valid_extension;
    bool invalid = not_exist || not_obj;
    if(invalid) {
        std::ostringstream ss;
        ss << filepath << " is not a .obj file";
        DebuggerPrintf(ss.str().c_str());
        return false;
    }
    return Parse(filepath);
}

//Run only as an asynchronous operation highly recommended.
bool Obj::Save(const std::string& filepath) {
    namespace FS = std::filesystem;
    FS::path p(filepath);
    p.make_preferred();
    return Save(p);
}

bool Obj::Save(const std::filesystem::path& filepath) {
    std::ostringstream buffer;
    for(auto& v : _verts) {
        buffer << "v " << v.x << ' ' << v.y << ' ' << v.z << '\n';
    }
    for(auto& v : _normals) {
        buffer << "vn " << v.x << ' ' << v.y << ' ' << v.z << '\n';
    }
    for(auto& v : _tex_coords) {
        buffer << "vt " << v.x << ' ' << v.y << '\n';
    }
    bool has_vn = !_normals.empty();
    bool has_vt = !_tex_coords.empty();
    bool has_neither = !has_vt && !has_vn;
    for(auto iter = std::begin(_ibo); iter != std::end(_ibo); /* DO NOTHING */) {
        auto value1 = *iter;
        ++iter;
        auto value2 = *iter;
        ++iter;
        auto value3 = *iter;
        ++iter;
        buffer << "f ";
        buffer << (1 + value1);
        if(!has_neither) {
            buffer << '/';
            if(has_vt) {
                buffer << (1 + value1);
            }
            buffer << '/';
            if(has_vn) {
                buffer << (1 + value1);
            }
        }
        buffer << ' ';
        buffer << (1 + value2);
        if(!has_neither) {
            buffer << '/';
            if(has_vt) {
                buffer << (1 + value2);
            }
            buffer << '/';
            if(has_vn) {
                buffer << (1 + value2);
            }
        }
        buffer << ' ';
        buffer << (1 + value3);
        if(!has_neither) {
            buffer << '/';
            if(has_vt) {
                buffer << (1 + value3);
            }
            buffer << '/';
            if(has_vn) {
                buffer << (1 + value3);
            }
        }
        buffer << '\n';
    }
    buffer.flush();
    return FileUtils::WriteBufferToFile(buffer.str().data(), buffer.str().size(), filepath.string());
}

bool Obj::IsLoaded() const {
    return _is_loaded;
}

Obj::~Obj() {
    _vbo.clear();
    _vbo.shrink_to_fit();
    _ibo.clear();
    _ibo.shrink_to_fit();
}

const std::vector<Vertex3D>& Obj::GetVbo() const {
    return _vbo;
}

const std::vector<unsigned int>& Obj::GetIbo() const {
    return _ibo;
}

bool Obj::Parse(const std::filesystem::path& filepath) {
    std::vector<unsigned char> buffer{};
    if(FileUtils::ReadBufferFromFile(buffer, filepath.string())) {
        std::stringstream ss{};
        if(ss.write(reinterpret_cast<const char*>(buffer.data()), buffer.size())) {
            buffer.clear();
            buffer.shrink_to_fit();
            ss.clear();
            ss.seekg(ss.beg);
            ss.seekp(ss.beg);
            std::string cur_line{};
            std::size_t vert_count{};
            while(std::getline(ss, cur_line, '\n')) {
                if(StringUtils::StartsWith(cur_line, "v ")) {
                    ++vert_count;
                }
            }
            ss.clear();
            ss.seekg(ss.beg);
            ss.seekp(ss.beg);
            _verts.reserve(vert_count);
            _vbo.resize(vert_count);
            while(std::getline(ss, cur_line, '\n')) {
                if(StringUtils::StartsWith(cur_line, "v ")) {
                    auto elems = StringUtils::Split(StringUtils::TrimWhitespace(std::string{std::begin(cur_line) + 2, std::end(cur_line)}), ' ');
                    std::string v_str = {"["};
                    v_str += StringUtils::Join(elems, ',');
                    v_str += "]";
                    Vector4 v(v_str);
                    v.CalcHomogeneous();
                    _verts.emplace_back(v);
                } else if(StringUtils::StartsWith(cur_line, "vt ")) {
                    auto elems = StringUtils::Split(StringUtils::TrimWhitespace(std::string{ std::begin(cur_line) + 3, std::end(cur_line) }), ' ');
                    std::string v_str = { "[" };
                    v_str += StringUtils::Join(elems, ',');
                    v_str += "]";
                    _tex_coords.emplace_back(v_str);
                } else if(StringUtils::StartsWith(cur_line, "vn ")) {
                    auto elems = StringUtils::Split(StringUtils::TrimWhitespace(std::string{ std::begin(cur_line) + 3, std::end(cur_line) }), ' ');
                    std::string v_str = { "[" };
                    v_str += StringUtils::Join(elems, ',');
                    v_str += "]";
                    _normals.emplace_back(v_str);
                } else if(StringUtils::StartsWith(cur_line, "f ")) {
                    if(cur_line.find('-') != std::string::npos) {
                        DebuggerPrintf("OBJ implementation does not support relative reference numbers!\n");
                        return false;
                    }
                    auto tris = StringUtils::Split(StringUtils::TrimWhitespace(std::string{ std::begin(cur_line) + 2, std::end(cur_line) }), ' ');
                    if(tris.size() != 3) {
                        DebuggerPrintf("OBJ implementation does not support non-triangle faces!\n");
                        return false;
                    }
                    for(auto& t : tris) {
                        auto elems = StringUtils::Split(t, '/', false);
                        Vertex3D vertex{};
                        auto elem_count = elems.size();
                        std::size_t cur_vbo_index = 0;
                        for(auto i = 0u; i < elem_count; ++i) {
                            switch(i) {
                                case 0:
                                    if(!elems[0].empty()) {
                                        std::size_t cur_v = std::stoul(elems[0]);
                                        cur_vbo_index = cur_v - 1;
                                        vertex.position = _verts[cur_v - 1];
                                        _ibo.push_back(cur_v - 1);
                                    }
                                    break;
                                case 1:
                                    if(!elems[1].empty()) {
                                        std::size_t cur_vt = std::stoul(elems[1]);
                                        vertex.texcoords = Vector2{ _tex_coords[cur_vt - 1] };
                                    }
                                    break;
                                case 2:
                                    if(!elems[2].empty()) {
                                        std::size_t cur_vn = std::stoul(elems[2]);
                                        vertex.normal = _normals[cur_vn - 1];
                                    }
                                    break;
                                default: break;
                            }
                        }
                        _vbo[cur_vbo_index] = vertex;
                    }
                } else {
                    /* DO NOTHING */
                }
            }
            _ibo.shrink_to_fit();
            _is_loaded = true;
            return true;
        }
    }
    return false;
}

} //End FileUtils