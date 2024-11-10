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
    // set vertex and pixel shaders
    // 
    material->GetVertexShader()->SetShader();
    material->GetPixelShader()->SetShader();

    std::shared_ptr<SimpleVertexShader> vs = material->GetVertexShader();

    //vs->SetFloat4("colorTint", material->GetColorTint());   // Strings here MUST
    vs->SetMatrix4x4("world", transform->GetWorldMatrix()); // match variable
    vs->SetMatrix4x4("view", cam->GetView());               // names in your
    vs->SetMatrix4x4("projection", cam->GetProjection());   // shader’s cbuffer!
          
    vs->CopyAllBufferData();

    std::shared_ptr<SimplePixelShader> ps = material->GetPixelShader();

    ps->SetFloat4("colorTint", material->GetColorTint());
    ps->CopyAllBufferData();

    mesh->Draw();
}
