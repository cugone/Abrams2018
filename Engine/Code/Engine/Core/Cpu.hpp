#pragma once

#include <ostream>

namespace System::Cpu {

enum class ProcessorArchitecture;

struct CpuDesc {
    ProcessorArchitecture type{};
    int processorCount = 0;
    int coreCount = 0;
    friend std::ostream& operator<<(std::ostream& out, const CpuDesc& cpu);
};

CpuDesc GetCpuDesc();

}