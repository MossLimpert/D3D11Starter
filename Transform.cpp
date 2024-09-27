#include "Transform.h"
#include <DirectXMath.h>

Transform::Transform()
{
    position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
    world = DirectX::XMFLOAT4X4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );

    worldInverseTranspose = DirectX::XMFLOAT4X4(world);

    dirty = false;
}

Transform::~Transform()
{
    delete[] & position;
    delete[] & rotation;
    delete[] & scale;
    delete[] & world;
    delete[] & worldInverseTranspose;
}

Transform::Transform(Transform& t)
{
    position = t.position;
    rotation = t.rotation;
    scale = t.scale;
    dirty = t.dirty;
    world = t.world;
    worldInverseTranspose = t.worldInverseTranspose;
}

void Transform::SetPosition(float _x, float _y, float _z)
{
    position.x = _x;
    position.y = _y;
    position.z = _z;
    dirty = true;
}

void Transform::SetPosition(DirectX::XMFLOAT3 _pos)
{
    position = _pos;
    dirty = true;
}

void Transform::SetRotation(float _pitch, float _yaw, float _roll)
{
    position.x = _pitch;
    position.y = _yaw;
    position.z = _roll;

    dirty = true;
}

void Transform::SetRotation(DirectX::XMFLOAT3 _rot)
{
    rotation = _rot;
    dirty = true;
}

void Transform::SetScale(float _x, float _y, float _z)
{
    scale.x = _x;
    scale.y = _y;
    scale.z = _z;

    dirty = true;
}

void Transform::SetScale(DirectX::XMFLOAT3 _scale)
{
    scale = _scale;
    dirty = true;
}

DirectX::XMFLOAT3 Transform::GetPosition()
{
    return position;
}

DirectX::XMFLOAT3 Transform::GetPitchYawRoll()
{
    return rotation;
}

DirectX::XMFLOAT3 Transform::GetScale()
{
    return scale;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
    // make translation, scale, and rotation matrices
    DirectX::XMMATRIX trans = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    DirectX::XMMATRIX scaleMat = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
    DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);

    // calc world matrix
    DirectX::XMMATRIX _world = scaleMat * rotMat * trans;

    // store it
    DirectX::XMStoreFloat4x4(&world, _world);
    DirectX::XMStoreFloat4x4(&worldInverseTranspose, XMMatrixInverse(0, XMMatrixTranspose(_world)));
    
    // return new mat
    return world;
}

DirectX::XMFLOAT4X4 Transform::GetInverseTransposeWorldMatrix()
{
    return worldInverseTranspose;
}

void Transform::MoveAbsolute(float _x, float _y, float _z)
{
    // storage to math
    DirectX::XMVECTOR curr = DirectX::XMLoadFloat3(&position);
    DirectX::XMVECTOR incoming = DirectX::XMVectorSet(_x, _y, _z, 0.0f);

    // math
    curr = DirectX::XMVectorAdd(curr, incoming);

    // math to storage
    DirectX::XMStoreFloat3(&position, curr);

    dirty = true;
}

void Transform::MoveAbsolute(DirectX::XMFLOAT3 offset)
{
    // storage to math
    DirectX::XMVECTOR curr = DirectX::XMLoadFloat3(&position);
    DirectX::XMVECTOR incoming = DirectX::XMLoadFloat3(&offset);
    
    // math
    curr = DirectX::XMVectorAdd(curr, incoming);

    // math to storage
    DirectX::XMStoreFloat3(&position, curr);

    dirty = true;
}

void Transform::Rotate(float _pitch, float _yaw, float _roll)
{
    // storage to math
    DirectX::XMVECTOR curr = DirectX::XMLoadFloat3(&rotation);
    DirectX::XMVECTOR incoming = DirectX::XMVectorSet(_pitch, _yaw, _roll, 0.0f);

    // math
    curr = DirectX::XMVectorAdd(curr, incoming);

    // math to storage
    DirectX::XMStoreFloat3(&rotation, curr);

    dirty = true;
}

void Transform::Rotate(DirectX::XMFLOAT3 _rotation)
{
    // storage to math
    DirectX::XMVECTOR curr = DirectX::XMLoadFloat3(&rotation);
    DirectX::XMVECTOR incoming = DirectX::XMLoadFloat3(&_rotation);

    // math
    curr = DirectX::XMVectorAdd(curr, incoming);

    // math storage
    DirectX::XMStoreFloat3(&rotation, curr);

    dirty = true;
}

void Transform::Scale(float _x, float _y, float _z)
{
    // storage to math
    DirectX::XMVECTOR curr = DirectX::XMLoadFloat3(&scale);
    DirectX::XMVECTOR incoming = DirectX::XMVectorSet(_x, _y, _z, 0.0f);

    // math
    DirectX::XMVectorMultiply(curr, incoming);

    // math to storage
    DirectX::XMStoreFloat3(&scale, curr);

    dirty = true;
}

void Transform::Scale(DirectX::XMFLOAT3 _scale)
{
    // storage to math
    DirectX::XMVECTOR curr = DirectX::XMLoadFloat3(&scale);
    DirectX::XMVECTOR incoming = DirectX::XMLoadFloat3(&_scale);

    // math
    DirectX::XMVectorMultiply(curr, incoming);

    // math to storage
    DirectX::XMStoreFloat3(&scale, curr);

    dirty = true;

}
