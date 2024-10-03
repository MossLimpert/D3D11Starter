#include "GameEntity.h"
#include "Graphics.h"
#include "BufferStructs.h"

GameEntity::GameEntity(std::shared_ptr<Mesh> _m)
{
    mesh = _m;
    transform = std::make_shared<Transform>();
}

GameEntity::~GameEntity()
{
}

GameEntity::GameEntity(GameEntity& ge)
{
    mesh = ge.mesh;
    transform = ge.transform;
}

std::shared_ptr<Mesh> GameEntity::GetMesh()
{
    return mesh;
}

std::shared_ptr<Transform> GameEntity::GetTransform()
{
    return transform;
}

void GameEntity::Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, DirectX::XMFLOAT4 tint, std::shared_ptr<Camera> cam)
{
    // bind constant buffer
    // using &constantBuffer tells windows we want to delete it, which isnt
    // what we actually want. you have to use .GetAddressOf()
    Graphics::Context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

    // collect data
    VertexShaderData data;
    data.colorTint = tint;
    data.world = transform->GetWorldMatrix();
    data.view = cam->GetView();
    data.projection = cam->GetProjection();

    // map 
    D3D11_MAPPED_SUBRESOURCE mappedBuff = {};
    Graphics::Context->Map(
        constantBuffer.Get(),
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &mappedBuff
    );

    // memcpy
    memcpy(mappedBuff.pData, &data, sizeof(data));

    // unmap
    Graphics::Context->Unmap(constantBuffer.Get(), 0);

    // set buffers and draw
    mesh->Draw();
}
