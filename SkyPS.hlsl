struct VertexToPixel
{
    float4 position : SV_POSITION;
    float3 sampleDir : DIRECTION;
};

// Texture resources
TextureCube SkyTexture		: register(t0);
SamplerState BasicSampler	: register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
    // instead of a uv coord, a cube map takes a float3 direction
    return SkyTexture.Sample(BasicSampler, input.sampleDir);
}