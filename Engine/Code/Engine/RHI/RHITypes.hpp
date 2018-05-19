#pragma once

#include <type_traits>

enum class RHIOutputMode : unsigned int {
    FIRST_
    , WINDOWED = FIRST_
    , BORDERLESS
    , FULLSCREEN_WINDOW
    , LAST_
};

RHIOutputMode& operator++(RHIOutputMode& mode);
RHIOutputMode operator++(RHIOutputMode& mode, int);

enum class BufferType : unsigned int {
    NONE
    , VERTEX
    , INDEX
    , STRUCTURED
    , CONSTANT
    , READWRITE
};

enum class PipelineStage : unsigned char {
    NONE  = 0b00000000
    , VS  = 0b00000001
    , HS  = 0b00000010
    , DS  = 0b00000100
    , GS  = 0b00001000
    , PS  = 0b00010000
    , CS  = 0b00100000
    , ALL = VS | HS | DS | GS | PS | CS
};

PipelineStage operator|(const PipelineStage& a, const PipelineStage& b);
PipelineStage operator&(const PipelineStage& a, const PipelineStage& b);

enum class ComparisonFunction {
    NEVER
    , LESS
    , EQUAL
    , LESS_EQUAL
    , GREATER
    , NOT_EQUAL
    , GREATER_EQUAL
    , ALWAYS
};

enum class StencilOperation {
    KEEP
    , ZERO
    , REPLACE
    , INCREMENT_CLAMP
    , DECREMENT_CLAMP
    , INVERT
    , INCREMENT_WRAP
    , DECREMENT_WRAP
};

enum class ImageFormat : unsigned int {
    UNKNOWN
    , R32G32B32A32_TYPELESS
    , R32G32B32A32_FLOAT
    , R32G32B32A32_UINT
    , R32G32B32A32_SINT
    , R32G32B32_TYPELESS
    , R32G32B32_FLOAT
    , R32G32B32_UINT
    , R32G32B32_SINT
    , R16G16B16A16_TYPELESS
    , R16G16B16A16_FLOAT
    , R16G16B16A16_UNORM
    , R16G16B16A16_UINT
    , R16G16B16A16_SNORM
    , R16G16B16A16_SINT
    , R32G32_TYPELESS
    , R32G32_FLOAT
    , R32G32_UINT
    , R32G32_SINT
    , R32G8X24_TYPELESS
    , D32_FLOAT_S8X24_UINT
    , R32_FLOAT_X8X24_TYPELESS
    , X32_TYPELESS_G8X24_UINT
    , R10G10B10A2_TYPELESS
    , R10G10B10A2_UNORM
    , R10G10B10A2_UINT
    , R11G11B10_FLOAT
    , R8G8B8A8_TYPELESS
    , R8G8B8A8_UNORM
    , R8G8B8A8_UNORM_SRGB
    , R8G8B8A8_UINT
    , R8G8B8A8_SNORM
    , R8G8B8A8_SINT
    , R16G16_TYPELESS
    , R16G16_FLOAT
    , R16G16_UNORM
    , R16G16_UINT
    , R16G16_SNORM
    , R16G16_SINT
    , R32_TYPELESS
    , D32_FLOAT
    , R32_FLOAT
    , R32_UINT
    , R32_SINT
    , R24G8_TYPELESS
    , D24_UNORM_S8_UINT
    , R24_UNORM_X8_TYPELESS
    , X24_TYPELESS_G8_UINT
    , R8G8_TYPELESS
    , R8G8_UNORM
    , R8G8_UINT
    , R8G8_SNORM
    , R8G8_SINT
    , R16_TYPELESS
    , R16_FLOAT
    , D16_UNORM
    , R16_UNORM
    , R16_UINT
    , R16_SNORM
    , R16_SINT
    , R8_TYPELESS
    , R8_UNORM
    , R8_UINT
    , R8_SNORM
    , R8_SINT
    , A8_UNORM
    , R1_UNORM
    , R9G9B9E5_SHAREDEXP
    , R8G8_B8G8_UNORM
    , G8R8_G8B8_UNORM
    , BC1_TYPELESS
    , BC1_UNORM
    , BC1_UNORM_SRGB
    , BC2_TYPELESS
    , BC2_UNORM
    , BC2_UNORM_SRGB
    , BC3_TYPELESS
    , BC3_UNORM
    , BC3_UNORM_SRGB
    , BC4_TYPELESS
    , BC4_UNORM
    , BC4_SNORM
    , BC5_TYPELESS
    , BC5_UNORM
    , BC5_SNORM
    , B5G6R5_UNORM
    , B5G5R5A1_UNORM
    , B8G8R8A8_UNORM
    , B8G8R8X8_UNORM
    , R10G10B10_XR_BIAS_A2_UNORM
    , B8G8R8A8_TYPELESS
    , B8G8R8A8_UNORM_SRGB
    , B8G8R8X8_TYPELESS
    , B8G8R8X8_UNORM_SRGB
    , BC6H_TYPELESS
    , BC6H_UF16
    , BC6H_SF16
    , BC7_TYPELESS
    , BC7_UNORM
    , BC7_UNORM_SRGB
    , AYUV
    , Y410
    , Y416
    , NV12
    , P010
    , P016
    , OPAQUE_420
    , YUY2
    , Y210
    , Y216
    , NV11
    , AI44
    , IA44
    , P8
    , A8P8
    , B4G4R4A4_UNORM
};

enum class PrimitiveType : unsigned int {
    NONE
    , POINTS
    , LINES
    , TRIANGLES
    , LINES_ADJ
    , LINESSTRIP
    , LINESSTRIP_ADJ
    , TRIANGLESTRIP
    , TRIANGLES_ADJ
    , TRIANGLESTRIP_ADJ
    , CONTROL_POINT_PATCHLIST_1
    , CONTROL_POINT_PATCHLIST_2
    , CONTROL_POINT_PATCHLIST_3
    , CONTROL_POINT_PATCHLIST_4
    , CONTROL_POINT_PATCHLIST_5
    , CONTROL_POINT_PATCHLIST_6
    , CONTROL_POINT_PATCHLIST_7
    , CONTROL_POINT_PATCHLIST_8
    , CONTROL_POINT_PATCHLIST_9
    , CONTROL_POINT_PATCHLIST_10
    , CONTROL_POINT_PATCHLIST_11
    , CONTROL_POINT_PATCHLIST_12
    , CONTROL_POINT_PATCHLIST_13
    , CONTROL_POINT_PATCHLIST_14
    , CONTROL_POINT_PATCHLIST_15
    , CONTROL_POINT_PATCHLIST_16
    , CONTROL_POINT_PATCHLIST_17
    , CONTROL_POINT_PATCHLIST_18
    , CONTROL_POINT_PATCHLIST_19
    , CONTROL_POINT_PATCHLIST_20
    , CONTROL_POINT_PATCHLIST_21
    , CONTROL_POINT_PATCHLIST_22
    , CONTROL_POINT_PATCHLIST_23
    , CONTROL_POINT_PATCHLIST_24
    , CONTROL_POINT_PATCHLIST_25
    , CONTROL_POINT_PATCHLIST_26
    , CONTROL_POINT_PATCHLIST_27
    , CONTROL_POINT_PATCHLIST_28
    , CONTROL_POINT_PATCHLIST_29
    , CONTROL_POINT_PATCHLIST_30
    , CONTROL_POINT_PATCHLIST_31
    , CONTROL_POINT_PATCHLIST_32
};

enum class BufferUsage : unsigned int {
    GPU
    , STATIC
    , DYNAMIC
    , STAGING
};

enum class BufferBindUsage : unsigned int {
    VERTEX_BUFFER = 0x001
    , INDEX_BUFFER = 0x002
    , CONSTANT_BUFFER = 0x004
    , SHADER_RESOURCE = 0x008
    , STREAM_OUTPUT = 0x010
    , RENDER_TARGET = 0x020
    , DEPTH_STENCIL = 0x040
    , UNORDERED_ACCESS = 0x080
    , DECODER = 0x100
    , VIDEO_ENCODER = 0x200
};

BufferBindUsage operator|(const BufferBindUsage& a, const BufferBindUsage& b);
BufferBindUsage operator&(const BufferBindUsage& a, const BufferBindUsage& b);

enum class FilterMode {
    POINT
    , LINEAR
    , ANISOTROPIC
};

enum class FilterComparisonMode {
    NONE
    , MINIMUM
    , MAXIMUM
    , COMPARISON
};

enum class TextureAddressMode {
    WRAP
    , MIRROR
    , CLAMP
    , BORDER
    , MIRROR_ONCE
};

enum class BlendFactor {
    ZERO,
    ONE,
    SRC_COLOR,
    INV_SRC_COLOR,
    SRC_ALPHA,
    INV_SRC_ALPHA,
    DEST_ALPHA,
    INV_DEST_ALPHA,
    DEST_COLOR,
    INV_DEST_COLOR,
    SRC_ALPHA_SAT,
    BLEND_FACTOR,
    INV_BLEND_FACTOR,
    SRC1_COLOR,
    INV_SRC1_COLOR,
    SRC1_ALPHA,
    INV_SRC1_ALPHA,
};

enum class BlendOperation {
    ADD,
    SUBTRACT,
    REVERSE_SUBTRACT,
    MIN,
    MAX,
};

enum class BlendColorWriteEnable : unsigned char {
    NONE = 0x00
    , RED = 0x01
    , GREEN = 0x02
    , BLUE = 0x04
    , ALPHA = 0x08
    , ALL = 0x0F
};

BlendColorWriteEnable operator~(const BlendColorWriteEnable& a);
BlendColorWriteEnable operator|(const BlendColorWriteEnable& a, const BlendColorWriteEnable& b);
BlendColorWriteEnable operator&(const BlendColorWriteEnable& a, const BlendColorWriteEnable& b);
BlendColorWriteEnable operator^(const BlendColorWriteEnable& a, const BlendColorWriteEnable& b);
BlendColorWriteEnable& operator&=(BlendColorWriteEnable& a, const BlendColorWriteEnable& b);
BlendColorWriteEnable& operator|=(BlendColorWriteEnable& a, const BlendColorWriteEnable& b);
BlendColorWriteEnable& operator^=(BlendColorWriteEnable& a, const BlendColorWriteEnable& b);

enum class FillMode {
    SOLID,
    WIREFRAME,
};

enum class CullMode {
    NONE,
    FRONT,
    BACK,
    BOTH,
};

enum class ResourceMiscFlag : long {
     GENERATE_MIPS                   = 0x1L
    ,SHARED                          = 0x2L
    ,TEXTURECUBE                     = 0x4L
    ,DRAWINDIRECT_ARGS               = 0x10L
    ,ALLOW_RAW_VIEWS                 = 0x20L
    ,STRUCTURED_BUFFER               = 0x40L
    ,RESOURCE_CLAMP                  = 0x80L
    ,SHARED_KEYEDMUTEX               = 0x100L
    ,GDI_COMPATIBLE                  = 0x200L
    ,SHARED_NTHANDLE                 = 0x800L
    ,RESTRICTED_CONTENT              = 0x1000L
    ,RESTRICT_SHARED_RESOURCE        = 0x2000L
    ,RESTRICT_SHARED_RESOURCE_DRIVER = 0x4000L
    ,GUARDED                         = 0x8000L
    ,TILE_POOL                       = 0x20000L
    ,TILED                           = 0x40000L
    ,HW_PROTECTED                    = 0x80000L
};

ResourceMiscFlag operator|(const ResourceMiscFlag& a, const ResourceMiscFlag& b);
ResourceMiscFlag operator&(const ResourceMiscFlag& a, const ResourceMiscFlag& b);
