#pragma once

#include "Engine/Math/IntVector3.hpp"

class RHIDevice;

class Texture {
public:
    Texture() noexcept = default;
    Texture(RHIDevice* device) noexcept;
    Texture(Texture&& r_other) noexcept;
    Texture(const Texture& other) noexcept = delete;
    Texture& operator=(const Texture& rhs) noexcept = delete;
    Texture& operator=(Texture&& rhs) noexcept;
    virtual ~Texture() noexcept = 0;

    const IntVector3& GetDimensions() const noexcept;

    void IsLoaded(bool is_loaded) noexcept;
    bool IsLoaded() const noexcept;
protected:
    RHIDevice* _device = nullptr;
    IntVector3 _dimensions = IntVector3::ZERO;
    bool _isLoaded = false;
private:
};
