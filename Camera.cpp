#include "Camera.h"
#include "Input.h"

Camera::Camera(float _aspectRatio, DirectX::XMFLOAT3 initPos, const char *_name) :
	aspectRatio(_aspectRatio)
{
	fov = DirectX::XM_PIDIV4;	// copied values from demo
	nearClip = 0.01f;
	farClip = 100.0f;
	mvmtSpd = 5.0f;
	mouseSpd = 0.002f;
	transform.SetPosition(0, 0, 0);
	transform.SetRotation(0, 0, 1);
	curProjection = PERSPECTIVE;
	orthographicWidth = 10;
	name = _name;

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
	transform.SetPosition(initPos);
	transform.SetRotation(initOrient);
	curProjection = PERSPECTIVE;
	orthographicWidth = 10;
	name = _name;

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
	return DirectX::XMFLOAT4X4();
}

DirectX::XMFLOAT4X4 Camera::GetProjection()
{
	return DirectX::XMFLOAT4X4();
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
	if (Input::KeyDown('W')) transform.MoveRelative(0, 0, mvmtSpd * dt);		// forward
	if (Input::KeyDown('S')) transform.MoveRelative(0, 0, -(mvmtSpd * dt));		// backward
	if (Input::KeyDown('A')) transform.MoveRelative(-(mvmtSpd * dt), 0, 0);		// left
	if (Input::KeyDown('D')) transform.MoveRelative(mvmtSpd * dt, 0, 0);		// right
	if (Input::KeyDown(VK_SPACE)) transform.MoveRelative(0, mvmtSpd * dt, 0);	// up
	if (Input::KeyDown('X')) transform.MoveRelative(0, -(mvmtSpd * dt), 0);		// down
	if (Input::MouseLeftDown()) {
		int mvX = Input::GetMouseXDelta();
		int mvY = Input::GetMouseYDelta();
		
		// storage to math
		DirectX::XMFLOAT3 rotFloat = transform.GetPitchYawRoll();
		DirectX::XMVECTOR curRot = DirectX::XMLoadFloat3(&rotFloat);
		DirectX::XMFLOAT3 addX(mvY * mouseSpd, 0, 0);	// make a vector for x rotation (around yaw)
		DirectX::XMFLOAT3 minVec(-0.5f, 0, 0);			// make min val vector
		DirectX::XMFLOAT3 maxVec(0.5f, 0, 0);			// make max val vector
		DirectX::XMVECTOR mouseMov = DirectX::XMVectorClamp(					// clamp x to min max
			DirectX::XMLoadFloat3(&addX), 
			DirectX::XMLoadFloat3(&minVec), 
			DirectX::XMLoadFloat3(&maxVec));	
		DirectX::XMFLOAT3 addXFloat;
		DirectX::XMStoreFloat3(&addXFloat, mouseMov);		// store result to use in rotation
		DirectX::XMVECTOR addRot = DirectX::XMQuaternionRotationRollPitchYaw(addXFloat.x, mvX * mouseSpd, 0);

		// math
		DirectX::XMVector3Rotate(curRot, addRot);
	}

	UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
	// storage to math
	DirectX::XMVECTOR pos = DirectX::XMVectorSet(transform.GetPosition().x, transform.GetPosition().y, transform.GetPosition().z, 0);
	DirectX::XMVECTOR lookDir = DirectX::XMVectorSet(transform.GetForward().x, transform.GetForward().y, transform.GetForward().z, 0);
	DirectX::XMVECTOR upDir = DirectX::XMVectorSet(transform.GetUp().x, transform.GetUp().y, transform.GetUp().z, 0);

	// create view matrix
	DirectX::XMMATRIX viewMat = DirectX::XMMatrixLookToLH(pos, lookDir, upDir);

	// math to storage
	DirectX::XMStoreFloat4x4(&view, viewMat);
}

void Camera::UpdateProjectionMatrix(float _aspectRatio)
{
	switch (curProjection) {
	case PERSPECTIVE:
		DirectX::XMMATRIX newPersp = DirectX::XMMatrixPerspectiveFovLH(fov, _aspectRatio, nearClip, farClip);
		DirectX::XMStoreFloat4x4(&projection, newPersp);
		break;

	case ORTHOGRAPHIC: 
		DirectX::XMMATRIX newOrtho = DirectX::XMMatrixOrthographicLH(orthographicWidth, orthographicWidth / aspectRatio, nearClip, farClip);
		DirectX::XMStoreFloat4x4(&projection, newOrtho);
		break;
	}
}
