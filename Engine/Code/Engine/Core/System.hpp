#pragma once

#include "Engine/Core/Cpu.hpp"

#include <ostream>

namespace System {

struct SystemDesc {
    unsigned long long available_ram = 0ull;
    unsigned long long installed_ram = 0ull;
    System::Cpu::CpuDesc cpu{};
    friend std::ostream& operator<<(std::ostream& out, const SystemDesc& desc);
};

SystemDesc GetSystemDesc();
unsigned long long GetPhysicalRam();
unsigned long long GetAvailableRam();

}