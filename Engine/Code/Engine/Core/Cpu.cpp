#include "Engine/Core/Cpu.hpp"

#include "Engine/Core/Win.hpp"

#include <iomanip>

std::ostream& System::Cpu::operator<<(std::ostream& out, const System::Cpu::CpuDesc& cpu) {
    auto old_fmt = out.flags();
    auto old_w = out.width();
    out << std::left << std::setw(22) << "Processor Type:" << std::right << std::setw(30) << ProcessorArchitectureToString(cpu.type) << '\n';
    out << std::left << std::setw(22) << "Processor Count:" << std::right << std::setw(30) << cpu.processorCount << '\n';
    out << std::left << std::setw(22) << "Core Count: " << std::right << std::setw(30) << cpu.coreCount << '\n';
    out.flags(old_fmt);
    out.width(old_w);
    return out;
}

std::string System::Cpu::ProcessorArchitectureToString(const ProcessorArchitecture& architecture) {
    switch(architecture) {
    case ProcessorArchitecture::Unknown:
        return "Unknown";
    case ProcessorArchitecture::x64: //Also Amd64
        return "x64";
    case ProcessorArchitecture::Arm:
        return "ARM";
    case ProcessorArchitecture::Arm64:
        return "ARM 64";
    case ProcessorArchitecture::Ia64:
        return "Intel Itanium 64";
    case ProcessorArchitecture::x86: //Also Intel
        return "x86";
    case ProcessorArchitecture::Mips:
        return "Mips";
    case ProcessorArchitecture::Alpha:
        return "Alpha";
    case ProcessorArchitecture::Ppc:
        return "PPC";
    case ProcessorArchitecture::Shx:
        return "SHX";
    case ProcessorArchitecture::Alpha64:
        return "Alpha 64";
    case ProcessorArchitecture::Msil:
        return "MSIL";
    case ProcessorArchitecture::Ia32OnWin64:
        return "Intel Itanium on Win64";
    case ProcessorArchitecture::Neutral:
        return "Neutral";
    case ProcessorArchitecture::Arm32OnWin64:
        return "ARM32 on Win64";
    case ProcessorArchitecture::Ia32OnArm64:
        return "Intel Itanium on ARM64";
    default:
        return "";
    }
}

System::Cpu::ProcessorArchitecture System::Cpu::GetProcessorArchitecture() {
    SYSTEM_INFO info{};
    ::GetSystemInfo(&info);
    switch(info.wProcessorArchitecture) {
    case PROCESSOR_ARCHITECTURE_INTEL: return ProcessorArchitecture::Intel;
    case PROCESSOR_ARCHITECTURE_MIPS: return ProcessorArchitecture::Mips;
    case PROCESSOR_ARCHITECTURE_ALPHA: return ProcessorArchitecture::Alpha;
    case PROCESSOR_ARCHITECTURE_PPC: return ProcessorArchitecture::Ppc;
    case PROCESSOR_ARCHITECTURE_SHX: return ProcessorArchitecture::Shx;
    case PROCESSOR_ARCHITECTURE_ARM: return ProcessorArchitecture::Arm;
    case PROCESSOR_ARCHITECTURE_IA64: return ProcessorArchitecture::Ia64;
    case PROCESSOR_ARCHITECTURE_ALPHA64: return ProcessorArchitecture::Alpha64;
    case PROCESSOR_ARCHITECTURE_MSIL: return ProcessorArchitecture::Msil;
    case PROCESSOR_ARCHITECTURE_AMD64: return ProcessorArchitecture::Amd64;
    case PROCESSOR_ARCHITECTURE_IA32_ON_WIN64: return ProcessorArchitecture::Ia32OnWin64;
    case PROCESSOR_ARCHITECTURE_NEUTRAL: return ProcessorArchitecture::Neutral;
    case PROCESSOR_ARCHITECTURE_ARM64: return ProcessorArchitecture::Arm64;
    case PROCESSOR_ARCHITECTURE_ARM32_ON_WIN64: return ProcessorArchitecture::Arm32OnWin64;
    case PROCESSOR_ARCHITECTURE_IA32_ON_ARM64: return ProcessorArchitecture::Ia32OnArm64;
    case PROCESSOR_ARCHITECTURE_UNKNOWN: return ProcessorArchitecture::Unknown;
    default: return ProcessorArchitecture::Unknown;
    }
}

System::Cpu::CpuDesc System::Cpu::GetCpuDesc() {
    CpuDesc desc{};
    desc.type = GetProcessorArchitecture();
    SYSTEM_INFO info{};
    ::GetSystemInfo(&info);
    desc.coreCount = info.dwNumberOfProcessors;
    DWORD length{};
    if(!::GetLogicalProcessorInformationEx(LOGICAL_PROCESSOR_RELATIONSHIP::RelationAll, nullptr, &length)) {
        if(::GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
            auto buffer = reinterpret_cast<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX*>(new unsigned char[length]);
            if(!::GetLogicalProcessorInformationEx(LOGICAL_PROCESSOR_RELATIONSHIP::RelationAll, buffer, &length)) {
                delete[] buffer;
                buffer = nullptr;
            } else {
                SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX* ptr = buffer;
                unsigned long long offset = 0ull;
                while(offset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX) <= length) {
                    switch(ptr->Relationship) {
                    case RelationProcessorPackage:
                    {
                        ++desc.processorCount;
                        break;
                    }
                    default:
                        break;
                    }
                    offset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX);
                    ptr++;
                }
            }
        }
    }
    return desc;
}
