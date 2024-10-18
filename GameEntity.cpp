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

    vs->SetFloat4("colorTint", material->GetColorTint());   // Strings here MUST
    vs->SetMatrix4x4("world", transform->GetWorldMatrix()); // match variable
    vs->SetMatrix4x4("view", cam->GetView());               // names in your
    vs->SetMatrix4x4("projection", cam->GetProjection());   // shader’s cbuffer!
    // bind constant buffer
    // using &constantBuffer tells windows we want to delete it, which isnt
    // what we actually want. you have to use .GetAddressOf()
    //Graphics::Context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
    
    //// collect data
    //VertexShaderData data = VertexShaderData();
    //data.colorTint = tint;
    //data.world = transform->GetWorldMatrix();
    //data.view = cam->GetView();
    //data.projection = cam->GetProjection();

    //// map 
    //D3D11_MAPPED_SUBRESOURCE mappedBuff = {};
    //Graphics::Context->Map(
    //    constantBuffer.Get(),
    //    0,
    //    D3D11_MAP_WRITE_DISCARD,
    //    0,
    //    &mappedBuff
    //);
    vs->CopyAllBufferData();
    //// memcpy
    //memcpy(mappedBuff.pData, &data, sizeof(data));

    //// unmap
    //Graphics::Context->Unmap(constantBuffer.Get(), 0);

    // set buffers and draw
    mesh->Draw();
}
