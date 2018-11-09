#include "Engine/UI/Sprite.hpp"

namespace UI {

Sprite::Sprite(Canvas* parent_canvas, AnimatedSprite* sprite)
    : Element(parent_canvas)
    , _sprite(sprite)
{

}

void Sprite::Update(float deltaSeconds) {
    /* DO NOTHING */
}

void Sprite::Render(Renderer* renderer) const {
    /* DO NOTHING */
}

void Sprite::DebugRender(Renderer* renderer) const {

}

void Sprite::SetPosition(const Metric& position) {
    /* DO NOTHING */
}

}