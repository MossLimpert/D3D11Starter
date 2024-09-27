#include "Transform.h"
#include <DirectXMath.h>

Transform::Transform()
{
    position = 
}

Transform::~Transform()
{
}

Transform::Transform(Transform& t)
{
}

DirectX::XMMATRIX Transform::GetWorldMatrix()
{
    return DirectX::XMMATRIX();
}
