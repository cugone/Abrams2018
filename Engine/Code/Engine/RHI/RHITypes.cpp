#include "Engine/RHI/RHITypes.hpp"

#include <iomanip>
#include <type_traits>

std::ostream& operator<<(std::ostream& out_stream, const GraphicsCardDesc& graphicsCardDesc) {
    auto name = graphicsCardDesc.Description;
    out_stream << std::left << std::setw(22) << "Name:" << std::right << std::setw(30) << name << '\n';
    auto vid = graphicsCardDesc.VendorId;
    out_stream << std::left << std::setw(22) << "Vendor ID:" << std::right << std::setw(30) << vid << '\n';
    auto did = graphicsCardDesc.DeviceId;
    out_stream << std::left << std::setw(22) << "Device ID:" << std::right << std::setw(30) << did << '\n';
    auto subsysid = graphicsCardDesc.SubSysId;
    out_stream << std::left << std::setw(22) << "Subsystem ID:" << std::right << std::setw(30) << subsysid << '\n';
    auto revision = graphicsCardDesc.Revision;
    out_stream << std::left << std::setw(22) << "Revision:" << std::right << std::setw(30) << revision << '\n';
    auto ded_vid_mem = graphicsCardDesc.DedicatedVideoMemory;
    out_stream << std::left << std::setw(22) << "Video Memory:" << std::right << std::setw(30) << ded_vid_mem << '\n';
    auto ded_sys_mem = graphicsCardDesc.DedicatedSystemMemory;
    out_stream << std::left << std::setw(22) << "System Memory:" << std::right << std::setw(30) << ded_sys_mem << '\n';
    auto shared_mem = graphicsCardDesc.SharedSystemMemory;
    out_stream << std::left << std::setw(22) << "Shared System Memory:" << std::right << std::setw(30) << shared_mem << '\n';
    bool is_unspecified = graphicsCardDesc.is_unspecified;
    bool is_software = graphicsCardDesc.is_software;
    bool is_remote = graphicsCardDesc.is_remote;
    out_stream << std::left << std::setw(22) << "Adapter Type:" << std::right << std::setw(30) << (is_software ? "Software" : (is_remote ? "Remote" : (is_unspecified ? "Unspecified" : "Unknown")));
    return out_stream;
}

bool DisplayDescLTComparator::operator()(const DisplayDesc& a, const DisplayDesc& b) const noexcept {
    if(a.width < b.width) {
        return true;
    }
    if(b.width < a.width) {
        return false;
    }
    if(a.height < b.height) {
        return true;
    }
    if(b.height < a.height) {
        return false;
    }
    if(a.refreshRateHz < b.refreshRateHz) {
        return true;
    }
    if(b.refreshRateHz < a.refreshRateHz) {
        return false;
    }
    return false;
}

bool DisplayDescGTComparator::operator()(const DisplayDesc& a, const DisplayDesc& b) const noexcept {
    if(a.width < b.width) {
        return false;
    }
    if(b.width < a.width) {
        return true;
    }
    if(a.height < b.height) {
        return false;
    }
    if(b.height < a.height) {
        return true;
    }
    if(a.refreshRateHz < b.refreshRateHz) {
        return false;
    }
    if(b.refreshRateHz < a.refreshRateHz) {
        return true;
    }
    return false;
}
RHIOutputMode& operator++(RHIOutputMode& mode) {
    using underlying = std::underlying_type_t<RHIOutputMode>;
    mode = static_cast<RHIOutputMode>(static_cast<underlying>(mode) + 1);
    if(mode == RHIOutputMode::Last_) {
        mode = RHIOutputMode::First_;
    }
    return mode;
}

RHIOutputMode operator++(RHIOutputMode& mode, int) {
    RHIOutputMode result = mode;
    ++result;
    return result;
}

BufferBindUsage& operator|=(BufferBindUsage& a, const BufferBindUsage& b) {
    using underlying = std::underlying_type_t<BufferBindUsage>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    a = static_cast<BufferBindUsage>(underlying_a | underlying_b);
    return a;
}

BufferBindUsage operator|(BufferBindUsage a, const BufferBindUsage& b) {
    a |= b;
    return a;
}

BufferBindUsage& operator&=(BufferBindUsage& a, const BufferBindUsage& b) {
    using underlying = std::underlying_type_t<BufferBindUsage>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    a = static_cast<BufferBindUsage>(underlying_a & underlying_b);
    return a;
}

BufferBindUsage operator&(BufferBindUsage a, const BufferBindUsage& b) {
    a &= b;
    return a;
}

PipelineStage& operator|=(PipelineStage& a, const PipelineStage& b) {
    using underlying = std::underlying_type_t<PipelineStage>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    a = static_cast<PipelineStage>(underlying_a | underlying_b);
    return a;
}

PipelineStage operator|(PipelineStage a, const PipelineStage& b) {
    a |= b;
    return a;
}

PipelineStage& operator&=(PipelineStage& a, const PipelineStage& b) {
    using underlying = std::underlying_type_t<PipelineStage>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    a = static_cast<PipelineStage>(underlying_a & underlying_b);
    return a;
}

PipelineStage operator&(PipelineStage a, const PipelineStage& b) {
    a &= b;
    return a;
}

BlendColorWriteEnable operator~(BlendColorWriteEnable a) {
    using underlying = std::underlying_type_t<BlendColorWriteEnable>;
    auto underlying_a = static_cast<underlying>(a);
    return static_cast<BlendColorWriteEnable>(~underlying_a);
}

BlendColorWriteEnable& operator&=(BlendColorWriteEnable& a, const BlendColorWriteEnable& b) {
    using underlying = std::underlying_type_t<BlendColorWriteEnable>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    a = static_cast<BlendColorWriteEnable>(underlying_a & underlying_b);
    return a;
}

BlendColorWriteEnable operator&(BlendColorWriteEnable a, const BlendColorWriteEnable& b) {
    a &= b;
    return a;
}

BlendColorWriteEnable& operator|=(BlendColorWriteEnable& a, const BlendColorWriteEnable& b) {
    using underlying = std::underlying_type_t<BlendColorWriteEnable>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    a = static_cast<BlendColorWriteEnable>(underlying_a | underlying_b);
    return a;
}

BlendColorWriteEnable operator|(BlendColorWriteEnable a, const BlendColorWriteEnable& b) {
    a |= b;
    return a;
}

BlendColorWriteEnable& operator^=(BlendColorWriteEnable& a, const BlendColorWriteEnable& b) {
    using underlying = std::underlying_type_t<BlendColorWriteEnable>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    a = static_cast<BlendColorWriteEnable>(underlying_a ^ underlying_b);
    return a;
}

BlendColorWriteEnable operator^(BlendColorWriteEnable a, const BlendColorWriteEnable& b) {
    using underlying = std::underlying_type_t<BlendColorWriteEnable>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    return static_cast<BlendColorWriteEnable>(underlying_a ^ underlying_b);
}

ResourceMiscFlag operator|(const ResourceMiscFlag& a, const ResourceMiscFlag& b) {
    using underlying = std::underlying_type_t<ResourceMiscFlag>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    return static_cast<ResourceMiscFlag>(underlying_a | underlying_b);
}

ResourceMiscFlag operator&(const ResourceMiscFlag& a, const ResourceMiscFlag& b) {
    using underlying = std::underlying_type_t<ResourceMiscFlag>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    return static_cast<ResourceMiscFlag>(underlying_a & underlying_b);
}
