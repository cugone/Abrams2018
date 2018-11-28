#include "Engine/UI/Label.hpp"

#include "Engine/Core/KerningFont.hpp"

#include "Engine/Renderer/Camera2D.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include "Engine/UI/Canvas.hpp"
#include "Engine/UI/Types.hpp"

namespace UI {

Label::Label(UI::Canvas* parent_canvas)
    : UI::Element(parent_canvas)
{
    /* DO NOTHING */
}

Label::Label(UI::Canvas* parent_canvas, KerningFont* font, const std::string& text /*= "Label"*/)
    : UI::Element(parent_canvas)
    , _font(font)
    , _text(text)
{
    DirtyElement();
    CalcBoundsFromFont(_font);
}

void Label::Render(Renderer* renderer) const {
    if(IsHidden()) {
        return;
    }
    auto world_transform = GetWorldTransform();
    auto world_transform_scale = world_transform.GetScale();
    auto inv_scale_x = 1.0f / world_transform_scale.x;
    auto inv_scale_y = 1.0f / world_transform_scale.y;
    auto inv_scale_z = 1.0f / world_transform_scale.z;
    auto inv_scale = Vector3(inv_scale_x, inv_scale_y, inv_scale_z);
    auto inv_scale_matrix = Matrix4::CreateScaleMatrix(inv_scale);
    Vector2 extents = GetSize();
    Vector2 half_extents = extents * 0.5f;
    auto inv_half_extents = Vector2(-half_extents.x, -half_extents.y);
    auto inv_half_extents_matrix = Matrix4::CreateTranslationMatrix(inv_half_extents);
    renderer->SetModelMatrix(world_transform * inv_scale_matrix * inv_half_extents_matrix);
    renderer->SetMaterial(_font->GetMaterial());
    renderer->DrawMultilineText(_font, _text, _color);
}

const KerningFont* const Label::GetFont() const {
    return _font;
}

void Label::SetFont(KerningFont* font) {
    _font = font;
    DirtyElement();
    CalcBoundsFromFont(_font);
}

void Label::SetText(const std::string& text) {
    _text = text;
    DirtyElement();
    CalcBoundsFromFont(_font);
}

const std::string& Label::GetText() const {
    return _text;
}

std::string& Label::GetText() {
    return const_cast<std::string&>(static_cast<const UI::Label&>(*this).GetText());
}

void Label::SetColor(const Rgba& color) {
    _color = color;
}

const Rgba& Label::GetColor() const {
    return _color;
}

Rgba& Label::GetColor() {
    return const_cast<Rgba&>(static_cast<const UI::Label&>(*this).GetColor());
}

void Label::SetScale(float value) {
    _scale = value;
    DirtyElement();
    CalcBoundsFromFont(_font);
}

float Label::GetScale() const {
    return _scale;
}

float Label::GetScale() {
    return static_cast<const UI::Label&>(*this).GetScale();
}

void Label::SetPosition(const Metric& position) {
    Element::SetPosition(position);
    CalcBoundsFromFont(_font);
}

void Label::SetPositionOffset(const Vector2& offset) {
    Element::SetPositionOffset(offset);
    CalcBoundsFromFont(_font);
}

void Label::SetPositionRatio(const UI::Ratio& ratio) {
    Element::SetPositionRatio(ratio);
    CalcBoundsFromFont(_font);
}

void Label::CalcBoundsFromFont(KerningFont* font) {
    if(font == nullptr) {
        return;
    }
    float width = font->CalculateTextWidth(_text, _scale);
    float height = font->CalculateTextHeight(_text, _scale);
    auto old_size = GetSize();
    float old_width = old_size.x;
    float old_height = old_size.y;
    if(old_width < width && old_height < height) {
        SetSize(UI::Metric{ UI::Ratio{ _size.ratio }, Vector2{ width, height } });
    } else {
        if(old_width < width) {
            SetSize(UI::Metric{ UI::Ratio{ _size.ratio }, Vector2{ width, old_height } });
        } else if(old_height < height) {
            SetSize(UI::Metric{ UI::Ratio{ _size.ratio }, Vector2{ old_width, height } });
        }
    }
}

} //End UI