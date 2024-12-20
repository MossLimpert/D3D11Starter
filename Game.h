#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <vector>
#include <memory>

#include "Mesh.h"
#include "GameEntity.h"
#include "Camera.h"
#include "SimpleShader.h"
#include "Lights.h"
#include "Sky.h"

class Game
{
public:
	// Basic OOP setup
	Game() = default;
	~Game();
	Game(const Game&) = delete; // Remove copy constructor
	Game& operator=(const Game&) = delete; // Remove copy-assignment operator

	// Primary functions
	void Initialize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);
	void OnResize();

private:

	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void LoadShaders();
	void CreateGeometry();
	void CreateLights();
	void CreateMaterials();
	void GuiUpdate(float deltaTime);
	void BuildGui();
	void InitializeCamera();
	void UpdateObjectTransformations(float deltaTime);

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	
	// list of meshes
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<std::shared_ptr<GameEntity>> entities;
	std::vector<std::shared_ptr<Camera>> cameras;
	std::vector<std::shared_ptr<Material>> materials;
	std::vector<Light> lights;

	int curCamera;
	DirectX::XMFLOAT3 ambient;
	std::shared_ptr<Sky> sky;

	// Shaders and shader-related constructs
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimplePixelShader> normalsPS;
	std::shared_ptr<SimplePixelShader> uvPS;
	std::shared_ptr<SimplePixelShader> pseudoPS;

	// shadow mapping
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> shadowDSV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shadowSRV;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> shadowRasterizer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> shadowSampler;
	DirectX::XMFLOAT4X4 lightViewMatrix;
	DirectX::XMFLOAT4X4 lightProjectionMatrix;
	//Microsoft::WRL::ComPtr<ID3D11Texture2D> shadowDepthMap;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> shadowDepthMap;
	ShadowOptions shadowOptions;
	std::shared_ptr<SimpleVertexShader> shadowVS;

};

