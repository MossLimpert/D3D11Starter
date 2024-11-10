#include "Material.h"
#include <memory>


Material::Material(DirectX::XMFLOAT4 _colorTint, float _roughness, Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, LPCWSTR vsPath, LPCWSTR psPath)
{
	this->colorTint = _colorTint;
	this->roughness = _roughness;
	this->vs = std::make_shared<SimpleVertexShader>(device, context, vsPath);
	this->ps = std::make_shared<SimplePixelShader>(device, context, psPath);
}

Material::Material(DirectX::XMFLOAT4 _colorTint, float _roughness, std::shared_ptr<SimpleVertexShader> _vs, std::shared_ptr<SimplePixelShader> _ps)
{
	this->colorTint = _colorTint;
	this->roughness = _roughness;
	this->vs = _vs;
	this->ps = _ps;
}

Material::~Material()
{
}

Material::Material(Material& m)
{
	this->colorTint = m.colorTint;
	this->vs = m.vs;
	this->ps = m.ps;
}

DirectX::XMFLOAT4 Material::GetColorTint()
{
	return colorTint;
}

std::shared_ptr<SimpleVertexShader> Material::GetVertexShader()
{
	return vs;
}

std::shared_ptr<SimplePixelShader> Material::GetPixelShader()
{
	return ps;
}

float Material::GetRoughness()
{
	return roughness;
}

void Material::SetColorTint(DirectX::XMFLOAT4 _colorTint)
{
	colorTint = _colorTint;
}

void Material::SetVertexShader(std::shared_ptr<SimpleVertexShader> _vs)
{
	vs = _vs;
}

void Material::SetPixelShader(std::shared_ptr<SimplePixelShader> _ps)
{
	ps = _ps;
}

void Material::SetRoughness(float _roughness)
{
	roughness = _roughness;
}

