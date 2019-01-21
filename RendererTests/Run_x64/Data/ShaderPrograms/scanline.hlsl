Texture2D <float4> tImage : register(t0);

SamplerState texsampler : register(s0);

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

cbuffer matrix_cb : register(b0) {
    float4x4 g_MODEL;
    float4x4 g_VIEW;
    float4x4 g_PROJECTION;
};

cbuffer time_cb : register(b1) {
    float GAME_TIME;
    float SYSTEM_TIME;
    float GAME_FRAME_TIME;
    float SYSTEM_FRAME_TIME;
}

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

float4 PixelFunction(ps_in_t input) : SV_Target0 {
    float width = 1.0f;
    float height = 1.0f;
    tImage.GetDimensions(width, height);
    float t = frac(GAME_TIME);

    // Sample the texture at the passed in UV coordinate
    float4 diffuse = tImage.Sample(texsampler, input.uv);
    float4 black = float4(0.0, 0.0, 0.0, 1.0);
    float4 white = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float4 green = float4(0.0f, 1.0f, 0.0f, 1.0f);
    float s = 1.0f + sin((t * 0.001f + input.uv.y) * 0.5f * height) * 0.50f * height;
    if(s < 1.0f) { //Even pixels
        return float4(diffuse.rgb * 0.7f, 1.0f) * green;
    } else {
        return diffuse * green;
    }
}