#include "Engine/Renderer/DepthStencilState.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Renderer/DirectX/DX11.hpp"

#include "Engine/RHI/RHIDevice.hpp"

DepthStencilState::DepthStencilState(RHIDevice* device
                                     , bool enableDepthTest /*= true */
                                     , bool enableStencilTest /*= false */
                                     , bool enableDepthWrite /*= true */
                                     , bool enableStencilRead /*= true */
                                     , bool enableStencilWrite /*= true */
                                     , const ComparisonFunction& depthComparison /*= ComparisonFunction::LESS */
                                     , std::pair<const StencilOperation&, const StencilOperation&> failFrontBackOp /*= std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP) */
                                     , std::pair<const StencilOperation&, const StencilOperation&> failDepthFrontBackOp /*= std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP) */
                                     , std::pair<const StencilOperation&, const StencilOperation&> passFrontBackOp /*= std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP) */
                                     , std::pair<const ComparisonFunction&, const ComparisonFunction&> stencilComparisonFrontBack /*= std::make_pair(ComparisonFunction::ALWAYS, ComparisonFunction::ALWAYS) */)
: _enableDepthTest(enableDepthTest)
, _enableStencilTest(enableStencilTest)
, _enableDepthWrite(enableDepthWrite)
, _enableStencilRead(enableStencilRead)
, _enableStencilWrite(enableStencilWrite)
, _depthComparison(depthComparison)
, _failFrontBackOp(failFrontBackOp)
, _failDepthFrontBackOp(failDepthFrontBackOp)
, _passFrontBackOp(passFrontBackOp)
, _stencilComparisonFrontBack(stencilComparisonFrontBack)
{
    bool succeeded = CreateDepthStencilState(device
                                             ,_enableDepthTest
                                             , _enableStencilTest
                                             , _enableDepthWrite
                                             , _enableStencilRead
                                             , _enableStencilWrite
                                             , _depthComparison
                                             , _failFrontBackOp
                                             , _failDepthFrontBackOp
                                             , _passFrontBackOp
                                             , _stencilComparisonFrontBack);
    if(!succeeded) {
        _dx_state->Release();
        _dx_state = nullptr;
        ERROR_AND_DIE("DepthStencilState failed to create.");
    }
}

DepthStencilState::DepthStencilState(RHIDevice* device, const XMLElement& element)
{
    if(!LoadFromXml(device, element)) {
        _dx_state->Release();
        _dx_state = nullptr;
        ERROR_AND_DIE("DepthStencilState: LoadFromXml failed.");
    }
}

DepthStencilState::~DepthStencilState() {
    _dx_state->Release();
    _dx_state = nullptr;
}

ID3D11DepthStencilState* DepthStencilState::GetDxDepthStencilState() const {
    return _dx_state;
}

bool DepthStencilState::CreateDepthStencilState(RHIDevice* device, bool enableDepthTest /*= true */, bool enableStencilTest /*= false */, bool enableDepthWrite /*= true */, bool enableStencilRead /*= true */, bool enableStencilWrite /*= true */, const ComparisonFunction& depthComparison /*= ComparisonFunction::LESS */, std::pair<const StencilOperation&, const StencilOperation&> failFrontBackOp /*= std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP) */, std::pair<const StencilOperation&, const StencilOperation&> failDepthFrontBackOp /*= std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP) */, std::pair<const StencilOperation&, const StencilOperation&> passFrontBackOp /*= std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP) */, std::pair<const ComparisonFunction&, const ComparisonFunction&> stencilComparisonFrontBack /*= std::make_pair(ComparisonFunction::ALWAYS, ComparisonFunction::ALWAYS)*/) {
    D3D11_DEPTH_STENCIL_DESC desc;
    desc.DepthEnable = enableDepthTest ? TRUE : FALSE;
    desc.DepthWriteMask = enableDepthWrite ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
    desc.StencilEnable = enableStencilTest ? TRUE : FALSE;
    desc.StencilReadMask = enableStencilRead ? D3D11_DEFAULT_STENCIL_READ_MASK :  0x00;
    desc.StencilWriteMask = enableStencilWrite ? D3D11_DEFAULT_STENCIL_WRITE_MASK : 0x00;
    desc.DepthFunc = ComparisonFunctionToD3DComparisonFunction(depthComparison);

    desc.FrontFace.StencilFailOp = StencilOperationToD3DStencilOperation(failFrontBackOp.first);
    desc.FrontFace.StencilDepthFailOp = StencilOperationToD3DStencilOperation(failDepthFrontBackOp.first);
    desc.FrontFace.StencilFunc = ComparisonFunctionToD3DComparisonFunction(stencilComparisonFrontBack.first);
    desc.FrontFace.StencilPassOp = StencilOperationToD3DStencilOperation(passFrontBackOp.first);

    desc.BackFace.StencilFailOp = StencilOperationToD3DStencilOperation(failFrontBackOp.second);
    desc.BackFace.StencilDepthFailOp = StencilOperationToD3DStencilOperation(failDepthFrontBackOp.second);
    desc.BackFace.StencilFunc = ComparisonFunctionToD3DComparisonFunction(stencilComparisonFrontBack.second);
    desc.BackFace.StencilPassOp = StencilOperationToD3DStencilOperation(passFrontBackOp.second);

    HRESULT hr = device->GetDxDevice()->CreateDepthStencilState(&desc, &_dx_state);
    return SUCCEEDED(hr);
}

bool DepthStencilState::LoadFromXml(RHIDevice* device, const XMLElement& element) {
    //<shader name = "__2D">
    //    <shaderprogram src = "__unlit" />
    //    <raster>
    //        <fill>solid</fill>
    //        <cull>none</cull>
    //        <antialiasing>true</antialiasing>
    //    </raster>
    //        <blends>
    //            <blend enable = "true">
    //            <color src = "src_alpha" dest = "inv_src_alpha" op = "add" />
    //        </blend>
    //    </blends>
    //    <depth enable = "false" writable = "false" />
    //    <stencil enable = "false" readable = "false" writable = "false" />
    //    </shader>
    //Default values if no depth stencil element exists.
    bool enableDepthTest = true;
    bool depthWritable = true;
    ComparisonFunction depthComparison = ComparisonFunction::LESS;

    auto xml_depth = element.FirstChildElement("depth");
    if(xml_depth) {
        DataUtils::ValidateXmlElement(*xml_depth, "depth", "", "", "", "enable,writable,test");
        enableDepthTest = DataUtils::ParseXmlAttribute(*xml_depth, "enable", enableDepthTest);
        depthWritable = DataUtils::ParseXmlAttribute(*xml_depth, "writable", depthWritable);
        std::string comp_func_str = "less";
        comp_func_str = DataUtils::ParseXmlAttribute(*xml_depth, "test", comp_func_str);
        depthComparison = ComparisonFunctionFromString(comp_func_str);
    }

    //Default values if no stencil element exists.
    bool enableStencilTest = false;
    bool enableStencilRead = true;
    bool enableStencilWrite = true;
    std::pair<StencilOperation, StencilOperation> failFBOp = std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP);
    std::pair<StencilOperation, StencilOperation> failDepthFBOp = std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP);
    std::pair<StencilOperation, StencilOperation> passFBOp = std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP);
    std::pair<ComparisonFunction, ComparisonFunction> stencilComparisonFB = std::make_pair(ComparisonFunction::ALWAYS, ComparisonFunction::ALWAYS);

    auto xml_stencil = element.FirstChildElement("stencil");
    if(xml_stencil) {
        DataUtils::ValidateXmlElement(*xml_stencil, "stencil", "", "", "front,back", "enable,writable,readable");

        enableStencilRead = DataUtils::ParseXmlAttribute(*xml_stencil, "readable", enableStencilRead);
        enableStencilWrite = DataUtils::ParseXmlAttribute(*xml_stencil, "writable", enableStencilWrite);
        enableStencilTest = DataUtils::ParseXmlAttribute(*xml_stencil, "enable", enableStencilTest);

        auto xml_stencilfront = xml_stencil->FirstChildElement("front");
        if(xml_stencilfront) {
            DataUtils::ValidateXmlElement(*xml_stencilfront, "front", "", "fail,depthfail,pass,test");

            std::string failFront_str = "keep";
            failFront_str = DataUtils::ParseXmlAttribute(*xml_stencilfront, "fail", failFront_str);
            failFBOp.first = StencilOperationFromString(failFront_str);

            std::string depthfailFront_str = "keep";
            depthfailFront_str = DataUtils::ParseXmlAttribute(*xml_stencilfront, "depthfail", depthfailFront_str);
            failDepthFBOp.first = StencilOperationFromString(depthfailFront_str);

            std::string passFront_str = "keep";
            passFront_str = DataUtils::ParseXmlAttribute(*xml_stencilfront, "pass", passFront_str);
            passFBOp.first = StencilOperationFromString(passFront_str);

            std::string compareFront_str = "always";
            compareFront_str = DataUtils::ParseXmlAttribute(*xml_stencilfront, "test", compareFront_str);
            stencilComparisonFB.first = ComparisonFunctionFromString(compareFront_str);
        }

        auto xml_stencilback = xml_stencil->FirstChildElement("back");
        if(xml_stencilback) {
            DataUtils::ValidateXmlElement(*xml_stencilback, "back", "", "fail,depthfail,pass,test");

            std::string failBack_str = "keep";
            failBack_str = DataUtils::ParseXmlAttribute(*xml_stencilback, "fail", failBack_str);
            failFBOp.second = StencilOperationFromString(failBack_str);

            std::string depthfailBack_str = "keep";
            depthfailBack_str = DataUtils::ParseXmlAttribute(*xml_stencilback, "depthfail", depthfailBack_str);
            failDepthFBOp.second = StencilOperationFromString(depthfailBack_str);

            std::string passBack_str = "keep";
            passBack_str = DataUtils::ParseXmlAttribute(*xml_stencilback, "pass", passBack_str);
            passFBOp.second = StencilOperationFromString(passBack_str);

            std::string compareBack_str = "always";
            compareBack_str = DataUtils::ParseXmlAttribute(*xml_stencilback, "test", compareBack_str);
            stencilComparisonFB.second = ComparisonFunctionFromString(compareBack_str);
        }
    }

    return CreateDepthStencilState(device,
                                   enableDepthTest,
                                   enableStencilTest,
                                   depthWritable,
                                   enableStencilRead,
                                   enableStencilWrite,
                                   depthComparison,
                                   failFBOp,
                                   failDepthFBOp,
                                   passFBOp,
                                   stencilComparisonFB);

}
