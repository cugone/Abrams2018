#pragma once

#include "Engine/UI/Element.hpp"

class Texture;
class Renderer;
class Camera2D;

namespace UI {

class Canvas : public UI::Element {
public:
    Canvas(Renderer& renderer, Texture* target_texture, Texture* target_depthstencil, float reference_resolution);
    virtual ~Canvas() = default;
    virtual void Update(float deltaSeconds) override;
    virtual void Render(Renderer* renderer) const override;
    void SetupMVPFromTargetAndCamera(Renderer* renderer) const;
    virtual void DebugRender(Renderer* renderer) const override;
    const Camera2D* GetUICamera() const;
protected:
private:
    void CalcDimensionsAndAspectRatio(Vector2& dimensions, float& aspectRatio);
    void SetTargetTexture(Renderer& renderer, Texture* target);

    Camera2D* _camera = nullptr;
    Renderer* _renderer = nullptr;
    Texture* _target_texture = nullptr;
    Texture* _target_depthstencil = nullptr;
    float _reference_resolution = 0.0f;
    float _aspect_ratio = 1.0f;
};

} //End UI