#pragma once

#include "Transform.h"
#include <DirectXMath.h>
#include <memory>

enum Projection {
	PERSPECTIVE,
	ORTHOGRAPHIC
};

class Camera
{
public:
	Camera(float _aspectRatio, DirectX::XMFLOAT3 initPos, const char* _name);
	Camera(float _aspectRatio, DirectX::XMFLOAT3 initPos, DirectX::XMFLOAT3 initOrient, float _fov, float _near, float _far, float _mvmtSpd, float _mouseSpd, const char* _name);
	~Camera();
	Camera(Camera& c);

	// GETTERS
	DirectX::XMFLOAT4X4 GetView();
	DirectX::XMFLOAT4X4 GetProjection();
	std::shared_ptr<Transform> GetTransform();
	const char* GetName();
	float GetFov();
	float GetNearClip();
	float GetFarClip();
	float GetMvmtSpd();
	float GetMouseSpd();

	void Update(float dt);
	void UpdateViewMatrix();
	void UpdateProjectionMatrix(float _aspectRatio);

private:

	// FIELDS
	std::shared_ptr<Transform> transform;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
	float aspectRatio;
	float fov, nearClip, farClip, mvmtSpd, mouseSpd, orthographicWidth;
	Projection curProjection;
	const char* name;

};

