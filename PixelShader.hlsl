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
    
}

// FIELDS

Texture2D SurfaceTexture    : register(t0); // t is registers for textures
SamplerState BasicSampler   : register(s0); // s is registers for samplers
Texture2D SpecularMap       : register(t1);

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
    
    // uv
    input.uv = input.uv * uvScale + uvOffset;
    
    // get the texture color at given uv coords, apply tint and ambient
    float3 curColor = SurfaceTexture.Sample(BasicSampler, input.uv).rgb;
    curColor *= colorTint;
    
    // get specular value for this pixel
    float specular = SpecularMap.Sample(BasicSampler, input.uv).r;
    
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
                totalLight += DirectionalLight(light, input.normal, input.worldPosition, camPos, roughness, colorTint, specular);
                break;
            
            case LIGHT_TYPE_POINT:
                totalLight += PointLight(light, input.normal, input.worldPosition, camPos, roughness, colorTint, specular);
                break;
            
            case LIGHT_TYPE_SPOT:
                totalLight += SpotLight(light, input.normal, input.worldPosition, camPos, roughness, colorTint, specular);
                break;
        }
    }
    
    return float4(totalLight, 1);
}