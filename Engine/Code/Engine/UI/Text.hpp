#pragma once

#include "Engine/Core/Rgba.hpp"

#include "Engine/UI/Element.hpp"

class KerningFont;

namespace UI {

class Canvas;

class Text : public UI::Element {
public:
    Text(UI::Canvas* parent_canvas);
    virtual ~Text() = default;

    virtual void Update(float deltaSeconds) override;
    virtual void Render(Renderer* renderer) const override;
    virtual void DebugRender(Renderer* renderer) const override;

    void SetFont(KerningFont* font);
    void SetText(const std::string& text);
    const std::string& GetText() const;
    std::string& GetText();

    void SetColor(const Rgba& color);
    const Rgba& GetColor() const;
    Rgba& GetColor();

    void SetScale(float value);
    float GetScale() const;
    float GetScale();

protected:
    void CalcBoundsFromFont(KerningFont* font);
private:
    KerningFont* _font = nullptr;
    std::string _text{};
    Rgba _color = Rgba::WHITE;
    float _scale = 1.0f;
};

} //End UI