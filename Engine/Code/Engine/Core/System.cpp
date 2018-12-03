#include "Engine/Core/System.hpp"

#include "Engine/Core/Win.hpp"
#include "Engine/Math/MathUtils.hpp"

#include <iomanip>

std::ostream& System::operator<<(std::ostream& out, const System::SystemDesc& desc) {
    auto old_fmt = out.flags();
    auto old_w = out.width();
    out << "SYSTEM:\n";
    out << desc.cpu;
    out << std::left << std::setw(22) << "Installed RAM:" << std::right << std::setw(30) << std::fixed << std::setprecision(1) << desc.installed_ram * MathUtils::GIB_BYTES_RATIO << " GB\n";
    out << std::left << std::setw(22) << "Available RAM:" << std::right << std::setw(30) << std::fixed << std::setprecision(1) << desc.available_ram * MathUtils::GIB_BYTES_RATIO << " GB\n";
    out << '\n';
    out.flags(old_fmt);
    out.width(old_w);
    return out;
}

System::SystemDesc System::GetSystemDesc() {
    SystemDesc desc{};
    desc.cpu = Cpu::GetCpuDesc();
    desc.installed_ram = GetPhysicalRam();
    desc.available_ram = GetAvailableRam();
    return desc;
}

unsigned long long System::GetPhysicalRam() {
    uint64_t pram = 0;
    ::GetPhysicallyInstalledSystemMemory(&pram);
    return static_cast<unsigned long long>(pram * MathUtils::BYTES_KIB_RATIO);
}

unsigned long long System::GetAvailableRam() {
    MEMORYSTATUSEX mem{};
    mem.dwLength = sizeof(MEMORYSTATUSEX);
    ::GlobalMemoryStatusEx(&mem);
    return mem.ullTotalPhys;
}

