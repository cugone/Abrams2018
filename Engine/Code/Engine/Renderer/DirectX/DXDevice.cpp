#include "Engine/Renderer/DirectX/DXDevice.hpp"

#include "Engine/Core/BuildConfig.cpp"
#include "Engine/Renderer/Window.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/RHI/RHIDeviceContext.hpp"
#include "Engine/RHI/RHIOutput.hpp"

#include "Engine/Renderer/DirectX/DXOutput.hpp"
#include "Engine/Renderer/DirectX/DXDeviceContext.hpp"
#include "Engine/Renderer/DirectX/DXDepthStencilState.hpp"

DXDevice::DXDevice()
    : RHIDevice()
    , _dx_device(nullptr)
{
    deviceType = RHIDeviceType::DIRECTX;
}

DXDevice::~DXDevice() {
    delete _highestSupportedFeatureLevel;
    _highestSupportedFeatureLevel = nullptr;
    _dx_device->Release();
    _dx_device = nullptr;
}

RHIOutput* DXDevice::CreateOutput(Window* window, const RHIOutputMode& outputMode /*= RHIOutputMode::WINDOWED*/) {
    if(!window) {
        ERROR_AND_DIE("DXDevice: Invalid Window!");
    }
    window->SetDisplayMode(outputMode);
    return CreateOutputFromWindow(window);
}

RHIOutput* DXDevice::CreateOutput(const IntVector2& clientSize, const IntVector2& clientPosition /*= IntVector2::ZERO*/, const RHIOutputMode& outputMode /*= RHIOutputMode::WINDOWED*/) {
    Window* window = new Window;
    window->SetClientSize(clientPosition, clientSize);
    window->SetDisplayMode(outputMode);
    return CreateOutputFromWindow(window);
}

bool DXDevice::CreateDepthStencilState(DepthStencilState* state, bool enableDepthTest /*= true */, bool enableStencilTest /*= false */, bool enableDepthWrite /*= true */, bool enableStencilRead /*= true */, bool enableStencilWrite /*= true */, const ComparisonFunction& depthComparison /*= ComparisonFunction::LESS */, std::pair<const StencilOperation&, const StencilOperation&> failFrontBackOp /*= std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP) */, std::pair<const StencilOperation&, const StencilOperation&> failDepthFrontBackOp /*= std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP) */, std::pair<const StencilOperation&, const StencilOperation&> passFrontBackOp /*= std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP) */, std::pair<const ComparisonFunction&, const ComparisonFunction&> stencilComparisonFrontBack /*= std::make_pair(ComparisonFunction::ALWAYS, ComparisonFunction::ALWAYS) */) {

    D3D11_DEPTH_STENCIL_DESC dsDesc;

    // Depth test parameters
    dsDesc.DepthEnable = enableDepthTest;
    dsDesc.DepthWriteMask = enableDepthWrite ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = ComparisonFunctionToD3DComparisonFunction(depthComparison);

    // Stencil test parameters
    dsDesc.StencilEnable = enableStencilTest;
    dsDesc.StencilReadMask = enableStencilRead ? 0xFF : 0x00;
    dsDesc.StencilWriteMask = enableStencilWrite ? 0xFF : 0x00;

    // Stencil operations if pixel is front-facing
    dsDesc.FrontFace.StencilFailOp = StencilOperationToD3DStencilOperation(failFrontBackOp.first);
    dsDesc.FrontFace.StencilDepthFailOp = StencilOperationToD3DStencilOperation(failDepthFrontBackOp.first);
    dsDesc.FrontFace.StencilPassOp = StencilOperationToD3DStencilOperation(passFrontBackOp.first);
    dsDesc.FrontFace.StencilFunc = ComparisonFunctionToD3DComparisonFunction(stencilComparisonFrontBack.first);

    // Stencil operations if pixel is back-facing
    dsDesc.BackFace.StencilFailOp = StencilOperationToD3DStencilOperation(failFrontBackOp.second);
    dsDesc.BackFace.StencilDepthFailOp = StencilOperationToD3DStencilOperation(failDepthFrontBackOp.second);
    dsDesc.BackFace.StencilPassOp = StencilOperationToD3DStencilOperation(passFrontBackOp.second);
    dsDesc.BackFace.StencilFunc = ComparisonFunctionToD3DComparisonFunction(stencilComparisonFrontBack.second);

    // Create depth stencil state
    DXDepthStencilState* dx_state = dynamic_cast<DXDepthStencilState*>(state);
    ID3D11DepthStencilState* dx_depthstencilstate = nullptr;
    HRESULT state_hr = _dx_device->CreateDepthStencilState(&dsDesc, &dx_depthstencilstate);
    bool succeeded = SUCCEEDED(state_hr);
    if(succeeded) {
        dx_state->SetDxDepthStencilState(dx_depthstencilstate);
    }
    return succeeded;
}

D3D_FEATURE_LEVEL* DXDevice::GetFeatureLevel() const {
    return _highestSupportedFeatureLevel;
}

ID3D11Device* DXDevice::GetDxDevice() {
    return _dx_device;
}

RHIOutput* DXDevice::CreateOutputFromWindow(Window*& window) {

    if(window == nullptr) {
        ERROR_AND_DIE("DXDevice: Invalid Window!");
    }

    window->Open();

    bool is_windowed = window->IsWindowed();
    unsigned int device_flags = 0U;
#ifdef RENDER_DEBUG
    device_flags |= D3D11_CREATE_DEVICE_DEBUG;
    device_flags |= D3D11_CREATE_DEVICE_DEBUGGABLE;
#endif

    unsigned int d3d11_1featureSize = 7;
    D3D_FEATURE_LEVEL d3d11_1features[7] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1,
    };

    ID3D11Device* dx_device = nullptr;
    ID3D11DeviceContext* dx_context = nullptr;
    D3D_FEATURE_LEVEL supported_feature_level;
    auto hr_device = ::D3D11CreateDevice(nullptr
                                         , D3D_DRIVER_TYPE_HARDWARE
                                         , nullptr
                                         , device_flags
                                         , d3d11_1features
                                         , d3d11_1featureSize
                                         , D3D11_SDK_VERSION
                                         , &dx_device
                                         , &supported_feature_level
                                         , &dx_context
    );

    bool d3d11_1_succeeded = SUCCEEDED(hr_device);
    if(!d3d11_1_succeeded) {

        unsigned int d3d11_0featureSize = 6;
        D3D_FEATURE_LEVEL d3d11_0features[6] = {
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_9_2,
            D3D_FEATURE_LEVEL_9_1,
        };
        device_flags = 0U;
#ifdef RENDER_DEBUG
        device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
        hr_device = ::D3D11CreateDevice(nullptr
                                        , D3D_DRIVER_TYPE_HARDWARE
                                        , nullptr
                                        , device_flags
                                        , d3d11_0features
                                        , d3d11_0featureSize
                                        , D3D11_SDK_VERSION
                                        , &dx_device
                                        , &supported_feature_level
                                        , &dx_context
        );
    }

    _highestSupportedFeatureLevel = new D3D_FEATURE_LEVEL;
    *_highestSupportedFeatureLevel = supported_feature_level;

    _immediate_context = new DXDeviceContext(this, dx_context);

    IDXGIDevice4* dxgi_device = nullptr;
    auto hr_dxgi_device = dx_device->QueryInterface(__uuidof(IDXGIDevice4), (void **)&dxgi_device);
    if(FAILED(hr_dxgi_device)) {
        delete _immediate_context;
        _immediate_context = nullptr;
        delete _highestSupportedFeatureLevel;
        _highestSupportedFeatureLevel = nullptr;
        ERROR_AND_DIE("Could not query IDXGIDevice.");
    }

    IDXGIAdapter4* dxgi_adapter = nullptr;
    auto hr_dxgi_adapter = dxgi_device->GetAdapter((IDXGIAdapter**)&dxgi_adapter);
    if(FAILED(hr_dxgi_adapter)) {
        dxgi_device->Release();
        dxgi_device = nullptr;
        delete _immediate_context;
        _immediate_context = nullptr;
        delete _highestSupportedFeatureLevel;
        _highestSupportedFeatureLevel = nullptr;
        ERROR_AND_DIE("Could not query IDXGIAdapter.");
    }

    IDXGIFactory5* dxgi_factory = nullptr;
    auto hr_dxgi_factory = dxgi_adapter->GetParent(__uuidof(IDXGIFactory5), (void **)&dxgi_factory);
    if(FAILED(hr_dxgi_factory)) {
        dxgi_adapter->Release();
        dxgi_adapter = nullptr;
        dxgi_device->Release();
        dxgi_device = nullptr;
        delete _immediate_context;
        _immediate_context = nullptr;
        delete _highestSupportedFeatureLevel;
        _highestSupportedFeatureLevel = nullptr;
        ERROR_AND_DIE("Could not query IDXGIFactory.");
    }

    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc1;
    std::memset(&swap_chain_desc1, 0, sizeof(swap_chain_desc1));
    swap_chain_desc1.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
    swap_chain_desc1.BufferCount = 2;
    swap_chain_desc1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER;
    swap_chain_desc1.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | (is_windowed ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : DXGI_SWAP_CHAIN_FLAG_NONPREROTATED);
    swap_chain_desc1.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_desc1.Height = window->GetClientSize().y;
    swap_chain_desc1.SampleDesc.Count = 1;
    swap_chain_desc1.SampleDesc.Quality = 0;
    swap_chain_desc1.Scaling = DXGI_SCALING_NONE;
    swap_chain_desc1.Stereo = false;
    swap_chain_desc1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    swap_chain_desc1.Width = window->GetClientSize().x;

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreen_desc;
    std::memset(&fullscreen_desc, 0, sizeof(fullscreen_desc));
    fullscreen_desc.Windowed = is_windowed ? TRUE : FALSE;
    fullscreen_desc.RefreshRate.Numerator = 1;
    fullscreen_desc.RefreshRate.Denominator = 60;
    fullscreen_desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    fullscreen_desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

    IDXGISwapChain1* dxgi_swapchain;
    auto hr_swapchain = dxgi_factory->CreateSwapChainForHwnd(dx_device
                                                             , window->GetWindowHandle()
                                                             , &swap_chain_desc1
                                                             , &fullscreen_desc
                                                             , nullptr
                                                             , &dxgi_swapchain);

    bool swapchain_create_succeeded = SUCCEEDED(hr_swapchain);
    if(!swapchain_create_succeeded) {
        dx_device->Release();
        dx_device = nullptr;
        dxgi_factory->Release();
        dxgi_factory = nullptr;
        dxgi_adapter->Release();
        dxgi_adapter = nullptr;
        dxgi_device->Release();
        dxgi_device = nullptr;
        delete _immediate_context;
        _immediate_context = nullptr;
        delete window;
        window = nullptr;
        ERROR_AND_DIE("Could not create swap chain.");
    }

    dxgi_factory->Release();
    dxgi_factory = nullptr;

    dxgi_adapter->Release();
    dxgi_adapter = nullptr;

    dxgi_device->Release();
    dxgi_device = nullptr;

    _dx_device = dx_device;
    RHIOutput* rhi_output = new DXOutput(this, window, dxgi_swapchain);
    return rhi_output;
}
