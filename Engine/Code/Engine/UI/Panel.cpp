#include "Engine/UI/Panel.hpp"

#include "Engine/UI/Canvas.hpp"

namespace UI {

Panel::Panel(UI::Canvas* parent_canvas)
    : Element(parent_canvas)
{
    /* DO NOTHING */
}

void Panel::Update(float deltaSeconds) {
    UpdateChildren(deltaSeconds);
}

void Panel::Render(Renderer* renderer) const {
    RenderChildren(renderer);
}

void Panel::DebugRender(Renderer* renderer, bool showSortOrder /*= false*/) const {
    DebugRenderBottomUp(renderer, showSortOrder);
}

} //End UI