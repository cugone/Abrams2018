// Texture - I put the <float4> at the end to really drive home this is a 4-channle texture
Texture2D <float4> tImage : register(t0);

// SamplerState - HOW do we sample the texture.  
SamplerState sSampler : register(s0);

// Input to vertex Shader
struct vs_input_t {
    float3 position : POSITION;

    // NEW - UV Coordinate - textures are sampled using UV coordinates, which are coordinates that 
    // span [0,1] over the whole of the texture.
    // In D3D, <0,0> corresponds to to the TOP LEFT
    //    and  <1,1> corresponds to the BOTTOM RIGHT 
    float2 uv : TEXCOORD;
};

struct v2f_t {
    // SV_POSITION is a semantic - or a name identifying this variable. 
    // Usually a semantic can be any name we want - but the SV_* family
    // usually denote special meaning (SV = System Variable I believe)
    // SV_POSITION denotes that this is output in clip space, and will be 
    // use for rasterization.  When it is the input (pixel shader stage), it will
    // actually hold the pixel coordinates.
    float4 position : SV_Position;

    // We can only pass data to the vertex shader - so to get to the fragment shader
    // we must pass it through.
    float2 uv : TEXCOORD;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
v2f_t VertexFunction(vs_input_t input) {
    v2f_t v2f = (v2f_t)0;

    // The output of a vertex shader is in clip-space, which is a 4D vector
    // so we need to convert out input to a 4D vector.
    v2f.position = float4(input.position, 1.0f);

    // do nothing but pass it through/
    v2f.uv = input.uv;

    // And return - this will pass it on to the next stage in the pipeline;
    return v2f;
}

//See https://en.wikipedia.org/wiki/Grayscale#Converting_color_to_grayscale
float4 GetLinearIntensity(float4 color) {
    float4 result;
    result.r = GetLinearIntensity(color.r);
    result.g = GetLinearIntensity(color.g);
    result.b = GetLinearIntensity(color.b);
    result.a = GetLinearIntensity(color.a);
    return result;
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
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 FragmentFunction(v2f_t input) : SV_Target0 {
    float4 diffuse = tImage.Sample(sSampler, input.uv);
    float3 lin = GetLinearIntensity(diffuse.rgb);
    float3 coefs = float3(0.2126f, 0.7152f, 0.0722f);
    return float4(saturate(dot(coefs, lin)), 1.0f);
}