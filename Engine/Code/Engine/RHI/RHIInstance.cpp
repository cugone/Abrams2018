#include "Engine/RHI/RHIInstance.hpp"

#include "Engine/Renderer/DirectX/DXInstance.hpp"
#include "Engine/Renderer/OpenGL/OGLInstance.hpp"

RHIInstance* RHIInstance::_instance = nullptr;

RHIInstance* RHIInstance::CreateInstance(const RHIInstanceType& instance_type) {
    if(_instance) {
        return _instance;
    }
    switch(instance_type) {
        case RHIInstanceType::DIRECTX:
            _instance = new DXInstance();
            break;
        case RHIInstanceType::OPENGL:
            _instance = new OGLInstance();
            break;
        default:
            break;
    }
    return _instance;
}

void RHIInstance::DestroyInstance() {
    if(_instance) {
        delete _instance;
        _instance = nullptr;
    }
}

RHIOutput* RHIInstance::CreateOutput(const IntVector2& /*client_size*/, const IntVector2& /*client_position*/, const RHIOutputMode& /*output_mode*/) {
    return nullptr;
}

RHIInstance::RHIInstance() {
    /* DO NOTHING */
}

RHIInstance::~RHIInstance() {
    /* DO NOTHING */
}
