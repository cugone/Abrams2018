#include "Engine/UI/Text.hpp"

#include "Engine/Core/KerningFont.hpp"

#include "Engine/Renderer/Renderer.hpp"

#include "Engine/UI/Types.hpp"

namespace UI {

Text::Text(UI::Canvas* parent_canvas)
    : UI::Element(parent_canvas)
{
    /* DO NOTHING */
}

void Text::Update(float /*deltaSeconds*/) {
    /* DO NOTHING */
}

void Text::Render(Renderer* renderer) const {
    renderer->SetModelMatrix(GetWorldTransform());
    renderer->DrawTextLine(_font, _text, _color, _scale);
}

void Text::DebugRender(Renderer* renderer) const {
    DebugRenderBoundsAndPivot(renderer);
}

void Text::SetFont(KerningFont* font) {
    _font = font;
    DirtyElement();
}

void Text::SetText(const std::string& text) {
    _text = text;
    DirtyElement();
    CalcBoundsForMeThenMyChildren();
}

const std::string& Text::GetText() const {
    return _text;
}

std::string& Text::GetText() {
    return const_cast<std::string&>(static_cast<const UI::Text&>(*this).GetText());
}

void Text::SetColor(const Rgba& color) {
    _color = color;
}

const Rgba& Text::GetColor() const {
    return _color;
}

Rgba& Text::GetColor() {
    return const_cast<Rgba&>(static_cast<const UI::Text&>(*this).GetColor());
}

void Text::SetScale(float value) {
    _scale = value;
    DirtyElement();
}

float Text::GetScale() const {
    return _scale;
}

float Text::GetScale() {
    return static_cast<const UI::Text&>(*this).GetScale();
}

void Text::CalcBoundsFromFont(KerningFont* font) {
    if(font == nullptr) {
        return;
    }
    float width = font->CalculateTextWidth(_text, _scale);
    float height = font->CalculateTextHeight(_text, _scale);
    auto old_size = GetSize();
    float old_width = old_size.x;
    float old_height = old_size.y;
    if(old_width < width) {
        SetSize(UI::Metric{ UI::Ratio{ Vector2::ZERO }, Vector2{ width, old_height } });
    }
    if(old_height < height) {
        SetSize(UI::Metric{ UI::Ratio{ Vector2::ZERO }, Vector2{ old_width, height } });
    }
}

} //End UI