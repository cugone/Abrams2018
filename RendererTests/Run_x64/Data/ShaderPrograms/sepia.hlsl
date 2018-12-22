//--------------------------------------------------------------------------------------
struct vs_input_t
{
   float3 position : POSITION;
   float2 uv0 : TEXCOORD0;

   // float4x4 test_mat : SOME_MATRIX;
};

//--------------------------------------------------------------------------------------
struct v2f_t
{
   float4 position : SV_Position;
   float2 uv : TEXCOORD;
};

Texture2D <float4> tTexture : register(t0);
SamplerState sSampler : register(s0);

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
v2f_t VertexFunction(vs_input_t input)
{
    v2f_t v2f = (v2f_t)0;

    float4 final_pos = float4( input.position, 1.0f );

    // Change the position vector to be 4 units for proper matrix calculations.
    v2f.position = final_pos;
    v2f.uv = input.uv0;

    return v2f;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 FragmentFunction(v2f_t input) : SV_Target0 // semeantic of what I'm returning
{
   float4 diffuse = tTexture.Sample( sSampler, input.uv );
   
   /* EXPLICIT WAY */
   float3 sepia_r = float3( 0.393f, 0.769f, 0.189f );
   float3 sepia_g = float3( 0.349f, 0.686f, 0.168f );
   float3 sepia_b = float3( 0.272f, 0.534f, 0.131f );
   float3 sepia = float3( 
      dot( diffuse.xyz, sepia_r ),
      dot( diffuse.xyz, sepia_g ),
      dot( diffuse.xyz, sepia_b )
   );
   /**/
   
   /* MATRIX WAY *
   float3x3 sepia_transform = float3x3( 
      float3( 0.393f, 0.349f, 0.272f ),
      float3( 0.769f, 0.686f, 0.534f ),
      float3( 0.189f, 0.168f, 0.131f )
   );
   float3 sepia = mul( diffuse.xyz, sepia_transform );
   /**/ 

   float4 final_color = float4( sepia, 1.0f );
   return final_color;
}