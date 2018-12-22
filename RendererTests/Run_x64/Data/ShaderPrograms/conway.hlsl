
cbuffer matrix_cb : register(b0) {
    float4x4 g_MODEL;
    float4x4 g_VIEW;
    float4x4 g_PROJECTION;
};

cbuffer time_cb : register(b1) {
    float g_gametime;
    float g_systemtime;
    float g_gameframetime;
    float g_systemframetime;
}

struct light_t {
    float4 position;
    float4 color;
    float4 attenuation;
    float4 specAttenuation;
    float4 innerOuterDotThresholds;
    float4 direction;
};

cbuffer lighting_cb : register(b2) {
    light_t lights[16];
    float4 ambient;
    float4 specular_glossy_emissive_factors;
    float4 eye_position;
    int useVertexNormals;
    float3 padding;
}

cbuffer conway_constants : register(b3) {
    float4 live_color;
    float4 dead_color;
};

struct vs_in_t {
    float3 position : POSITION;
    float4 color : COLOR;
    float2 uv : UV;
};

struct ps_in_t {
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : UV;
};

// https://msdn.microsoft.com/en-us/library/windows/desktop/ff471566(v=vs.85).asxyp
struct compute_input_t {
   uint3 group_id : SV_GroupID;              // What is my index within the context of a the dispatch call
   uint  group_idx : SV_GroupIndex;          // Flattend version of the top 
                                             // (z * (width * height) + y * width + x)
   uint3 group_thread_id : SV_GroupThreadID; // My thread within the context of the group
   uint3 dispatch_id : SV_DispatchThreadID;  // My thread within context of the entire dispatch 
                                             // [5x5x1, with threads 5,5,1]  If I was in 2, 2, 0 group 
                                             // and 3, 3, 0 thread, I would be in 13 13 0 thread id
}; 

SamplerState sSampler : register(s0);

Texture2D<float4> tImage    : register(t0);
Texture2D<float4> tNormal   : register(t1);
Texture2D<float4> tDisplacement : register(t2);
Texture2D<float4> tSpecular : register(t3);
Texture2D<float4> tOcclusion : register(t4);
Texture2D<float4> tEmissive : register(t5);
Texture2D<float4> tLinear : register(t6);

RWTexture2D<float4> tOutput;

ps_in_t VertexFunction(vs_in_t input_vertex) {
    ps_in_t output;

    float4 local = float4(input_vertex.position, 1.0f);
    float4 world = mul(local, g_MODEL);
    float4 view = mul(world, g_VIEW);
    float4 clip = mul(view, g_PROJECTION);

    output.position = clip;
    output.color = input_vertex.color;
    output.uv = input_vertex.uv;

    return output;
}

float4 PixelFunction(ps_in_t input_pixel) : SV_Target0{
    float4 albedo = tImage.Sample(sSampler, input_pixel.uv);
    return albedo;
}

float RangeMap(float valueToMap, const float minInputRange, const float maxInputRange, const float minOutputRange, const float maxOutputRange) {
    return (valueToMap - minInputRange) * (maxOutputRange - minOutputRange) / (maxInputRange - minInputRange) + minOutputRange;
}

float3 GetLinearIntensity(float3 color) {
    float3 result;
    result.r = GetLinearIntensity(color.r);
    result.g = GetLinearIntensity(color.g);
    result.b = GetLinearIntensity(color.b);
    return result;
}

float2 GetLinearIntensity(float2 color) {
    float2 result;
    result.r = GetLinearIntensity(color.r);
    result.g = GetLinearIntensity(color.g);
    return result;
}

float GetLinearIntensity(float channel) {
    float result = 0.0f;
    float LUM_BREAKPOINT = 0.04045f;
    channel = saturate(channel);
    if(channel > LUM_BREAKPOINT) {
        result = pow((channel + 0.055f) / 1.055f, 2.4);
    } else {
        result = channel / 12.92f;
    }
    return result;
}

//--------------------------------------------------------------------------------------
// Compute Shader
//--------------------------------------------------------------------------------------
[numthreads(16, 16, 1)]
void ComputeFunction( compute_input_t input ) {
    float x = input.dispatch_id.x;
    float y = input.dispatch_id.y;
    uint w = 0;
    uint h = 0;
    tOutput.GetDimensions(w, h);
    
    uint2 up = uint2(x,y - 1);
    uint2 upleft = uint2(x - 1,y - 1);
    uint2 upright = uint2(x + 1,y - 1);
    uint2 down = uint2(x, y + 1);
    uint2 downleft = uint2(x - 1, y + 1);
    uint2 downright = uint2(x + 1, y + 1);
    uint2 left = uint2(x - 1, y);
    uint2 center = uint2(x, y);
    uint2 right = uint2(x + 1, y);

    float upleft_pixel = GetLinearIntensity(tOutput[upleft]).r;
    float up_pixel = GetLinearIntensity(tOutput[up]).r;
    float upright_pixel = GetLinearIntensity(tOutput[upright]).r;
    float left_pixel = GetLinearIntensity(tOutput[left]).r;
    float pixel = GetLinearIntensity(tOutput[uint2(x, y)]).r;
    float right_pixel = GetLinearIntensity(tOutput[right]).r;
    float downleft_pixel = GetLinearIntensity(tOutput[downleft]).r;
    float down_pixel = GetLinearIntensity(tOutput[down]).r;
    float downright_pixel = GetLinearIntensity(tOutput[downright]).r;

    float neighbors = upleft_pixel + up_pixel + upright_pixel +
                       left_pixel   + right_pixel +
                       downleft_pixel + down_pixel + downright_pixel;
    if(2.5f < neighbors && neighbors > 3.5f) {
        tOutput[center] = dead_color;
    } else {
        tOutput[center] = live_color;
    }
}

