#include "Engine/Renderer/ShaderProgram.hpp"

#include "Engine/Renderer/InputLayout.hpp"

#include "Engine/RHI/RHIDevice.hpp"

ShaderProgram::ShaderProgram(const std::string& name
                             ,RHIDevice* device
                             ,ID3D11VertexShader* vs
                             ,ID3D11PixelShader* ps
                             ,ID3DBlob* vs_bytecode
                             ,ID3DBlob* ps_bytecode
                             ,InputLayout* input_layout
                             ,ID3D11HullShader* hs     /*= nullptr*/
                             ,ID3DBlob* hs_bytecode    /*= nullptr*/
                             ,ID3D11DomainShader* ds   /*= nullptr*/
                             ,ID3DBlob* ds_bytecode    /*= nullptr*/
                             ,ID3D11GeometryShader* gs /*= nullptr*/
                             ,ID3DBlob* gs_bytecode    /*= nullptr*/
                             ,ID3D11ComputeShader* cs  /*= nullptr*/
                             ,ID3DBlob* cs_bytecode    /*= nullptr*/
)
:_name(name)
,_device(device)
, _vs_bytecode(vs_bytecode)
, _hs_bytecode(hs_bytecode)
, _ds_bytecode(ds_bytecode)
, _gs_bytecode(gs_bytecode)
, _ps_bytecode(ps_bytecode)
, _cs_bytecode(cs_bytecode)
, _il(input_layout)
, _vs(vs)
, _hs(hs)
, _ds(ds)
, _gs(gs)
, _ps(ps)
, _cs(cs)
{
    /* DO NOTHING */
}

ShaderProgram::~ShaderProgram() {
    if( _vs_bytecode) {
        _vs_bytecode->Release();
        _vs_bytecode = nullptr;
    }
    if( _hs_bytecode) {
        _hs_bytecode->Release();
        _hs_bytecode = nullptr;
    }
    if( _ds_bytecode) {
        _ds_bytecode->Release();
        _ds_bytecode = nullptr;
    }
    if( _gs_bytecode) {
        _gs_bytecode->Release();
        _gs_bytecode = nullptr;
    }
    if(_ps_bytecode) {
        _ps_bytecode->Release();
        _ps_bytecode = nullptr;
    }
    if(_cs_bytecode) {
        _cs_bytecode->Release();
        _cs_bytecode = nullptr;
    }

    delete _il;
    _il = nullptr;

    if(_vs) {
        _vs->Release();
        _vs = nullptr;
    }
    if(_hs) {
        _hs->Release();
        _hs = nullptr;
    }
    if(_ds) {
        _ds->Release();
        _ds = nullptr;
    }
    if(_gs) {
        _gs->Release();
        _gs = nullptr;
    }
    if(_ps) {
        _ps->Release();
        _ps = nullptr;
    }
    if(_cs) {
        _cs->Release();
        _cs = nullptr;
    }
}

std::string ShaderProgram::GetName() const {
    return _name;
}

RHIDevice* ShaderProgram::GetParentDevice() {
    return _device;
}

ID3DBlob* ShaderProgram::GetVSByteCode() const {
    return _vs_bytecode;
}

ID3DBlob* ShaderProgram::GetHSByteCode() const {
    return _hs_bytecode;
}

ID3DBlob* ShaderProgram::GetDSByteCode() const {
    return _ds_bytecode;
}

ID3DBlob* ShaderProgram::GetGSByteCode() const {
    return _gs_bytecode;
}

ID3DBlob* ShaderProgram::GetPSByteCode() const {
    return _ps_bytecode;
}

ID3DBlob* ShaderProgram::GetCSByteCode() const {
    return _cs_bytecode;
}

InputLayout* ShaderProgram::GetInputLayout() const {
    return _il;
}

ID3D11VertexShader* ShaderProgram::GetVS() const {
    return _vs;
}

ID3D11HullShader* ShaderProgram::GetHS() const {
    return _hs;
}

ID3D11DomainShader* ShaderProgram::GetDS() const {
    return _ds;
}

ID3D11GeometryShader* ShaderProgram::GetGS() const {
    return _gs;
}

ID3D11PixelShader* ShaderProgram::GetPS() const {
    return _ps;
}

ID3D11ComputeShader* ShaderProgram::GetCS() const {
    return _cs;
}
