#pragma once

#include "Engine/RHI/RHIDeviceContext.hpp"

class Rgba;
class Texture2D;
class RHIDevice;
class RasterState;
class BlendState;
class ComputeShader;
class ConstantBuffer;
class StructuredBuffer;
class ReadWriteBuffer;
class DepthStencilBuffer;
class DepthStencilState;
class ShaderProgram;
class Shader;
class Material;

struct ID3D11DeviceContext;

class DXDeviceContext : public RHIDeviceContext {
public:
    DXDeviceContext(RHIDevice* parentDevice = nullptr, ID3D11DeviceContext* dx_context = nullptr);
    virtual ~DXDeviceContext();

    ID3D11DeviceContext* GetDxContext() const;

    void UnbindAllShaderResources();


    virtual void ClearState();
    virtual void Flush();

    virtual void ClearColorTarget(Texture2D* output, const Rgba& color);
    virtual void ClearDepthStencilTarget(Texture2D* output
                                         , bool depth = true
                                         , bool stencil = true
                                         , float depthValue = 1.0f
                                         , unsigned char stencilValue = 0);


private:
    ID3D11DeviceContext* _dx_context;

};
