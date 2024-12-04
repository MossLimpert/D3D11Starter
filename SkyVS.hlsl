cbuffer ExternalData : register(b0)
{
    matrix view;
    matrix projection;
}

struct VertexToPixel
{
    float4 position : SV_POSITION;
    float3 sampleDir : DIRECTION;
};

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


VertexToPixel main( VertexShaderInput input )
{
    VertexToPixel output;
    
    // modify view by removing the translation
    matrix viewNoTrans = view;
    viewNoTrans._14 = 0;
    viewNoTrans._24 = 0;
    viewNoTrans._34 = 0;
    
    // multiply view and projection
    matrix vp = mul(projection, viewNoTrans);
    output.position = mul(vp, float4(input.localPosition, 1.0f));
    
    // clamp sky verts to the far clip plane
    // by setting z to w, because xyz is divided by w in the rasterizer
    output.position.z = output.position.w;
    
    // use pos as the sample direction for the cube map
    output.sampleDir = input.localPosition;
    
	return output;
}