#pragma once

#include "Engine/Core/Cpu.hpp"
#include "Engine/Core/Ram.hpp"

#include <ostream>

namespace System {

struct SystemDesc {
    System::Ram::RamDesc ram{};
    System::Cpu::CpuDesc cpu{};
    friend std::ostream& operator<<(std::ostream& out, const SystemDesc& desc);
};

SystemDesc GetSystemDesc();

}