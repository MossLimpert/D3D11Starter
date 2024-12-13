#ifndef __GGP_SHADER_INCLUDES__ // hlsli unique identifier

#define __GGP_SHADER_INCLUDES__
#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2
#define MAX_SPECULAR_EXPONENT 256.0f

// constant Fresnel value for non-metals (glass and plastic are about 0.04)
static const float F0_NON_METAL = 0.04f;

// minimum roughness, for when specular distrib func denominator goes to 0
static const float MIN_ROUGHNESS = 0.0000001f;

static const float PI = 3.14159265359f;

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
    int castsShadows;
    float pad;
	
};

struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float4 screenPosition   : SV_POSITION;
    float3 normal           : NORMAL;
    float3 worldPosition    : POSITION;
    float3 tangent          : TANGENT;
    float2 uv               : TEXCOORD;
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
    float3 localPosition    : POSITION; // XYZ position
    float3 normal           : NORMAL; // normal 
    float3 tangent          : TANGENT;
    float2 uv               : TEXCOORD; // uv
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



//
//
// PBR FUNCTIONS

// lambert diffuse brdf. assumes vectors are NORMALIZED!
float DiffusePBR(float3 normal, float3 dirToLight)
{
    return saturate(dot(normal, dirToLight));
}

// calculates diffuse based on energy conservation
// diffuse      - diffuse amount
// f            - fresnel result from microfacet brdf
// metalness    - metalness amt
float3 DiffuseEnergyConserve(float3 diffuse, float3 F, float metalness)
{
    return diffuse * (1 - F) * (1 - metalness);
}

// normal distribution function
// ggx Trowbridge-Reitz
// a - roughness  
// h - half vector
// n - normal
// D(h , n, a) = a^2 / pi * ((n dot h)^2 * (a^2 - 1) + 1)^2
float D_GGX(float3 n, float3 h, float3 roughness)
{
    // pre calculation work
    float NdotH = saturate(dot(n, h));
    float NdotH2 = NdotH * NdotH;
    float a = roughness * roughness;
    float a2 = max(a * a, MIN_ROUGHNESS);
    
    // ((n dot h)^2 * (a^2 - 1) + 1)
    // can go to 0 if roughness is 0 and ndoth is 1, use MIN_ROUGHNESS
    float denomToSquare = NdotH2 * (a2 - 1) + 1;
    
    return a2 / (PI * denomToSquare * denomToSquare);
}

// Fresnel term - Schlick approximation
// v - view vector
// h - half vector
// f0 - value when 1 = n
// F(v , h, f0) = f0 + (1-F0)(1- (v dot h))^5
float3 F_Schlick(float3 v, float3 h, float3 f0)
{
    // pre calc
    float VdotH = saturate(dot(v, h));
    
    return f0 + (1 - f0) * pow(1 - VdotH, 5);
}

// Geometric Shadowing - Schlick GGX 
// k becomes a / 2, roughness becomes (r + 1)/2 before squaring
// n - normal
// v - view vector
// G_Schlick(n, v, a) = (n dot v) / ((n dot v) * (1-k ) * k )
// full G(n, v, l, a) term = G_SchlickGGX(n,v,a) * G_SchlickGGX(n,l,a)
float G_SchlickGGX(float3 n, float3 v, float roughness)
{
    float k = pow(roughness + 1, 2) / 8.0f;
    float NdotV = saturate(dot(n, v));
    
    // numerator should be NdotV (or NdotL)
    // but they also are in BRDF denominator, so they cancel out
    // we leave them out here and in BRDF 
    // because if we leave them in the dot products get small and cause 
    // rounding errors
    return 1 / (NdotV * (1 - k) + k);
}

// Cook-torrance Microfacet BRDF (Specular)
// f(l,v) = D(h)F(v,h)G(l,v,h) / 4 (n dot l )(n dot v)
// parts of denominator are canceled out by numerator
// D() - normal distribution func Trowbridge-Reitz GGX
// F() - Fresnel - Schlick approximation
// G() - Geometric Shadowing - Schlick GGX 
float3 MicroFacetBRDF(float3 n, float3 l, float3 v, float roughness, float3 specColor, out float3 F_out)
{
    // other vectors
    float3 h = normalize(v + l);
    
    // run numerator funcs
    float D = D_GGX(n, h, roughness);
    float3 F = F_Schlick(v, h, specColor);
    float G = G_SchlickGGX(n, v, roughness) * G_SchlickGGX(n, l, roughness);
    
    // pass F out of the function for diffuse balance?
    F_out = F;
    
    // final formula
    
    // denominator *should* contain ndotv and ndotl but 
    // theyd be cancelled out by our G term, so they've been 
    // removed in both spots to prevent float rounding errors
    float3 specularResult = (D * F * G) / 4;
    
    // according to the rendering equation
    // specular must have the same ndotl applied as diffuse
    // apply that here
    return specularResult * max(dot(n, l), 0);
}

// directional light calculation
float3 DirectionalLight(Light light, float3 normal, float3 worldPos, float3 camPos, float roughness, float metalness, float3 surfaceColor, float3 specular)
{
    // get the normalized direction towards the light
    float3 toLight = normalize(-light.direction);
    float3 toCam = normalize(camPos - worldPos);
    
    // calculate the amount of diffuse and specular light, taking
    // roughness into account for specular
    float diff = Diffuse(normal, toLight);
    float3 F;
    float3 spec = MicroFacetBRDF(normal, toLight, toCam, roughness, specular, F);
    
    // calculate diffuse with energy conservation
    // reflected light does not get diffused
    float3 balancedDiff = DiffuseEnergyConserve(diff, F, metalness);
    
    //return balancedDiff;
    // combine
    return (balancedDiff * surfaceColor + spec) * light.intensity * light.color;

}

// point light equation
float3 PointLight(Light light, float3 normal, float3 worldPos, float3 camPos, float roughness, float metalness, float3 surfaceColor, float3 specular)
{
    // light direction
    float3 toLight = normalize(light.position - worldPos);
    float3 toCam = normalize(camPos - worldPos);
    
    // calculate light amounts
    float atten = Attenuate(light, worldPos);
    float diff = Diffuse(normal, toLight);
    float3 F;
    float3 spec = MicroFacetBRDF(normal, toLight, toCam, roughness, specular, F);
    
    // diffuse with energy conservation - reflected light doesnt diffuse
    float3 balancedDiff = DiffuseEnergyConserve(diff, F, metalness);
    
    // combine
    return (balancedDiff * surfaceColor + spec) * atten * light.intensity * light.color;
}

// spot light equation
float3 SpotLight(Light light, float3 normal, float3 worldPos, float3 camPos, float roughness, float metalness, float3 surfaceColor, float3 specular)
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
    return PointLight(light, normal, worldPos, camPos, roughness, metalness, surfaceColor, specular) * spotTerm;
}


#endif