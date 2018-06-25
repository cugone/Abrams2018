#include "Engine/RHI/RHITypes.hpp"

#include <type_traits>

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

BufferBindUsage operator|(const BufferBindUsage& a, const BufferBindUsage& b) {
    using underlying = std::underlying_type_t<BufferBindUsage>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    return static_cast<BufferBindUsage>(underlying_a | underlying_b);
}

BufferBindUsage operator&(const BufferBindUsage& a, const BufferBindUsage& b) {
    using underlying = std::underlying_type_t<BufferBindUsage>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    return static_cast<BufferBindUsage>(underlying_a & underlying_b);
}

PipelineStage operator|(const PipelineStage& a, const PipelineStage& b) {
    using underlying = std::underlying_type_t<PipelineStage>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    return static_cast<PipelineStage>(underlying_a | underlying_b);
}

PipelineStage operator&(const PipelineStage& a, const PipelineStage& b) {
    using underlying = std::underlying_type_t<PipelineStage>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    return static_cast<PipelineStage>(underlying_a & underlying_b);
}


BlendColorWriteEnable operator~(const BlendColorWriteEnable& a) {
    using underlying = std::underlying_type_t<BlendColorWriteEnable>;
    auto underlying_a = static_cast<underlying>(a);
    return static_cast<BlendColorWriteEnable>(~underlying_a);
}

BlendColorWriteEnable operator&(const BlendColorWriteEnable& a, const BlendColorWriteEnable& b) {
    using underlying = std::underlying_type_t<BlendColorWriteEnable>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    return static_cast<BlendColorWriteEnable>(underlying_a & underlying_b);
}

BlendColorWriteEnable operator|(const BlendColorWriteEnable& a, const BlendColorWriteEnable& b) {
    using underlying = std::underlying_type_t<BlendColorWriteEnable>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    return static_cast<BlendColorWriteEnable>(underlying_a | underlying_b);
}

BlendColorWriteEnable operator^(const BlendColorWriteEnable& a, const BlendColorWriteEnable& b) {
    using underlying = std::underlying_type_t<BlendColorWriteEnable>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    return static_cast<BlendColorWriteEnable>(underlying_a ^ underlying_b);
}

BlendColorWriteEnable& operator&=(BlendColorWriteEnable& a, const BlendColorWriteEnable& b) {

    using underlying = std::underlying_type_t<BlendColorWriteEnable>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    a = static_cast<BlendColorWriteEnable>(underlying_a & underlying_b);
    return a;
}

BlendColorWriteEnable& operator|=(BlendColorWriteEnable& a, const BlendColorWriteEnable& b) {

    using underlying = std::underlying_type_t<BlendColorWriteEnable>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    a = static_cast<BlendColorWriteEnable>(underlying_a | underlying_b);
    return a;
}

BlendColorWriteEnable& operator^=(BlendColorWriteEnable& a, const BlendColorWriteEnable& b) {

    using underlying = std::underlying_type_t<BlendColorWriteEnable>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    a = static_cast<BlendColorWriteEnable>(underlying_a ^ underlying_b);
    return a;
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
