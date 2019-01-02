
cbuffer matrix_cb : register(b0) {
    float4x4 g_MODEL;
    float4x4 g_VIEW;
    float4x4 g_PROJECTION;
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

SamplerState sSampler : register(s0);

Texture2D<float4> tDiffuse    : register(t0);
Texture2D<float4> tNormal   : register(t1);
Texture2D<float4> tDisplacement : register(t2);
Texture2D<float4> tSpecular : register(t3);
Texture2D<float4> tOcclusion : register(t4);
Texture2D<float4> tEmissive : register(t5);


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

//See https://en.wikipedia.org/wiki/Grayscale#Converting_color_to_grayscale
float GetLinearIntensity(float channel) {
    float result = 0.0f;
    float LUM_BREAKPOINT = 0.04045f;
    channel = saturate(channel);
    if(channel > LUM_BREAKPOINT) {
        result = pow((channel + 0.055f) / 1.055f, 2.4f);
    } else {
        result = channel / 12.92f;
    }
    return result;
}

float4 PixelFunction(ps_in_t input) : SV_Target0 {
    float4 diffuse = tDiffuse.Sample(sSampler, input.uv);
    float r_lin = GetLinearIntensity(diffuse.r);
    float g_lin = GetLinearIntensity(diffuse.g);
    float b_lin = GetLinearIntensity(diffuse.b);
    float l = 0.2126f * r_lin + 0.7152f * g_lin + 0.0722f * b_lin;
    return float4(l, l, l, 1.0f);
}