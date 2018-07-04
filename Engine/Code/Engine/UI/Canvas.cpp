#include "Engine/UI/Canvas.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Renderer/Camera2D.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Texture2D.hpp"

#include "Engine/RHI/RHIOutput.hpp"

#include <sstream>

namespace UI {

Canvas::Canvas(Renderer& renderer, Texture* target_texture, float reference_resolution)
    : Element()
    , _renderer(&renderer)
    , _target_texture(target_texture)
    , _reference_resolution(reference_resolution)
{
    if(!_target_texture) {
        _target_texture = _renderer->GetOutput()->GetBackBuffer();
    }
    {
    std::ostringstream ss;
    ss << __FUNCTION__ << ": reference resolution must not be zero.";
    GUARANTEE_OR_DIE(!MathUtils::IsEquivalent(_reference_resolution, 0.0f), ss.str().c_str());
    }

    Vector2 dimensions{};
    CalcDimensionsAndAspectRatio(dimensions, _aspect_ratio);
    Metric m;
    m.ratio = Ratio(Vector2::ZERO);
    m.unit = dimensions;
    SetSize(m);

    SetParentCanvas(*this);
    _camera = new Camera2D;
}

Canvas::~Canvas() {
    _target_texture = nullptr;
}

void Canvas::Update(float deltaSeconds) {
    UpdateChildren(deltaSeconds);
}

void Canvas::Render(Renderer* renderer) const {
    renderer->SetRenderTarget(_target_texture);
    auto texture_dims = _target_texture->GetDimensions();
    auto target_dims = Vector2((float)texture_dims.x, (float)texture_dims.y);
    Vector2 leftBottom = Vector2(-0.5f, 0.5f);
    Vector2 rightTop = Vector2(0.5f, -0.5f);
    Vector2 nearFar{ 0.0f, 1.0f };
    _camera->SetupView(leftBottom, rightTop, nearFar, _aspect_ratio);
    auto scale = target_dims * 0.5f;
    Matrix4 s = Matrix4::CreateScaleMatrix(scale);
    renderer->SetModelMatrix(s);
    renderer->SetViewMatrix(_camera->GetViewMatrix());
    renderer->SetProjectionMatrix(_camera->GetProjectionMatrix());
    RenderChildren(renderer);
}

void Canvas::DebugRender(Renderer* renderer) const {
    DebugRenderChildren(renderer);
    DebugRenderBoundsAndPivot(renderer);
}

void Canvas::CalcDimensionsAndAspectRatio(Vector2& dimensions, float& aspectRatio) {
    if(!_target_texture) {
        _target_texture = _renderer->GetOutput()->GetBackBuffer();
    }
    auto texture_dims = _target_texture->GetDimensions();
    Vector2 target_dims = Vector2((float)texture_dims.x, (float)texture_dims.y);

    auto target_AR = target_dims.x / target_dims.y;
    Vector2 dims = Vector2::ZERO;
    if(target_AR <= 1.0f) {
        dims.x = _reference_resolution;
        dims.y = target_AR * _reference_resolution;
    } else {
        dims.x = target_AR * _reference_resolution;
        dims.y = _reference_resolution;
    }
    aspectRatio = dims.x / dims.y;
    dimensions = dims;
}


void Canvas::SetTargetTexture(Renderer& renderer, Texture* target) {
    _renderer = &renderer;
    if(!target) {
        target = _renderer->GetOutput()->GetBackBuffer();
    }
    _target_texture = target;

    Vector2 dimensions{};
    CalcDimensionsAndAspectRatio(dimensions, _aspect_ratio);
    Metric m;
    m.ratio = Ratio(Vector2::ZERO);
    m.unit = dimensions;
    SetSize(m);
}

} //End UI