#pragma once

#include "Engine/Core/Rgba.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Matrix4.hpp"
#include "Engine/Math/Vector2.hpp"

#include "Engine/UI/Types.hpp"

class Renderer;

namespace UI {

class Canvas;

class Element {
public:
    Element();
    explicit Element(UI::Canvas& parent_canvas);
    virtual ~Element();

    virtual void Update(float deltaSeconds);
    virtual void Render(Renderer* renderer) const;
    virtual void DebugRender(Renderer* renderer) const;

    template<typename T>
    T* CreateChild();

    template<typename T>
    T* CreateChild(UI::Canvas* parentCanvas);

    UI::Element* AddChild(UI::Element* child);
    void RemoveChild(Element* child);
    void RemoveAllChildren();
    void RemoveSelf();

    void DestroyChild(UI::Element*& child);
    void DestroyAllChildren();
protected:

    const UI::Metric& GetPosition() const;
    void SetPosition(const Metric& position);

    Vector2 CalcLocalPosition() const;

    Vector2 CalcRelativePosition() const;
    Vector2 CalcRelativePosition(const Vector2& position) const;

    AABB2 GetParentBounds() const;

    void CalcBounds();
    AABB2 CalcBoundsRelativeToParent();

    void CalcBoundsForChildren();
    void CalcBoundsForMeThenMyChildren();
    AABB2 AlignBoundsToContainer(AABB2 bounds, AABB2 container, const Vector2& alignment) noexcept;
    AABB2 CalcRelativeBounds();
    AABB2 CalcAbsoluteBounds();
    AABB2 CalcAlignedAbsoluteBounds();
    AABB2 CalcLocalBounds() const;

    Matrix4 GetLocalTransform() const;
    Matrix4 GetWorldTransform() const;
    Matrix4 GetParentWorldTransform() const;

    bool IsDirty() const;
    bool IsParent() const;
    bool IsChild() const;

    UI::Canvas* GetParentCanvas() const;
    void SetParentCanvas(UI::Canvas& canvas);

    void DebugRenderBottomUp(Renderer* renderer) const;
    void DebugRenderTopDown(Renderer* renderer) const;
    void DebugRenderChildren(Renderer* renderer) const;
    void DebugRenderBoundsAndPivot(Renderer* renderer) const;

    AABB2 GetParentLocalBounds() const;
    AABB2 GetParentRelativeBounds() const;
    void UpdateChildren(float deltaSeconds);
    void RenderChildren(Renderer* renderer) const;

    AABB2 GetBounds(const AABB2& parent, const Vector4& anchors, const Vector4& offsets) const noexcept;
    Vector2 GetSmallestOffset(AABB2 a, AABB2 b) const noexcept;
    AABB2 MoveToBestFit(const AABB2& obj, const AABB2& container) const noexcept;

    float GetAspectRatio() const noexcept;
    float GetInvAspectRatio() const noexcept;

    void SetSize(const Metric& size);
    Vector2 GetSize() const;

    Vector2 GetTopLeft() const noexcept;
    Vector2 GetTopRight() const noexcept;
    Vector2 GetBottomLeft() const noexcept;
    Vector2 GetBottomRight() const noexcept;

private:
    Metric _position{};
    Metric _size{};
    Ratio _pivot{};
    PositionMode _mode{};
    Rgba _fill_color = Rgba::NOALPHA;
    Rgba _edge_color = Rgba::WHITE;
    Element* _parent = nullptr;
    std::vector<Element*> _children{};
    UI::Canvas* _parent_canvas = nullptr;
    AABB2 _bounds{};
    bool _dirty_bounds = false;
};

template<typename T>
T* UI::Element::CreateChild() {
    return (T*)AddChild(new T{});
}

template<typename T>
T* UI::Element::CreateChild(UI::Canvas* parentCanvas) {
    return (T*)AddChild(new T{ parentCanvas });
}

} //End UI