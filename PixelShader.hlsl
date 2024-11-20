#include "include.hlsli"

#define NUM_LIGHTS 6

cbuffer ExternalData : register(b0)
{
    float roughness;
    float3 colorTint;
    float3 camPos;
    float3 ambient;
    
    Light lights[NUM_LIGHTS];
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
	// make sure all the normals are normalized
    input.normal = normalize(input.normal);
    
    // get the ambient color
    float3 totalLight = ambient * colorTint;
    
    // loop through lights and apply
    for (int i = 0; i < NUM_LIGHTS; i++)
    {
        // get light and normalize its direction
        Light light = lights[i];
        light.direction = normalize(light.direction);
        
        switch (lights[i].type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                totalLight += DirectionalLight(light, input.normal, input.worldPosition, camPos, roughness, colorTint);
                break;
            
            case LIGHT_TYPE_POINT:
                totalLight += PointLight(light, input.normal, input.worldPosition, camPos, roughness, colorTint);
                break;
            
            case LIGHT_TYPE_SPOT:
                totalLight += SpotLight(light, input.normal, input.worldPosition, camPos, roughness, colorTint);
                break;
        }
    }
    
    return float4(totalLight, 1);
}