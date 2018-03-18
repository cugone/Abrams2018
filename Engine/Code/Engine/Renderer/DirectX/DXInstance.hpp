#pragma once

#include "Engine/RHI/RHIInstance.hpp"

class IntVector2;
struct IDXGIDebug;

class DXInstance : public RHIInstance {
public:
    DXInstance();
    ~DXInstance();

    virtual RHIOutput* CreateOutput(const IntVector2& client_size, const IntVector2& client_position, const RHIOutputMode& output_mode) override;

protected:
private:
    static IDXGIDebug* _debuggerInstance;
};