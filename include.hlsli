#ifndef __GGP_SHADER_INCLUDES__ // hlsli unique identifier

#define __GGP_SHADER_INCLUDES__
#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2
#define MAX_SPECULAR_EXPONENT 256.0f


struct Light
{

    int type;               // use definitions above
    float3 direction;       // directional and spot need direction
    float range;            // point and spot have max range
    float3 position;        // point and spot have a position	
    float intensity;        // all need intensity
    float3 color;           // all need color
    float spotInnerAngle;   // inside this full light
    float spotOuterAngle;   // outside this no light
    float2 padding;         // adding padding to hit the 16byte bound
	
};

struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float4 screenPosition : SV_POSITION;
    float3 normal : NORMAL;
    float3 worldPosition : POSITION;
    float2 uv : TEXCOORD;
};

// Struct representing a single vertex worth of data
// - This should match the vertex definition in our C++ code
// - By "match", I mean the size, order and number of members
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexShaderInput
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float3 localPosition : POSITION; // XYZ position
    float3 normal : NORMAL; // normal 
    float2 uv : TEXCOORD; // uv
};

// helper functions referenced from demo
//
//

// range based attenuation
float Attenuate(Light light, float3 worldPos)
{
    // distance between surface and light
    float dist = distance(light.position, worldPos);
    
    // ranged-based attenuation
    float att = saturate(1.0f - (dist * dist / (light.range * light.range)));
    
    // soft falloff
    return att * att;
}

// lambert diffuse Bidirectional reflectance distribution function
float Diffuse(float3 normal, float3 dirToLight)
{
    return saturate(dot(normal, dirToLight));
}

// phong (specular) Bidirectional reflectance distribution function
float SpecularPhong(float3 normal, float3 dirToLight, float3 toCamera, float roughness)
{
    // reflection vector
    float3 refl = reflect(-dirToLight, normal);
    
    // compare reflection and view, raise to a power
    return roughness == 1 ? 0.0f : pow(max(dot(toCamera, refl), 0), (1 - roughness) * MAX_SPECULAR_EXPONENT);
}

// Blinn phong Bidirectional reflectance distribution function
float SpecularBlinnPhong(float3 normal, float3 dirToLight, float3 toCamera, float roughness)
{
    // halfway vector
    float3 halfwayVector = normalize(dirToLight + toCamera);
    
    // compare halfway vector and normal and raise to a power
    return roughness == 1 ? 0.0f : pow(max(dot(halfwayVector, normal), 0), (1 - roughness) * MAX_SPECULAR_EXPONENT);
}

// directional light calculation
float3 DirectionalLight(Light light, float3 normal, float3 worldPos, float3 camPos, float roughness, float3 surfaceColor, float specular)
{
    // get the normalized direction towards the light
    float3 toLight = normalize(-light.direction);
    float3 toCam = normalize(camPos - worldPos);
    
    // calculate the amount of diffuse and specular light, taking
    // roughness into account for specular
    float diff = Diffuse(normal, toLight);
    float spec = SpecularPhong(normal, toLight, toCam, roughness) * specular;
    
    // combine
    return (diff * surfaceColor + spec) * light.intensity * light.color;

}

// point light equation
float3 PointLight(Light light, float3 normal, float3 worldPos, float3 camPos, float roughness, float3 surfaceColor, float specular)
{
    // light direction
    float3 toLight = normalize(light.position - worldPos);
    float3 toCam = normalize(camPos - worldPos);
    
    // calculate light amounts
    float atten = Attenuate(light, worldPos);
    float diff = Diffuse(normal, toLight);
    float spec = SpecularPhong(normal, toLight, toCam, roughness) * specular;
    
    // combine
    return (diff * surfaceColor + spec) * atten * light.intensity * light.color;
}

// spot light equation
float3 SpotLight(Light light, float3 normal, float3 worldPos, float3 camPos, float roughness, float3 surfaceColor, float specular)
{
    // spot falloff
    float3 toLight = normalize(light.position - worldPos);
    float pixelAngle = saturate(dot(-toLight, light.direction));
    
    // spot angle is the cosine of the angle from the dot product
    // we need the cosine of these angles
    float cosOuter = cos(light.spotOuterAngle);
    float cosInner = cos(light.spotInnerAngle);
    float falloffRange = cosOuter - cosInner;
    
    // linear falloff as pixel approaches outer range
    float spotTerm = saturate((cosOuter - pixelAngle) / falloffRange);
    
    // combine with point light calc
    // idgaf about optimization ;9
    return PointLight(light, normal, worldPos, camPos, roughness, surfaceColor, specular) * spotTerm;
}

#endif