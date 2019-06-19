#include "Engine/Renderer/RenderTargetStack.hpp"

#include "Engine/Renderer/DepthStencilState.hpp"
#include "Engine/Renderer/Renderer.hpp"

RenderTargetStack::RenderTargetStack(Renderer* renderer) noexcept
    : _renderer(renderer)
{
    /* DO NOTHING */
}

void RenderTargetStack::Push(const RenderTargetStack::Node& node) noexcept {
    _stack.push_back(node);
    const auto& top = _stack.back();
    _renderer->SetRenderTarget(top.color_target, top.depthstencil_target);
    auto x = static_cast<unsigned int>(top.view_desc.x);
    auto y = static_cast<unsigned int>(top.view_desc.y);
    auto w = static_cast<unsigned int>(top.view_desc.width);
    auto h = static_cast<unsigned int>(top.view_desc.height);
    _renderer->SetViewport(x, y, w, h);
}

void RenderTargetStack::Pop() noexcept {
    _stack.pop_back();
    const auto& top = _stack.back();
    _renderer->SetRenderTarget(top.color_target, top.depthstencil_target);
    _renderer->ClearColor(Rgba::Black);
    _renderer->ClearDepthStencilBuffer();
    auto x = static_cast<unsigned int>(top.view_desc.x);
    auto y = static_cast<unsigned int>(top.view_desc.y);
    auto w = static_cast<unsigned int>(top.view_desc.width);
    auto h = static_cast<unsigned int>(top.view_desc.height);
    _renderer->SetViewport(x, y, w, h);
}

const RenderTargetStack::Node& RenderTargetStack::Top() const noexcept {
    return _stack.back();
}
