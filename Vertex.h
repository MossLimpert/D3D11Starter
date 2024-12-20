#pragma once

#include <DirectXMath.h>

// --------------------------------------------------------
// A custom vertex definition
//
// You will eventually ADD TO this, and/or make more of these!
// --------------------------------------------------------
struct Vertex
{
	DirectX::XMFLOAT3 Position;	    // The local position of the vertex
	DirectX::XMFLOAT3 Normal;       // The color of the vertex
	DirectX::XMFLOAT3 Tangent;		// tangent oriented to U of uv
	DirectX::XMFLOAT2 UV;			// uv texture coordinate
};