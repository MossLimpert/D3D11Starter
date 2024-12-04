#include "Sky.h"
#include "Graphics.h"
//#include "packages/directxtk_desktop_win10.2024.10.29.1/include/WICTextureLoader.h"
//#include "packages/directxtk_desktop_win10.2024.10.29.1/include/DDSTextureLoader.h"
//#include "packages/directxtk_desktop_win10.2024.10.29.1/include/WICTextureLoader.h"
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"

void Sky::InitRenderStates()
{
	// reverse cull mode
	D3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_FRONT;	// draw inside instead of out
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.DepthClipEnable = true;
	Graphics::Device->CreateRasterizerState(&rastDesc, skyRasterState.GetAddressOf());

	// set depth state to accept pixels with a depth == 1 (far clip plane)
	D3D11_DEPTH_STENCIL_DESC depthDesc = {};
	depthDesc.DepthEnable = true;
	depthDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	Graphics::Device->CreateDepthStencilState(&depthDesc, skyDepthState.GetAddressOf());
}

Sky::Sky(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeMap, std::shared_ptr<Mesh> mesh, std::shared_ptr<SimpleVertexShader> skyVS, std::shared_ptr<SimplePixelShader> skyPS, Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerOptions)
{
	skySRV = cubeMap;
	skyMesh = mesh;
	this->skyVS = skyVS;
	this->skyPS = skyPS;
	this->samplerOptions = samplerOptions;

	InitRenderStates();
}

Sky::Sky(const wchar_t* cubemapDDSFile, std::shared_ptr<Mesh> mesh, std::shared_ptr<SimpleVertexShader> skyVS, std::shared_ptr<SimplePixelShader> skyPS, Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerOptions)
{
	skyMesh = mesh;
	this->skyVS = skyVS;
	this->skyPS = skyPS;
	this->samplerOptions = samplerOptions;

	InitRenderStates();

	// load the texture
	CreateDDSTextureFromFile(Graphics::Device.Get(), cubemapDDSFile, 0, skySRV.GetAddressOf());
}

Sky::Sky(const wchar_t* right, const wchar_t* left, const wchar_t* up, const wchar_t* down, const wchar_t* front, const wchar_t* back, std::shared_ptr<Mesh> mesh, std::shared_ptr<SimpleVertexShader> skyVS, std::shared_ptr<SimplePixelShader> skyPS, Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerOptions)
{
	skyMesh = mesh;
	this->skyVS = skyVS;
	this->skyPS = skyPS;
	this->samplerOptions = samplerOptions;

	InitRenderStates();

	// create texture out of 6 images
	skySRV = CreateCubemap(right, left, up, down, front, back);
}

Sky::~Sky()
{
}

void Sky::Draw(std::shared_ptr<Camera> camera)
{
	// change the rasterizer state and depth stencil state
	Graphics::Context->RSSetState(skyRasterState.Get());
	Graphics::Context->OMSetDepthStencilState(skyDepthState.Get(), 0);

	// set sky shaders
	skyVS->SetShader();
	skyPS->SetShader();

	// give the vertex shader data
	skyVS->SetMatrix4x4("view", camera->GetView());
	skyVS->SetMatrix4x4("projection", camera->GetProjection());
	skyVS->CopyAllBufferData();

	// give the pixel shader data
	skyPS->SetShaderResourceView("SkyTexture", skySRV);
	skyPS->SetSamplerState("BasicSampler", samplerOptions);

	// set mesh buffers and draw
	skyMesh->Draw();

	// reset rasterizer and depth stencil
	Graphics::Context->RSSetState(0);
	Graphics::Context->OMSetDepthStencilState(0, 0);
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Sky::GetSkyTexture()
{
	return skySRV;
}

// --------------------------------------------------------
// Author: Chris Cascioli
// Purpose: Creates a cube map on the GPU from 6 individual textures
// 
// - You are allowed to directly copy/paste this into your code base
//   for assignments, given that you clearly cite that this is not
//   code of your own design.
//
// - Note: This code assumes you�re putting the function in Sky.cpp, 
//   you�ve included WICTextureLoader.h and you have an ID3D11Device 
//   ComPtr called �device�.  Make any adjustments necessary for
//   your own implementation.
// --------------------------------------------------------
// --------------------------------------------------------
// Loads six individual textures (the six faces of a cube map), then
// creates a blank cube map and copies each of the six textures to
// another face.  Afterwards, creates a shader resource view for
// the cube map and cleans up all of the temporary resources.
// --------------------------------------------------------
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Sky::CreateCubemap(
	const wchar_t* right,
	const wchar_t* left,
	const wchar_t* up,
	const wchar_t* down,
	const wchar_t* front,
	const wchar_t* back)
{
	// Load the 6 textures into an array.
	// - We need references to the TEXTURES, not SHADER RESOURCE VIEWS!
	// - Explicitly NOT generating mipmaps, as we don't need them for the sky!
	// - Order matters here!  +X, -X, +Y, -Y, +Z, -Z
	Microsoft::WRL::ComPtr<ID3D11Texture2D> textures[6] = {};
	CreateWICTextureFromFile(Graphics::Device.Get(), right, (ID3D11Resource**)textures[0].GetAddressOf(), 0);
	CreateWICTextureFromFile(Graphics::Device.Get(), left, (ID3D11Resource**)textures[1].GetAddressOf(), 0);
	CreateWICTextureFromFile(Graphics::Device.Get(), up, (ID3D11Resource**)textures[2].GetAddressOf(), 0);
	CreateWICTextureFromFile(Graphics::Device.Get(), down, (ID3D11Resource**)textures[3].GetAddressOf(), 0);
	CreateWICTextureFromFile(Graphics::Device.Get(), front, (ID3D11Resource**)textures[4].GetAddressOf(), 0);
	CreateWICTextureFromFile(Graphics::Device.Get(), back, (ID3D11Resource**)textures[5].GetAddressOf(), 0);

	// We'll assume all of the textures are the same color format and resolution,
	// so get the description of the first texture
	D3D11_TEXTURE2D_DESC faceDesc = {};
	textures[0]->GetDesc(&faceDesc);

	// Describe the resource for the cube map, which is simply 
	// a "texture 2d array" with the TEXTURECUBE flag set.  
	// This is a special GPU resource format, NOT just a 
	// C++ array of textures!!!
	D3D11_TEXTURE2D_DESC cubeDesc = {};
	cubeDesc.ArraySize = 6;            // Cube map!
	cubeDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // We'll be using as a texture in a shader
	cubeDesc.CPUAccessFlags = 0;       // No read back
	cubeDesc.Format = faceDesc.Format; // Match the loaded texture's color format
	cubeDesc.Width = faceDesc.Width;   // Match the size
	cubeDesc.Height = faceDesc.Height; // Match the size
	cubeDesc.MipLevels = 1;            // Only need 1
	cubeDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE; // This should be treated as a CUBE, not 6 separate textures
	cubeDesc.Usage = D3D11_USAGE_DEFAULT; // Standard usage
	cubeDesc.SampleDesc.Count = 1;
	cubeDesc.SampleDesc.Quality = 0;

	// Create the final texture resource to hold the cube map
	Microsoft::WRL::ComPtr<ID3D11Texture2D> cubeMapTexture;
	Graphics::Device->CreateTexture2D(&cubeDesc, 0, cubeMapTexture.GetAddressOf());

	// Loop through the individual face textures and copy them,
	// one at a time, to the cube map texure
	for (int i = 0; i < 6; i++)
	{
		// Calculate the subresource position to copy into
		unsigned int subresource = D3D11CalcSubresource(
			0,  // Which mip (zero, since there's only one)
			i,  // Which array element?
			1); // How many mip levels are in the texture?

		// Copy from one resource (texture) to another
		Graphics::Context->CopySubresourceRegion(
			cubeMapTexture.Get(),  // Destination resource
			subresource,           // Dest subresource index (one of the array elements)
			0, 0, 0,               // XYZ location of copy
			textures[i].Get(),     // Source resource
			0,                     // Source subresource index (we're assuming there's only one)
			0);                    // Source subresource "box" of data to copy (zero means the whole thing)
	}

	// At this point, all of the faces have been copied into the 
	// cube map texture, so we can describe a shader resource view for it
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = cubeDesc.Format;         // Same format as texture
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE; // Treat this as a cube!
	srvDesc.TextureCube.MipLevels = 1;        // Only need access to 1 mip
	srvDesc.TextureCube.MostDetailedMip = 0;  // Index of the first mip we want to see

	// Make the SRV
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeSRV;
	Graphics::Device->CreateShaderResourceView(cubeMapTexture.Get(), &srvDesc, cubeSRV.GetAddressOf());

	// Send back the SRV, which is what we need for our shaders
	return cubeSRV;
}