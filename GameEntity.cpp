#include "GameEntity.h"
#include "Graphics.h"
#include "BufferStructs.h"

GameEntity::GameEntity(std::shared_ptr<Mesh> _m, std::shared_ptr<Material> _material)
{
    mesh = _m;
    transform = std::make_shared<Transform>();
    this->material = _material;
}

GameEntity::~GameEntity()
{
}

GameEntity::GameEntity(GameEntity& ge)
{
    mesh = ge.mesh;
    transform = ge.transform;
    material = ge.material;
}

std::shared_ptr<Mesh> GameEntity::GetMesh()
{
    return mesh;
}

std::shared_ptr<Transform> GameEntity::GetTransform()
{
    return transform;
}

std::shared_ptr<Material> GameEntity::GetMaterial()
{
    return material;
}

void GameEntity::SetMaterial(std::shared_ptr<Material> _m)
{
    material = _m;
}

void GameEntity::Draw(DirectX::XMFLOAT4 tint, std::shared_ptr<Camera> cam)
{
    // set up material's shaders and data
    material->PrepareMaterial(transform, cam);

    // draw it
    mesh->Draw();
}
