#pragma once

#include "Transform.h"
#include "Mesh.h"

class GameEntity
{
public:
	// RULE OF 3
	GameEntity(std::shared_ptr<Mesh> _m);
	~GameEntity();
	GameEntity(GameEntity& ge);

	// GETTERS
	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Transform> GetTransform();

	void Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, DirectX::XMFLOAT4 tint);
private:

	std::shared_ptr<Transform> transform;
	std::shared_ptr<Mesh> mesh;
};

