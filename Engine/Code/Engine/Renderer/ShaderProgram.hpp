#pragma once

#include "Engine/Renderer/DirectX/DX11.hpp"

class InputLayout;
class RHIDevice;

class ShaderProgram {
public:
    ShaderProgram(const std::string& name,
                  RHIDevice* device,
                  ID3D11VertexShader* vs,
                  ID3D11PixelShader* ps,
                  ID3DBlob* vs_bytecode,
                  ID3DBlob* ps_bytecode,
                  InputLayout* input_layout,
                  ID3D11HullShader* hs = nullptr,
                  ID3DBlob* hs_bytecode = nullptr,
                  ID3D11DomainShader* ds = nullptr,
                  ID3DBlob* ds_bytecode = nullptr,
                  ID3D11GeometryShader* gs = nullptr,
                  ID3DBlob* gs_bytecode = nullptr,
                  ID3D11ComputeShader* cs = nullptr,
                  ID3DBlob* cs_bytecode = nullptr
                  );
    ~ShaderProgram();

    std::string GetName() const;
    RHIDevice* GetParentDevice();
    ID3DBlob* GetVSByteCode() const;
    ID3DBlob* GetHSByteCode() const;
    ID3DBlob* GetDSByteCode() const;
    ID3DBlob* GetGSByteCode() const;
    ID3DBlob* GetPSByteCode() const;
    ID3DBlob* GetCSByteCode() const;
    InputLayout* GetInputLayout() const;
    ID3D11VertexShader* GetVS() const;
    ID3D11HullShader* GetHS() const;
    ID3D11DomainShader* GetDS() const;
    ID3D11GeometryShader* GetGS() const;
    ID3D11PixelShader* GetPS() const;
    ID3D11ComputeShader* GetCS() const;

protected:
private:
    std::string _name = "UNNAMED SHADER PROGRAM";
    RHIDevice* _device = nullptr;
    ID3DBlob* _vs_bytecode = nullptr;
    ID3DBlob* _hs_bytecode = nullptr;
    ID3DBlob* _ds_bytecode = nullptr;
    ID3DBlob* _gs_bytecode = nullptr;
    ID3DBlob* _ps_bytecode = nullptr;
    ID3DBlob* _cs_bytecode = nullptr;
    InputLayout* _il = nullptr;
    ID3D11VertexShader* _vs = nullptr;
    ID3D11HullShader* _hs = nullptr;
    ID3D11DomainShader* _ds = nullptr;
    ID3D11GeometryShader* _gs = nullptr;
    ID3D11PixelShader* _ps = nullptr;
    ID3D11ComputeShader* _cs = nullptr;

};