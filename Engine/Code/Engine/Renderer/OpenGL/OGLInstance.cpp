#include "Engine/Renderer/OpenGL/OGLInstance.hpp"

OGLInstance::OGLInstance()
    : RHIInstance()
{
    /* DO NOTHING */
}

OGLInstance::~OGLInstance() {
    /* DO NOTHING */
}

RHIOutput* OGLInstance::CreateOutput(const IntVector2& /*client_size*/, const IntVector2& /*client_position*/, const RHIOutputMode& /*output_mode*/) {
    return nullptr;
}

