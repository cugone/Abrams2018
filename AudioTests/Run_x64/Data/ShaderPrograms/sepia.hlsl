
cbuffer matrix_cb : register(b0) {
    float4x4 g_MODEL;
    float4x4 g_VIEW;
    float4x4 g_PROJECTION;
};

cbuffer time_cb : register(b1) {
    float g_GAME_TIME;
    float g_SYSTEM_TIME;
    float g_GAME_FRAME_TIME;
    float g_SYSTEM_FRAME_TIME;
}

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


float4 PixelFunction(ps_in_t input) : SV_Target0 {
   float4 diffuse = tDiffuse.Sample( sSampler, input.uv );
   float3x3 sepia_transform = float3x3( 
      float3( 0.393f, 0.349f, 0.272f ),
      float3( 0.769f, 0.686f, 0.534f ),
      float3( 0.189f, 0.168f, 0.131f )
   );
   float3 sepia = mul(diffuse.xyz, sepia_transform);
   float4 final_color = float4( sepia, diffuse.a);
   return final_color;
}