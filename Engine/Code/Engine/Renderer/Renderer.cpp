#include "Engine/Renderer/Renderer.hpp"

#include <filesystem>

#include "Engine/RHI/RHIInstance.hpp"
#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"
#include "Engine/RHI/RHIOutput.hpp"

#include "Engine/Renderer/DepthStencilState.hpp"
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

    delete _default_depthstencil;
    _default_depthstencil = nullptr;

    delete _current_depthstencil_state;
    _current_depthstencil_state = nullptr;

    delete _rhi_context;
    _rhi_context = nullptr;

    delete _rhi_device;
    _rhi_device = nullptr;

    delete _rhi_output;
    _rhi_output = nullptr;

    RHIInstance::DestroyInstance();
    _rhi_instance = nullptr;

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
