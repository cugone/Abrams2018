#include "Engine/Renderer/DirectX/DXInstance.hpp"

#include "Engine/Core/BuildConfig.cpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Renderer/DirectX/DX11.hpp"

#include "Engine/Renderer/DirectX/DXDevice.hpp"
#include "Engine/Renderer/DirectX/DXDeviceContext.hpp"
#include "Engine/Renderer/DirectX/DXOutput.hpp"

#include "Engine/Math/IntVector2.hpp"

#include "Engine/Renderer/Window.hpp"

IDXGIDebug* DXInstance::_debuggerInstance = nullptr;

DXInstance::DXInstance()
    : RHIInstance()
{

    _instanceType = RHIInstanceType::DIRECTX;

#if defined(RENDER_DEBUG)

    HMODULE debug_module = nullptr;

    // Debug Setup

    debug_module = ::LoadLibraryA("Dxgidebug.dll");

    using GetDebugModuleCB = HRESULT(WINAPI *)(REFIID, void**);

    GetDebugModuleCB cb = (GetDebugModuleCB) ::GetProcAddress(debug_module, "DXGIGetDebugInterface");

    HRESULT hr = cb(__uuidof(IDXGIDebug), (void**)&_debuggerInstance);

    bool succeeded = SUCCEEDED(hr);
    //ASSERT_OR_DIE(succeeded, "DXGIDugger failed to initialize.");
    if(succeeded) {
        _debuggerInstance->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
    } else {
        _debuggerInstance = nullptr;
    }

#endif
}

DXInstance::~DXInstance() {
#ifdef RENDER_DEBUG
    if(_debuggerInstance) {
        _debuggerInstance->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
        _debuggerInstance->Release();
        _debuggerInstance = nullptr;
    }
#endif
}

RHIOutput* DXInstance::CreateOutput(const IntVector2& client_size, const IntVector2& client_position, const RHIOutputMode& output_mode) {

    RHIDevice* rhi_device = nullptr;
    RHIOutput* rhi_output = nullptr;
    RHIDeviceContext* rhi_context = nullptr;
    try {
        rhi_device = new DXDevice();
        rhi_output = rhi_device->CreateOutput(client_size, client_position, output_mode);
        return rhi_output;
    } catch(...) {
        delete rhi_device;
        rhi_device = nullptr;
        
        delete rhi_output;
        rhi_output = nullptr;
        
        delete rhi_context;
        rhi_context = nullptr;

    }
    return nullptr;
}
