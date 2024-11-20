#pragma once
#include <DirectXMath.h>
#include "SimpleShader.h"
#include <memory>
#include "Transform.h"
#include "Camera.h"


class Material
{
private:
	DirectX::XMFLOAT4 colorTint;
	std::shared_ptr<SimpleVertexShader> vs;
	std::shared_ptr<SimplePixelShader> ps;
	float roughness;


public:
	Material(DirectX::XMFLOAT4 _colorTint, float _roughness, Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, LPCWSTR vsPath, LPCWSTR psPath);
	Material(DirectX::XMFLOAT4 _colorTint, float _roughness, std::shared_ptr<SimpleVertexShader> _vs, std::shared_ptr<SimplePixelShader> _ps);
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

};

