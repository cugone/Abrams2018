#pragma once

#include "Engine/RHI/RHITypes.hpp"

class RHIOutput;
class IntVector2;

class RHIInstance {
public:
    static RHIInstance* CreateInstance(const RHIInstanceType& instance_type);
    static void DestroyInstance();

    virtual RHIOutput* CreateOutput(const IntVector2& client_size, const IntVector2& client_position, const RHIOutputMode& output_mode);
protected:
    RHIInstance();
    virtual ~RHIInstance() = 0;

    RHIInstanceType _instanceType = RHIInstanceType::NONE;
private:
    static RHIInstance* _instance;
};