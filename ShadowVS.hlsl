#include "include.hlsli"

// light view and projection, entity's world
cbuffer ExternalData : register(b0) {
	matrix world;
	matrix view;
	matrix projection;
}

float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}