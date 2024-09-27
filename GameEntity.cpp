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

void GameEntity::Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer)
{
    // bind constant buffer
    Graphics::Context->VSSetConstantBuffers(0, 1, &constantBuffer);

    // collect data
    
}
