#pragma once

#include "Engine/Core/Rgba.hpp"

#include "Engine/UI/Element.hpp"

class KerningFont;

namespace UI {

class Canvas;

class Label : public UI::Element {
public:
    Label(UI::Canvas* parent_canvas);
    virtual ~Label() = default;

    virtual void Update(float deltaSeconds) override;
    virtual void Render(Renderer* renderer) const override;
    virtual void DebugRender(Renderer* renderer) const override;

    const KerningFont* const GetFont() const;
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

    virtual void SetPosition(const Metric& position) override;
    virtual void SetPositionOffset(const Vector2& offset) override;
    virtual void SetPositionRatio(const UI::Ratio& ratio) override;

protected:
    void CalcBoundsFromFont(KerningFont* font);
private:
    KerningFont* _font = nullptr;
    std::string _text{};
    Rgba _color = Rgba::White;
    float _scale = 1.0f;
};

} //End UI