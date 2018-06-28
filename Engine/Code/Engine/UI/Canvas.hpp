#pragma once

class Texture2D;

namespace UI {

class Canvas : public Element {
public:
    Canvas(Texture2D* target_texture, float reference_resolution);
    ~Canvas();

protected:
private:
    Texture2D* _target_texture = nullptr;
    float _reference_resolution = 1.0f;
};

} //End UI