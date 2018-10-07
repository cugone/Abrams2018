#pragma once

#include "Engine/RHI/RHITypes.hpp"

class RHIOutput;
class IntVector2;
class RHIDevice;
struct IDXGIDebug;

class RHIInstance {
public:
    static RHIInstance* CreateInstance();
    static void DestroyInstance();

    RHIDevice* CreateDevice() noexcept;

protected:
    RHIInstance();
    ~RHIInstance();

private:
    static RHIInstance* _instance;
    static IDXGIDebug* _debuggerInstance;
};