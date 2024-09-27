#pragma once

#include <DirectXMath.h>

class Transform
{

	bool dirty; // tracks if any of the world matrix's components have changed this frame
	DirectX::XMVECTOR position;
	DirectX::XMVECTOR rotation;	// pitch, yaw, roll stored as x, y, z
	DirectX::XMVECTOR scale;
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX worldInverseTranspose;

public: 
	Transform();				// constructor
	~Transform();				// destructor
	Transform(Transform& t);	// copy constructor

	DirectX::XMMATRIX GetWorldMatrix();
};

