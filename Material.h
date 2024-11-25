#pragma once
#include <DirectXMath.h>
#include <memory>
#include <unordered_map>
#include "SimpleShader.h"
#include "Transform.h"
#include "Camera.h"



class Material
{
private:
	DirectX::XMFLOAT4 colorTint;
	std::shared_ptr<SimpleVertexShader> vs;
	std::shared_ptr<SimplePixelShader> ps;
	float roughness;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureSRVs;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> samplers;

	DirectX::XMFLOAT2 uvScale;
	DirectX::XMFLOAT2 uvOffset;

public:
	Material(DirectX::XMFLOAT4 _colorTint, float _roughness, Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, LPCWSTR vsPath, LPCWSTR psPath, DirectX::XMFLOAT2 scale, DirectX::XMFLOAT2 offset);
	Material(DirectX::XMFLOAT4 _colorTint, float _roughness, std::shared_ptr<SimpleVertexShader> _vs, std::shared_ptr<SimplePixelShader> _ps, DirectX::XMFLOAT2 scale, DirectX::XMFLOAT2 offset);
	~Material();
	Material(Material& m);

	DirectX::XMFLOAT4 GetColorTint();
	std::shared_ptr<SimpleVertexShader> GetVertexShader();
	std::shared_ptr<SimplePixelShader> GetPixelShader();
	float GetRoughness();

	void SetColorTint(DirectX::XMFLOAT4 _colorTint);
	void SetVertexShader(std::shared_ptr<SimpleVertexShader> _vs);
	void SetPixelShader(std::shared_ptr<SimplePixelShader> _ps);
	void SetRoughness(float _roughness);

	void PrepareMaterial(std::shared_ptr<Transform> transform, std::shared_ptr<Camera> camera);
	void AddTextureSRV(std::string _name, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _srv);
	void AddSampler(std::string _name, Microsoft::WRL::ComPtr<ID3D11SamplerState> _sampler);

	DirectX::XMFLOAT2 GetUVOffset();
	void SetUVOffset(DirectX::XMFLOAT2 offset);
	DirectX::XMFLOAT2 GetUVScale();
	void SetUVScale(DirectX::XMFLOAT2 scale);
};

