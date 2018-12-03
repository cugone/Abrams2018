#pragma once

#include <ostream>

namespace System::Ram {

struct RamDesc {
    unsigned long long installed = 0ull;
    unsigned long long available = 0ull;
    friend std::ostream& operator<<(std::ostream& out, const RamDesc& desc);
};

RamDesc GetRamDesc();

}