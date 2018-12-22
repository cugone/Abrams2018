
cbuffer matrix_cb : register(b0) {
    float4x4 g_MODEL;
    float4x4 g_VIEW;
    float4x4 g_PROJECTION;
};

cbuffer MANDLEBROT_CONSTANTS : register(b3) {
    float4 ESCAPE_COLOR;
    float4 CONVERGE_COLOR;
    float2 MOUSE_DELTA;
    float2 PADDING;
    uint MAX_ITERATIONS;
    float POWER_SETTING;
    float ZOOM_SETTING;
    uint USE_ESCAPE_COLORING;
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

uint CheckMandlebrotSet(float x0, float y0) {
    float x = 0.0f;
    float y = 0.0f;
    uint iteration = 0;
    while ((((x * x) + (y * y)) < 4.0f) && (iteration < MAX_ITERATIONS))
    {
        //float xtemp = x * x - y * y + x0;
        //y = 2.0f * x * y + y0;
        //x = xtemp;            
        float z_mag = sqrt(x * x + y * y);
        float z_mag_pow = pow(abs(z_mag), POWER_SETTING);
        
        float theta = 0.0f;
        if (x != 0.0f)
        {
            theta = atan2(y, x);
        }

        float z_pow_cos = z_mag_pow * cos(POWER_SETTING * theta);
        float z_pow_sin = z_mag_pow * sin(POWER_SETTING * theta);

        x = z_pow_cos + x0;
        y = z_pow_sin + y0;

        iteration++;
    }
    //return 0;
    return iteration;
}

//Escape coloring from https://solarianprogrammer.com/2013/02/28/mandelbrot-set-cpp-11/
bool ImageGenerateMandlebrot(float x, float y, uint width, uint height) {
    tOutput[uint2(x, y)] = float4(1.0f, 0.0f, 1.0f, 1.0f);
    //float x0 = RangeMap((float) x, 0.0f, (float) width, -2.5f * ZOOM_SETTING + MOUSE_DELTA.x * ZOOM_SETTING, 1.0f * ZOOM_SETTING + MOUSE_DELTA.x * ZOOM_SETTING); //-2.5f, 1.0f
    //float y0 = RangeMap((float) y, 0.0f, (float) height, -1.0f * ZOOM_SETTING + MOUSE_DELTA.y * ZOOM_SETTING, 1.0f * ZOOM_SETTING + MOUSE_DELTA.y * ZOOM_SETTING); //-1.0f, 1.0f
    //uint iterations = CheckMandlebrotSet(x0, y0);

    ////Bernstien Distributions
    //if(USE_ESCAPE_COLORING == 1) {
    // 
    //    float t = (float) iterations / (float) MAX_ITERATIONS;

    //    float4 escape_color;
    //    escape_color.a = 1.0f;
    //    escape_color.r = (19.0f * (1.0f - t) * t * t * t);
    //    escape_color.g = (25.0f * (1.0f - t) * (1.0f - t) * t * t);
    //    escape_color.b = (18.5f * (1.0f - t) * (1.0f - t) * (1.0f - t) * t);
    //    tOutput[uint2(x, y)] = escape_color;

    //} else if (USE_ESCAPE_COLORING == 2) {
    //    
    //    float red_scale = 19.0f;
    //    float green_scale = 25.0f;
    //    float blue_scale = 18.5f;

    //    float t = (float) iterations / (float) MAX_ITERATIONS;

    //    float4 diffuse = tLinear.SampleLevel(sSampler, float2(t, 0.0f), 0);
    //    
    //    float4 escape_color = diffuse;
    //    tOutput[uint2(x, y)] = escape_color;

    //} else {
    //    if (iterations == MAX_ITERATIONS) {
    //        tOutput[uint2(x, y)] = ESCAPE_COLOR;
    //    } else {
    //        tOutput[uint2(x, y)] = CONVERGE_COLOR;
    //    }
    //}

    return true;
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
    ImageGenerateMandlebrot(x, y, w, h);
}

