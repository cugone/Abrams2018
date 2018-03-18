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


    virtual void ClearState() override;
    virtual void Flush() override;

    virtual void ClearColorTarget(Texture* output, const Rgba& color) override;
    virtual void ClearDepthStencilTarget(Texture* output
                                         , bool depth = true
                                         , bool stencil = true
                                         , float depthValue = 1.0f
                                         , unsigned char stencilValue = 0) override;

    virtual void SetTexture(unsigned int index, Texture* texture) override;


private:
    ID3D11DeviceContext* _dx_context;

};
