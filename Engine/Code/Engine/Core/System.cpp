#include "Engine/Core/System.hpp"

#include "Engine/Core/Win.hpp"
#include "Engine/Math/MathUtils.hpp"

#include <iomanip>

std::ostream& System::operator<<(std::ostream& out, const System::SystemDesc& desc) {
    auto old_fmt = out.flags();
    auto old_w = out.width();
    out << "SYSTEM:\n";
    out << desc.cpu;
    out << desc.ram;
    out << '\n';
    out.flags(old_fmt);
    out.width(old_w);
    return out;
}

System::SystemDesc System::GetSystemDesc() {
    SystemDesc desc{};
    desc.cpu = Cpu::GetCpuDesc();
    desc.ram = Ram::GetRamDesc();
    return desc;
}
