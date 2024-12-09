#include "include.hlsli"

#define NUM_LIGHTS 6

cbuffer ExternalData : register(b0)
{
    Light lights[NUM_LIGHTS];
    float3 ambient;
    float3 colorTint;
    float3 camPos;
    
    float roughness;
    
    float2 uvScale;
    float2 uvOffset;
    int useSpecularMap;
    
}

// FIELDS

// assignment 11
Texture2D Albedo : register(t0);
Texture2D NormalMap : register(t1);
Texture2D RoughnessMap : register(t2);
Texture2D MetalnessMap : register(t3);

//Texture2D SurfaceTexture    : register(t0); // t is registers for textures
SamplerState BasicSampler   : register(s0); // s is registers for samplers
//Texture2D SpecularMap       : register(t1);
//Texture2D NormalMap         : register(t2);

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
	// make sure all the normals are normalized
    input.normal = normalize(input.normal);
    input.tangent = normalize(input.tangent);
    
    // uv
    input.uv = input.uv * uvScale + uvOffset;
    
    // normal
    // get the normal map normal
    float3 unpackedNormal = NormalMap.Sample(BasicSampler, input.uv).rgb * 2 - 1;
    unpackedNormal = normalize(unpackedNormal);
    // create the tbn matrix
    float3 N = input.normal; // its already normalized above
    float3 T = normalize(input.tangent);
    T = normalize(T - N * dot(T, N));
    float3 B = cross(T, N);
    float3x3 TBN = float3x3(T, B, N);
    // transform the unpacked normal
    input.normal = mul(unpackedNormal, TBN); // multiplication order important
    
    
    // assignment 11 
    // roughness
    float roughness = RoughnessMap.Sample(BasicSampler, input.uv).r;
    // metalness
    float metalness = MetalnessMap.Sample(BasicSampler, input.uv).r;
    
    // get the texture color at given uv coords, apply tint and ambient
    // assignment 11
    // undoing gamma correction (added back on final line in main)
    float3 curColor = pow(Albedo.Sample(BasicSampler, input.uv).rgb, 2.2f);
    curColor *= colorTint;
    
    
    // assignment 11
    // specular 
    float3 specColor = lerp(F0_NON_METAL, curColor.rgb, metalness);
    
    float3 totalLight = ambient * curColor;
    
    // loop through lights and apply
    for (int i = 0; i < NUM_LIGHTS; i++)
    {
        // get light and normalize its direction
        Light light = lights[i];
        light.direction = normalize(light.direction);
        
        switch (lights[i].type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                totalLight += DirectionalLight(light, input.normal, input.worldPosition, camPos, roughness, metalness, colorTint, specColor);
                break;
            
            case LIGHT_TYPE_POINT:
                totalLight += PointLight(light, input.normal, input.worldPosition, camPos, roughness, metalness, colorTint, specColor);
                break;
            
            case LIGHT_TYPE_SPOT:
                totalLight += SpotLight(light, input.normal, input.worldPosition, camPos, roughness, metalness, colorTint, specColor);
                break;
        }
    }
    
    // with gamma correction
    return float4(pow(totalLight, 1.0f / 2.2f), 1);
}