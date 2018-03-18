#include "Engine/Renderer/DirectX/DXOutput.hpp"

#include "Engine/Renderer/Window.hpp"
#include "Engine/Renderer/DirectX/DX11.hpp"
#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"

DXOutput::DXOutput(RHIDevice* parent, Window* wnd, IDXGISwapChain1* swap_chain)
    : RHIOutput(parent, wnd)
    , _dx_swapchain(swap_chain)
{
    CreateBackbuffer();
}

DXOutput::~DXOutput() {
    _dx_swapchain->Release();
    _dx_swapchain = nullptr;
}

void DXOutput::Present(bool vsync) {
    unsigned int flags = _window->IsWindowed() ? DXGI_PRESENT_ALLOW_TEARING : 0;
    _dx_swapchain->Present1(vsync ? 1 : 0, flags, nullptr);
}

bool DXOutput::SetDisplayMode(const RHIOutputMode& newMode) {
    _window->SetDisplayMode(newMode);
    auto width = _window->GetClientSize().x;
    auto height = _window->GetClientSize().y;
    DXGI_SWAP_CHAIN_DESC desc;
    _dx_swapchain->GetDesc(&desc);
    _parentDevice->GetImmediateContext()->Flush();
    _parentDevice->GetImmediateContext()->ClearState();
    //delete _back_buffer;
    //_back_buffer = nullptr;
    _dx_swapchain->ResizeBuffers(desc.BufferCount, width, height, DXGI_FORMAT_UNKNOWN, 0);
    CreateBackbuffer();
    return true;
}

void DXOutput::CreateBackbuffer() {
    //if(_back_buffer != nullptr) {
    //    delete _back_buffer;
    //}
    ID3D11Texture2D* back_buffer = nullptr;
    _dx_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&back_buffer));
    //_back_buffer = new Texture2D(_parentDevice, back_buffer);
}

void DXOutput::ResetBackbuffer() {
    //delete _back_buffer;
    //_back_buffer = nullptr;
    CreateBackbuffer();
}
