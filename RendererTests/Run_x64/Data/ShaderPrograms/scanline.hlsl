Texture2D <float4> tImage : register(t0);

SamplerState texsampler : register(s0);

struct vertex_in_t {
    float3 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

struct vertex_to_fragment_t {
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    //float3 world_position : WORLD_POSITION;
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

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
vertex_to_fragment_t VertexFunction(vertex_in_t input) {
    vertex_to_fragment_t v2f = (vertex_to_fragment_t)0;

    // The output of a vertex shader is in clip-space, which is a 4D vector
    // so we need to convert out input to a 4D vector.
    v2f.position = float4(input.position, 1.0f);

    v2f.color = input.color;
    // do nothing but pass it through/
    v2f.uv = input.uv;

    // And return - this will pass it on to the next stage in the pipeline;
    return v2f;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
// If I'm only returning one value, I can optionally just mark the return value with
// a SEMANTIC - in this case, SV_TARGET0, which means it is outputting to the first colour 
// target.
float4 PixelFunction(vertex_to_fragment_t input) : SV_Target0 // semeantic of what I'm returning
{
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