#include "Engine/Renderer/Renderer.hpp"

#include <filesystem>
#include "Engine/Renderer/Texture.hpp"

Renderer::Renderer(unsigned int width, unsigned int height)
    : _window_dimensions(width, height)
{
    /* DO NOTHING */
}

Renderer::~Renderer() {
    for(auto& texture : _textures) {
        delete texture.second;
        texture.second = nullptr;
    }
    _textures.clear();
}

bool Renderer::RegisterTexture(const std::wstring& name, Texture* texture) {
    auto found_texture = _textures.find(name);
    if(found_texture == _textures.end()) {
        _textures.insert_or_assign(name, texture);
        return true;
    } else {
        return false;
    }
}

Texture* Renderer::GetTexture(const std::wstring& filepath) {
    namespace FS = std::experimental::filesystem;
    FS::path p(filepath);
    return _textures[p.wstring()];
}

RHIDeviceContext* Renderer::GetDeviceContext() const {
    return _rhi_context;
}

RHIDevice* Renderer::GetDevice() const {
    return _rhi_device;
}

RHIOutput* Renderer::GetOutput() const {
    return _rhi_output;
}
