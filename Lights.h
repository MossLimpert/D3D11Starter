#pragma once

#include <DirectXMath.h>

#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2

struct Light {

	int type;						// use definitions above
	DirectX::XMFLOAT3 direction;	// directional and spot need direction
	float range;					// point and spot have max range
	DirectX::XMFLOAT3 position;		// point and spot have a position	
	float intensity;				// all need intensity
	DirectX::XMFLOAT3 color;		// all need color
	float spotInnerAngle;			// inside this full light
	float spotOuterAngle;			// outside this no light
	int castsShadows;				// 0 false 1 true
	float pad;
};

struct ShadowOptions {
	int resolution;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> shadowDSV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shadowSRV;

	float projectionSize;
	DirectX::XMFLOAT4X4 shadowViewMatrix;
	DirectX::XMFLOAT4X4 shadowProjectionMatrix;
};