#include "Camera.h"
#include "Input.h"

// want to loop through values of float4x4 for gui
// mouse drag does not work
// view is upside down

Camera::Camera(float _aspectRatio, DirectX::XMFLOAT3 initPos, const char *_name) :
	aspectRatio(_aspectRatio)
{
	fov = DirectX::XM_PIDIV4;	
	nearClip = 0.001f;
	farClip = 100.0f;
	mvmtSpd = 3.0f;
	mouseSpd = 0.005f;
	curProjection = PERSPECTIVE;
	orthographicWidth = 10;
	name = _name;

	transform = std::make_shared<Transform>();
	transform->SetPosition(0, 0, 0);
	transform->SetRotation(0, 0, 0);

	UpdateViewMatrix();
	UpdateProjectionMatrix(aspectRatio);
}

Camera::Camera(float _aspectRatio, DirectX::XMFLOAT3 initPos, DirectX::XMFLOAT3 initOrient, float _fov, float _near, float _far, float _mvmtSpd, float _mouseSpd, const char* _name) :
	aspectRatio(_aspectRatio),
	fov(_fov),
	nearClip(_near),
	farClip(_far),
	mvmtSpd(_mvmtSpd),
	mouseSpd(_mouseSpd)
{
	curProjection = PERSPECTIVE;
	orthographicWidth = 10;
	name = _name;

	transform = std::make_shared<Transform>();
	transform->SetPosition(initPos);
	transform->SetRotation(initOrient);

	UpdateViewMatrix();
	UpdateProjectionMatrix(aspectRatio);
}

Camera::~Camera()
{
}

Camera::Camera(Camera& c)
{
	transform = c.transform;
	view = c.view;
	projection = c.projection;
	aspectRatio = c.aspectRatio;
	fov = c.fov;
	nearClip = c.nearClip;
	farClip = c.farClip;
	mvmtSpd = c.mvmtSpd;
	mouseSpd = c.mouseSpd;
	projection = c.projection;
	orthographicWidth = c.orthographicWidth;
	name = c.name;

	UpdateViewMatrix();
	UpdateProjectionMatrix(c.aspectRatio);
}

DirectX::XMFLOAT4X4 Camera::GetView()
{
	return view;
}

DirectX::XMFLOAT4X4 Camera::GetProjection()
{
	return projection;
}

std::shared_ptr<Transform> Camera::GetTransform()
{
	return transform;
}

const char* Camera::GetName()
{
	return name;
}

float Camera::GetFov()
{
	return fov;
}

float Camera::GetNearClip()
{
	return nearClip;
}

float Camera::GetFarClip()
{
	return farClip;
}

float Camera::GetMvmtSpd()
{
	return mvmtSpd;
}

float Camera::GetMouseSpd()
{
	return mouseSpd;
}

void Camera::Update(float dt)
{
	// handle input
	if (Input::KeyDown('W')) transform->MoveRelative(0, 0, mvmtSpd * dt);		// forward
	if (Input::KeyDown('S')) transform->MoveRelative(0, 0, -(mvmtSpd * dt));	// backward
	if (Input::KeyDown('A')) transform->MoveRelative(-(mvmtSpd * dt), 0, 0);	// left
	if (Input::KeyDown('D')) transform->MoveRelative(mvmtSpd * dt, 0, 0);		// right
	if (Input::KeyDown(VK_SPACE)) transform->MoveRelative(0, mvmtSpd * dt, 0);	// up
	if (Input::KeyDown('X')) transform->MoveRelative(0, -(mvmtSpd * dt), 0);	// down
	if (Input::MouseLeftDown()) {
		float mvX = mouseSpd * Input::GetMouseXDelta();
		float mvY = mouseSpd * Input::GetMouseYDelta();
		
		transform->Rotate(mvY, mvX, 0);

		// storage to math
		DirectX::XMFLOAT3 rotFloat = transform->GetPitchYawRoll();
		if (rotFloat.x > DirectX::XM_PIDIV2) rotFloat.x = DirectX::XM_PIDIV2;
		if (rotFloat.x < -DirectX::XM_PIDIV2) rotFloat.x = -DirectX::XM_PIDIV2;
		transform->SetRotation(rotFloat);
	}
	
	UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
	// storage to math
	DirectX::XMFLOAT3 position = transform->GetPosition();
	DirectX::XMFLOAT3 forward = transform->GetForward();

	// create view matrix
	DirectX::XMMATRIX viewMat = DirectX::XMMatrixLookToLH(
		DirectX::XMLoadFloat3(&position),
		DirectX::XMLoadFloat3(&forward),
		DirectX::XMVectorSet(0, 1, 0, 0));

	// math to storage
	DirectX::XMStoreFloat4x4(&view, viewMat);
}

void Camera::UpdateProjectionMatrix(float _aspectRatio)
{
	aspectRatio = _aspectRatio;

	switch (curProjection) {
	case PERSPECTIVE:
		DirectX::XMMATRIX perspProj = DirectX::XMMatrixPerspectiveFovLH(fov, _aspectRatio, nearClip, farClip);
		DirectX::XMStoreFloat4x4(&projection, perspProj);
		break;

	case ORTHOGRAPHIC: 
		DirectX::XMMATRIX orthoProj = DirectX::XMMatrixOrthographicLH(orthographicWidth, orthographicWidth / aspectRatio, nearClip, farClip);
		DirectX::XMStoreFloat4x4(&projection, orthoProj);
		break;
	}

}
