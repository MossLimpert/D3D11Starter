#pragma once

#include "Mesh.h"
#include "SimpleShader.h"
#include "Camera.h"

#include <memory>
#include <wrl/client.h>

class Sky
{
private:

	// shaders
	std::shared_ptr<SimpleVertexShader> skyVS;
	std::shared_ptr<SimplePixelShader> skyPS;

	std::shared_ptr<Mesh> skyMesh;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> skyRasterState;	// rasterizer options
																	// to draw inside
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> skyDepthState;	// depth buffer 
																	// comparison type
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skySRV;		// texture

	// sampler options
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerOptions;

	void InitRenderStates();

	// create cubemap from 6 individual textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateCubemap(
		const wchar_t* right,
		const wchar_t* left,
		const wchar_t* up,
		const wchar_t* down,
		const wchar_t* front,
		const wchar_t* back
	);



public:

	// Constructor that takes the existing cube map SRV
	Sky(
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeMap,
		std::shared_ptr<Mesh> mesh,
		std::shared_ptr<SimpleVertexShader> skyVS,
		std::shared_ptr<SimplePixelShader> skyPS,
		Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerOptions
	);

	// constructor that loads a dds cube map
	Sky(
		const wchar_t* cubemapDDSFile,
		std::shared_ptr<Mesh> mesh,
		std::shared_ptr<SimpleVertexShader> skyVS,
		std::shared_ptr<SimplePixelShader> skyPS,
		Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerOptions
	);

	// constructor that loads 6 textures and makes a cube map
	Sky(
		const wchar_t* right,
		const wchar_t* left,
		const wchar_t* up,
		const wchar_t* down,
		const wchar_t* front,
		const wchar_t* back,
		std::shared_ptr<Mesh> mesh,
		std::shared_ptr<SimpleVertexShader> skyVS,
		std::shared_ptr<SimplePixelShader> skyPS,
		Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerOptions
	);

	~Sky();

	void Draw(std::shared_ptr<Camera> camera);

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetSkyTexture();


};

