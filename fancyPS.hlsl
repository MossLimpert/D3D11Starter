// perlin noise as in 
// https://thebookofshaders.com/edit.php#11/2d-snoise-clear.frag

cbuffer ExternalData : register(b0)
{
    float4 colorTint;
}

// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float4 screenPosition : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

// helper functions
float3 mod289(float3 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}
float2 mod289(float2 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}
float3 permute(float3 x)
{
    return mod289(((x * 34.0) + 1.0) * x);
}

// noise from uv
float perlinNoise(float2 uv)
{
	//skewed triangular grid
    const float4 C = float4(
	    0.211324865405187,
        // (3.0-sqrt(3.0))/6.0
        0.366025403784439,
        // 0.5*(sqrt(3.0)-1.0)
        -0.577350269189626,
        // -1.0 + 2.0 * C.x
        0.024390243902439
        // 1.0 / 41.0
    );
    
    //corner 1 x = 0
    float2 i = floor(uv + dot(uv, C.yy));
    float2 x0 = uv - 1 + dot(i, C.xx);
    
    // x 1 x 2
    float2 i1 = float2(0.0f, 0.0f);
    i1 = (x0.x > x0.y) ? float2(1.0, 0.0) : float2(0.0, 1.0);
    float2 x1 = x0.xy + C.xx - i1;
    float2 x2 = x0.xy + C.zz;
    
    // permutations, to avoid trunc effects
    i = mod289(i);
    float3 p = permute(
        permute(
            i.y + float3(0.0, i1.y, 1.0))
        + i.x + float3(0.0, i1.x, 1.0)
    );
    
    float3 m = max(
        0.5 - float3(
            dot(x0, x0),
            dot(x1, x1),
            dot(x2, x2)
        ), 0.0
    );
    
    m = m * m;
    m = m * m;
    
    //gradients
    float3 x = float3(1.0,1.0,1.0);
    //float3 x = 2.0 * frac(p * C.ww) - 1.0;
    float3 h = abs(x) - 0.5;
    float3 ox = floor(x + 0.5);
    float3 a0 = x - ox;
    
    // normalize gradients implicitly
    m *= 1.79284291400159 - 0.85373472095314 * (a0 * a0 + h * h);
    
    // compute final noise
    float3 g = float3(0.0, 0.0, 0.0);
    g.x = a0.x * x0.x + h.x * x0.y;
    g.yz = a0.yz * float2(x1.x, x2.x) + h.yz * float2(x1.y, x2.y);
    
    return 130.0 * dot(m, g);
}

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
    float3 reso = float3(1.0, 1.0, 1.0);
    float2 st = input.screenPosition.xy / reso.xy; // i dont have a resolution\
    st.x *= reso.x / reso.y;
    
    float3 color = float3(0.0, 0.0, 0.0);
    
    st *= 10;
    
    float meow = perlinNoise(st) * 0.5 + 0.5;
    color = float3(meow, meow, meow);
	
    return float4(color, 1.0);
}