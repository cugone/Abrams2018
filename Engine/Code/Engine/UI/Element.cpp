#include "Engine/UI/Element.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Math/MathUtils.hpp"

#include "Engine/Renderer/Renderer.hpp"

#include "Engine/UI/Canvas.hpp"

#include <sstream>

namespace UI {

Element::Element()
    : _parent_canvas(nullptr)
{
    /* DO NOTHING */
}

Element::Element(UI::Canvas* parent_canvas)
    : _parent_canvas(parent_canvas)
{
    /* DO NOTHING */
}

Element::~Element() {
    RemoveSelf();
    DestroyAllChildren();
}

Element* Element::AddChild(Element* child) {
    _dirty_bounds = true;
    child->_parent = this;
    _children.push_back(child);
    CalcBoundsForMeThenMyChildren();
    return child;
}

void Element::RemoveChild(Element* child) {
    _dirty_bounds = true;
    _children.erase(
        std::remove_if(_children.begin(), _children.end(),
            [&child](UI::Element* c) {
                return child == c;
            }),
        _children.end());
    CalcBoundsForMeThenMyChildren();
}

void Element::RemoveAllChildren() {
    _dirty_bounds = true;
    _children.clear();
    _children.shrink_to_fit();
    CalcBounds();
}

void Element::RemoveSelf() {
    if(_parent) {
        _parent->RemoveChild(this);
        _parent = nullptr;
    }
}

void Element::DestroyChild(UI::Element*& child) {
    auto iter = std::find_if(std::begin(_children), std::end(_children), [child](UI::Element* c) { return child == c; });
    if(iter != std::end(_children)) {
        delete *iter;
        *iter = nullptr;
    }
}

void Element::DestroyAllChildren() {
    for(auto& iter : _children) {
        iter->_parent = nullptr;
        delete iter;
        iter = nullptr;
    }
    _children.clear();
    _children.shrink_to_fit();
}

void Element::SetBorderColor(const Rgba& color) {
    SetDebugColors(color, _fill_color, _pivot_color);
}

void Element::SetBackgroundColor(const Rgba& color) {
    SetDebugColors(_edge_color, color, _pivot_color);
}

void Element::SetPivotColor(const Rgba& color) {
    SetDebugColors(_edge_color, _fill_color, color);
}

void Element::SetDebugColors(const Rgba& edge, const Rgba& fill, const Rgba& pivot /*= Rgba::RED*/) {
    _edge_color = edge;
    _fill_color = fill;
    _pivot_color = pivot;
}

Vector2 Element::CalcLocalPosition() const {
    AABB2 local_bounds = GetParentBounds();
    return MathUtils::CalcPointFromNormalizedPoint(_position.ratio.GetValue(), local_bounds) + _position.unit;
}

Vector2 Element::CalcRelativePosition(const Vector2& position) const {
    AABB2 parent_bounds = GetParentLocalBounds();
    return MathUtils::CalcPointFromNormalizedPoint(position, parent_bounds);
}

Vector2 Element::CalcRelativePosition() const {
    AABB2 parent_bounds = GetParentLocalBounds();
    return MathUtils::CalcPointFromNormalizedPoint(_pivot.GetValue(), parent_bounds);
}

const UI::Metric& Element::GetPosition() const {
    return _position;
}

void Element::SetPosition(const Metric& position) {
    _dirty_bounds = true;
    _position = position;
    CalcBoundsForMeThenMyChildren();
}

void Element::SetPivot(const Vector2& pivotPosition) {
    _dirty_bounds = true;
    _pivot.SetValue(pivotPosition);
    CalcBoundsForMeThenMyChildren();
}

void Element::SetPivot(const PivotPosition& pivotPosition) {
    switch(pivotPosition) {
        case PivotPosition::Center:
            SetPivot(Vector2(0.0f, 0.0f));
            break;
        case PivotPosition::TopLeft:
            SetPivot(Vector2(-0.5f, -0.5f));
            break;
        case PivotPosition::Top:
            SetPivot(Vector2(0.0f, -0.5f));
            break;
        case PivotPosition::TopRight:
            SetPivot(Vector2(0.5f, -0.5f));
            break;
        case PivotPosition::Right:
            SetPivot(Vector2(0.5f, 0.0f));
            break;
        case PivotPosition::BottomRight:
            SetPivot(Vector2(0.5f, 0.5f));
            break;
        case PivotPosition::Bottom:
            SetPivot(Vector2(0.0f, 0.5f));
            break;
        case PivotPosition::BottomLeft:
            SetPivot(Vector2(-0.5f, 0.5f));
            break;
        case PivotPosition::Left:
            SetPivot(Vector2(-0.5f, 0.0f));
            break;
        default:
            std::ostringstream ss;
            ss << __FUNCTION__ << ": Unhandled pivot mode.";
            ERROR_AND_DIE(ss.str().c_str());
            break;
    }
}

const Vector2& Element::GetPivot() const {
    return _pivot.GetValue();
}

void Element::Update(float /*deltaSeconds*/) {
    /* DO NOTHING */
}

void Element::Render(Renderer* /*renderer*/) const {
    /* DO NOTHING */
}

void Element::DebugRender(Renderer* renderer) const {
    DebugRenderBoundsAndPivot(renderer);
}

Matrix4 Element::GetLocalTransform() const {
    auto t = Matrix4::CreateTranslationMatrix(CalcLocalPosition());
    auto r = Matrix4::Create2DRotationMatrix(CalcLocalRotationRadians());
    auto s = Matrix4::CreateScaleMatrix(CalcLocalScale());
    auto model = t * r * s;
    return model;
}

Vector2 Element::CalcLocalScale() const {
    auto my_bounds = CalcLocalBounds();
    auto parent_bounds = GetParentBounds();
    auto parent_width = parent_bounds.maxs.x - parent_bounds.mins.x;
    auto my_width = my_bounds.maxs.x - my_bounds.mins.x;
    auto width_scale = my_width / parent_width;
    auto parent_height = parent_bounds.maxs.y - parent_bounds.mins.y;
    auto my_height = my_bounds.maxs.y - my_bounds.mins.y;
    auto height_scale = my_height / parent_height;
    return Vector2(width_scale, height_scale);
}

Matrix4 Element::GetWorldTransform() const {
    return GetParentWorldTransform() * GetLocalTransform();
}

Matrix4 Element::GetParentWorldTransform() const {
    return _parent ? _parent->GetWorldTransform() : Matrix4::GetIdentity();
}

void Element::DebugRenderBoundsAndPivot(Renderer* renderer) const {
    DebugRenderBounds(renderer);
    DebugRenderPivot(renderer);
}

void Element::DebugRenderPivot(Renderer* renderer) const {
    auto world_transform = GetWorldTransform();
    Matrix4 pivot_mat = world_transform * Matrix4::CreateScaleMatrix(0.01f);
    renderer->SetModelMatrix(pivot_mat);
    renderer->DrawX2D(_pivot_color);
}

void Element::DebugRenderBounds(Renderer* renderer) const {
    Vector2 topLeft = Vector2(-0.50f, -0.50f);
    Vector2 topRight = Vector2(0.50f, -0.50f);
    Vector2 bottomRight = Vector2(0.50f, 0.50f);
    Vector2 bottomLeft = Vector2(-0.50f, 0.50f);
    std::vector<Vertex3D> vbo = {
      Vertex3D(Vector3(topLeft, 0.0f), _edge_color, Vector2::ZERO)
    , Vertex3D(Vector3(topRight, 0.0f), _edge_color, Vector2::ZERO)
    , Vertex3D(Vector3(bottomRight, 0.0f), _edge_color, Vector2::ZERO)
    , Vertex3D(Vector3(bottomLeft, 0.0f), _edge_color, Vector2::ZERO)
    };
    std::vector<unsigned int> ibo = {
        0, 1, 2, 3, 0
    };
    renderer->SetMaterial(renderer->GetMaterial("__2D"));
    auto world_transform = GetWorldTransform();
    renderer->SetModelMatrix(world_transform);
    renderer->DrawIndexed(PrimitiveType::LinesStrip, vbo, ibo);
}

AABB2 Element::GetParentBounds() const {
    return _parent ? _parent->_bounds : AABB2{ 0.0f, 0.0f, _size.unit.x, _size.unit.y };
}

void Element::CalcBounds() {
    switch(_mode) {
        case UI::PositionMode::Absolute:
            _bounds = CalcAbsoluteBounds();
            break;
        case UI::PositionMode::Relative:
            _bounds = CalcRelativeBounds();
            break;
        default:
        {
            std::ostringstream ss;
            ss << __FUNCTION__ << ": Unhandled positioning mode.";
            ERROR_AND_DIE(ss.str().c_str());
            break;
        }
    }
}

AABB2 Element::CalcBoundsRelativeToParent() {
    Vector2 my_size = GetSize();

    AABB2 parent_bounds = _parent ? _parent->CalcLocalBounds() : CalcLocalBounds();
    Vector2 parent_size = parent_bounds.CalcDimensions();

    Vector2 pivot_position = parent_bounds.mins + (parent_size * _position.ratio.GetValue() + _position.unit);

    AABB2 my_local_bounds = CalcLocalBounds();
    my_local_bounds.Translate(pivot_position);

    return my_local_bounds;
}

void Element::CalcBoundsForChildren() {
    for(auto& c : _children) {
        if(c) {
            c->CalcBounds();
        }
    }
}

void Element::CalcBoundsForMeThenMyChildren() {
    CalcBounds();
    CalcBoundsForChildren();
}

AABB2 Element::CalcRelativeBounds() {
    Vector2 size = GetSize();
    Vector2 pivot_position = size * _pivot.GetValue();

    AABB2 bounds;
    bounds.StretchToIncludePoint(Vector2::ZERO);
    bounds.StretchToIncludePoint(size);
    bounds.Translate(-pivot_position);
    return bounds;
}

AABB2 Element::CalcAbsoluteBounds() {
    Vector2 size = GetSize();

    Vector2 pivot = _pivot.GetValue();
    float pivot_x = pivot.x;
    float pivot_y = pivot.y;
    Vector2 mins{ { -(size.x * pivot_x) },{ -(size.y * pivot_y) } };
    Vector2 maxs{ { size.x * (1.0f - pivot_y) },{ size.y * (1.0f - pivot_y) } };
    AABB2 result(mins, maxs);
    return result;
}

AABB2 Element::AlignBoundsToContainer(AABB2 bounds, AABB2 container, const Vector2& alignment) noexcept {
    Vector2 max_distance = MathUtils::CalcPointFromNormalizedPoint(alignment, bounds);
    Vector2 distance = MathUtils::CalcPointFromNormalizedPoint(alignment, container) + max_distance;
    bounds.Translate(distance);
    return bounds;
}

AABB2 Element::CalcAlignedAbsoluteBounds() {
    AABB2 parent_bounds = GetParentLocalBounds();
    const auto& ratio = _position.ratio;
    const auto& ratio_v = ratio.GetValue();
    AABB2 alignedBounds = AlignBoundsToContainer(CalcBoundsRelativeToParent(), parent_bounds, ratio_v);

    auto unit = _position.unit;
    float normalized_ratio_x = MathUtils::RangeMap(ratio_v.x, 0.0f, 1.0f, 1.0f, -1.0f);
    float normalized_ratio_y = MathUtils::RangeMap(ratio_v.y, 0.0f, 1.0f, 1.0f, -1.0f);
    float scaled_x = normalized_ratio_x * unit.x;
    float scaled_y = normalized_ratio_y * unit.y;
    Vector2 offset(scaled_x, scaled_y);

    alignedBounds.Translate(offset);

    return alignedBounds;
}

AABB2 Element::CalcLocalBounds() const {
    return { Vector2::ZERO, GetSize() };
}

bool Element::IsDirty() const {
    return _dirty_bounds;
}

bool Element::IsParent() const {
    return _children.empty();
}

bool Element::IsChild() const {
    return !_parent;
}

UI::Canvas* Element::GetParentCanvas() const {
    return _parent_canvas;
}

void Element::SetParentCanvas(UI::Canvas* canvas) {
    _parent_canvas = canvas;
}

void Element::DebugRenderBottomUp(Renderer* renderer) const {
    DebugRenderBoundsAndPivot(renderer);
    DebugRenderChildren(renderer);
}

void Element::DebugRenderTopDown(Renderer* renderer) const {
    DebugRenderChildren(renderer);
    DebugRenderBoundsAndPivot(renderer);
}

void Element::DebugRenderChildren(Renderer* renderer) const {
    for(auto& child : _children) {
        if(child) {
            child->DebugRender(renderer);
        }
    }
}

AABB2 Element::GetParentLocalBounds() const {
    return _parent ? _parent->CalcLocalBounds() : AABB2( Vector2::ZERO , _size.unit );
}

AABB2 Element::GetParentRelativeBounds() const {
    return _parent ? _parent->CalcBoundsRelativeToParent() : AABB2{ 0.0f, 0.0f, 0.0f, 0.0f };
}

void Element::UpdateChildren(float deltaSeconds) {
    for(auto& child : _children) {
        if(child) {
            child->Update(deltaSeconds);
        }
    }
}

void Element::RenderChildren(Renderer* renderer) const {
    for(auto& child : _children) {
        if(child) {
            child->Render(renderer);
        }
    }
}

AABB2 Element::GetBounds(const AABB2& parent, const Vector4& anchors, const Vector4& offsets) const noexcept {
    Vector2 boundMins = MathUtils::CalcPointFromNormalizedPoint(Vector2(anchors.x, anchors.y), parent) + Vector2(offsets.x, offsets.y);
    Vector2 boundMaxs = MathUtils::CalcPointFromNormalizedPoint(Vector2(anchors.z, anchors.w), parent) + Vector2(offsets.z, offsets.w);
    return AABB2(boundMins, boundMaxs);
}

Vector2 Element::GetSmallestOffset(AABB2 a, AABB2 b) const noexcept {
    auto width = a.CalcDimensions().x;
    auto height = a.CalcDimensions().y;
    auto center = a.CalcCenter();
    b.AddPaddingToSides(-(width * 0.5f), -(height * 0.5f));
    Vector2 closestPoint = MathUtils::CalcClosestPoint(center, b);
    return closestPoint - center;
}

AABB2 Element::MoveToBestFit(const AABB2& obj, const AABB2& container) const noexcept {
    Vector2 offset = GetSmallestOffset(obj, container);
    return obj + offset;
}

float Element::GetAspectRatio() const noexcept {
    auto dims = _bounds.CalcDimensions();
    return dims.x / dims.y;
}

Vector2 Element::GetTopLeft() const noexcept {
    return _bounds.mins;
}

Vector2 Element::GetTopRight() const noexcept {
    return Vector2{_bounds.maxs.x, _bounds.mins.y };
}

Vector2 Element::GetBottomLeft() const noexcept {
    return Vector2{ _bounds.mins.x, _bounds.maxs.y };
}

Vector2 Element::GetBottomRight() const noexcept {
    return _bounds.maxs;
}

float Element::GetParentOrientationRadians() const {
    return _parent ? _parent->GetOrientationRadians() : 0.0f;
}

float Element::GetParentOrientationDegrees() const {
    return _parent ? _parent->GetOrientationDegrees() : 0.0f;
}

void Element::SetOrientationDegrees(float value) {
    _orientationRadians = MathUtils::ConvertDegreesToRadians(value);
}

void Element::SetOrientationRadians(float value) {
    _orientationRadians = value;
}

float Element::GetOrientationDegrees() const {
    return MathUtils::ConvertRadiansToDegrees(GetOrientationRadians());
}

float Element::GetOrientationRadians() const {
    return _orientationRadians;
}

float Element::CalcLocalRotationDegrees() const {
    return MathUtils::ConvertRadiansToDegrees(GetOrientationDegrees());
}

float Element::CalcLocalRotationRadians() const {
    return GetOrientationRadians();
}

float Element::CalcWorldRotationRadians() const {
    return GetParentOrientationRadians() + GetOrientationRadians();
}

float Element::CalcWorldRotationDegrees() const {
    return GetParentOrientationDegrees() + GetOrientationDegrees();
}

Vector2 Element::GetSize() const {
    return _parent ? (_parent->GetSize() * _size.ratio.GetValue() + _size.unit) : _size.unit;
}

float Element::GetInvAspectRatio() const noexcept {
    return 1.0f / GetAspectRatio();
}

void Element::SetSize(const Metric& size) {
    _dirty_bounds = true;
    _size = size;
    CalcBoundsForMeThenMyChildren();
}

} //End UI