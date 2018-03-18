#pragma once

#include "Engine/RHI/RHIInstance.hpp"
#include "Engine/Renderer/OpenGL/OGL.hpp"

class IntVector2;

class OGLInstance : public RHIInstance {
public:
    OGLInstance();
    ~OGLInstance();

    virtual RHIOutput* CreateOutput(const IntVector2& client_size, const IntVector2& client_position, const RHIOutputMode& output_mode) override;

protected:
private:
};