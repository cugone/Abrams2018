#pragma once

#include "Engine/Renderer/Texture.hpp"

class RHIDevice;

class Texture2D : public Texture {
public:
    Texture2D() = default;
    Texture2D(RHIDevice* device);
    Texture2D(Texture2D&& r_other) noexcept;
    Texture2D(const Texture2D& other) noexcept = delete;
    Texture2D& operator=(const Texture2D& rhs) noexcept = delete;
    Texture2D& operator=(Texture2D&& rhs) noexcept;

    virtual void SetDebugName(char const* name) noexcept = 0;

    virtual ~Texture2D();

protected:
private:

};