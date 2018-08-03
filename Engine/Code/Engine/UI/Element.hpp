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
    explicit Element(UI::Canvas* parent_canvas);
    virtual ~Element() = 0;

    virtual void Update(float deltaSeconds);
    virtual void Render(Renderer* renderer) const;
    virtual void DebugRender(Renderer* renderer) const;

    template<typename T>
    T* CreateChild();
    template<typename T>
    T* CreateChild(UI::Canvas* parentCanvas);
    template<typename T>
    T* CreateChildBefore(UI::Element* youngerSibling);
    template<typename T>
    T* CreateChildBefore(UI::Canvas* parentCanvas, UI::Element* youngerSibling);
    template<typename T>
    T* CreateChildAfter(UI::Element* olderSibling);
    template<typename T>
    T* CreateChildAfter(UI::Canvas* parentCanvas, UI::Element* olderSibling);

    UI::Element* AddChild(UI::Element* child);
    UI::Element* AddChildBefore(UI::Element* child, UI::Element* younger_sibling);
    UI::Element* AddChildAfter(UI::Element* child, UI::Element* older_sibling);

    void RemoveChild(Element* child);
    void RemoveAllChildren();
    void RemoveSelf();

    void DestroyChild(UI::Element*& child);
    void DestroyAllChildren();

    void SetBorderColor(const Rgba& color);
    void SetBackgroundColor(const Rgba& color);
    void SetPivotColor(const Rgba& color);
    void SetDebugColors(const Rgba& edge, const Rgba& fill, const Rgba& pivot = Rgba::RED);


    void SetSize(const Metric& size);
    Vector2 GetSize() const;

    const UI::Metric& GetPosition() const;
    virtual void SetPosition(const Metric& position);

    void SetPivot(const Vector2& pivotPosition);
    const Vector2& GetPivot() const;
    void SetPivot(const PivotPosition& pivotPosition);

    void SetOrientationDegrees(float value);
    void SetOrientationRadians(float value);
    float GetOrientationDegrees() const;
    float GetOrientationRadians() const;

    void SetOrder(std::size_t value);
    std::size_t GetOrder() const;

protected:

    Vector2 CalcLocalPosition() const;
    Vector2 CalcLocalScale() const;
    float CalcLocalRotationRadians() const;
    float CalcLocalRotationDegrees() const;
    float CalcWorldRotationRadians() const;
    float CalcWorldRotationDegrees() const;

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

    void DirtyElement();
    bool IsDirty() const;
    bool IsParent() const;
    bool IsChild() const;

    UI::Canvas* GetParentCanvas() const;
    void SetParentCanvas(UI::Canvas* canvas);

    void DebugRenderBottomUp(Renderer* renderer) const;
    void DebugRenderTopDown(Renderer* renderer) const;
    void DebugRenderChildren(Renderer* renderer) const;
    void DebugRenderBoundsAndPivot(Renderer* renderer) const;
    void DebugRenderPivot(Renderer* renderer) const;
    void DebugRenderBounds(Renderer* renderer) const;
    AABB2 GetParentLocalBounds() const;
    AABB2 GetParentRelativeBounds() const;
    void UpdateChildren(float deltaSeconds);
    void RenderChildren(Renderer* renderer) const;

    AABB2 GetBounds(const AABB2& parent, const Vector4& anchors, const Vector4& offsets) const noexcept;
    Vector2 GetSmallestOffset(AABB2 a, AABB2 b) const noexcept;
    AABB2 MoveToBestFit(const AABB2& obj, const AABB2& container) const noexcept;

    float GetAspectRatio() const noexcept;
    float GetInvAspectRatio() const noexcept;

    Vector2 GetTopLeft() const noexcept;
    Vector2 GetTopRight() const noexcept;
    Vector2 GetBottomLeft() const noexcept;
    Vector2 GetBottomRight() const noexcept;

private:
    Metric _position{};
    Metric _size{};
    HalfExtent _pivot{};
    PositionMode _mode{};
    Rgba _fill_color = Rgba::NOALPHA;
    Rgba _edge_color = Rgba::WHITE;
    Rgba _pivot_color = Rgba::RED;
    Element* _parent = nullptr;
    std::vector<Element*> _children{};
    UI::Canvas* _parent_canvas = nullptr;
    AABB2 _bounds{};
    float _orientationRadians = 0.0f;
    std::size_t _order = 0;
    bool _dirty_bounds = false;

    float GetParentOrientationRadians() const;
    float GetParentOrientationDegrees() const;
    void SortChildren();
};

template<typename T>
T* UI::Element::CreateChild() {
    return (T*)AddChild(new T{});
}

template<typename T>
T* UI::Element::CreateChild(UI::Canvas* parentCanvas) {
    return (T*)AddChild(new T{ parentCanvas });
}

template<typename T>
T* UI::Element::CreateChildBefore(UI::Element* youngerSibling) {
    return (T*)AddChildBefore(new T{}, youngerSibling);
}

template<typename T>
T* UI::Element::CreateChildAfter(UI::Element* olderSibling) {
    return (T*)AddChildAfter(new T{}, olderSibling);
}

template<typename T>
T* UI::Element::CreateChildBefore(UI::Canvas* parentCanvas, UI::Element* youngerSibling) {
    return (T*)AddChildBefore(new T{ parentCanvas }, youngerSibling);
}

template<typename T>
T* UI::Element::CreateChildAfter(UI::Canvas* parentCanvas, UI::Element* olderSibling) {
    return (T*)AddChildAfter(new T{ parentCanvas }, olderSibling);
}

bool operator<(const Element& a, const Element& b);

} //End UI