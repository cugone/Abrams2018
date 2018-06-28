#pragma once

#include "Engine/Math/Vector2.hpp"
#include "Engine/UI/Canvas.hpp"

namespace UI {

class Element {
public:
    Element() = default;
    Element(UI::Canvas& parent_canvas);
    virtual ~Element();

protected:
private:

};

} //End UI