#pragma once

#include <DirectXMath.h>

class Transform
{
public: 
	// RULE OF 3
	Transform();				// constructor
	~Transform();				// destructor
	Transform(Transform& t);	// copy constructor

	// SETTERS
	void SetPosition(float _x, float _y, float _z);
	void SetPosition(DirectX::XMFLOAT3 _pos);
	void SetRotation(float _pitch, float _yaw, float _roll);
	void SetRotation(DirectX::XMFLOAT3 _rot);
	void SetScale(float _x, float _y, float _z);
	void SetScale(DirectX::XMFLOAT3 _scale);

	// GETTERS
	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetPitchYawRoll();
	DirectX::XMFLOAT3 GetScale();
	DirectX::XMFLOAT4X4 GetWorldMatrix();
	DirectX::XMFLOAT4X4 GetInverseTransposeWorldMatrix();
	DirectX::XMFLOAT3 GetRight();
	DirectX::XMFLOAT3 GetUp();
	DirectX::XMFLOAT3 GetForward();


	// TRANSFORMERS
	void MoveAbsolute(float _x, float _y, float _z);
	void MoveAbsolute(DirectX::XMFLOAT3 offset);
	void Rotate(float _pitch, float _yaw, float _roll);
	void Rotate(DirectX::XMFLOAT3 _rotation);
	void Scale(float _x, float _y, float _z);
	void Scale(DirectX::XMFLOAT3 _scale);
	void MoveRelative(float _x, float _y, float _z);
	void MoveRelative(DirectX::XMFLOAT3 offset);

private:
	bool dirty; // tracks if any of the world matrix's components have changed this frame
	
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 rotation;	// pitch, yaw, roll stored as x, y, z
	DirectX::XMFLOAT3 scale;
	
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 worldInverseTranspose;

	
};

