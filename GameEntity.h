#pragma once

#include "Transform.h"
#include "Mesh.h"
#include "Camera.h"
#include "Material.h"

class GameEntity
{
public:
	// RULE OF 3
	GameEntity(std::shared_ptr<Mesh> _m, std::shared_ptr<Material> _material);
	~GameEntity();
	GameEntity(GameEntity& ge);

	// GETTERS
	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Transform> GetTransform();
	std::shared_ptr<Material> GetMaterial();  

	void SetMaterial(std::shared_ptr<Material> _m);

	void Draw(DirectX::XMFLOAT4 tint, std::shared_ptr<Camera> cam);
private:

	std::shared_ptr<Transform> transform;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;
};

