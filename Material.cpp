#include "Material.h"
#include <memory>



Material::Material(DirectX::XMFLOAT4 _colorTint, float _roughness, Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, LPCWSTR vsPath, LPCWSTR psPath, DirectX::XMFLOAT2 scale, DirectX::XMFLOAT2 offset)
{
	this->colorTint = _colorTint;
	this->roughness = _roughness;
	this->vs = std::make_shared<SimpleVertexShader>(device, context, vsPath);
	this->ps = std::make_shared<SimplePixelShader>(device, context, psPath);
	this->uvScale = scale;
	this->uvOffset = offset;
}

Material::Material(DirectX::XMFLOAT4 _colorTint, float _roughness, std::shared_ptr<SimpleVertexShader> _vs, std::shared_ptr<SimplePixelShader> _ps, DirectX::XMFLOAT2 scale, DirectX::XMFLOAT2 offset)
{
	this->colorTint = _colorTint;
	this->roughness = _roughness;
	this->vs = _vs;
	this->ps = _ps;
	this->uvScale = scale;
	this->uvOffset = offset;
}

Material::~Material()
{
}

Material::Material(Material& m)
{
	this->colorTint = m.colorTint;
	this->vs = m.vs;
	this->ps = m.ps;
	this->roughness = m.roughness;
	this->uvScale = m.uvScale;
	this->uvOffset = m.uvOffset;
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

void Material::PrepareMaterial(std::shared_ptr<Transform> transform, std::shared_ptr<Camera> camera)
{
	// copied from the demo

	// turn on the shaders for this material
	vs->SetShader();
	ps->SetShader();

	// send data to the vertex shader
	vs->SetMatrix4x4("world", transform->GetWorldMatrix());
	vs->SetMatrix4x4("worldInvTranspose", transform->GetInverseTransposeWorldMatrix());
	vs->SetMatrix4x4("view", camera->GetView());
	vs->SetMatrix4x4("projection", camera->GetProjection());
	vs->CopyAllBufferData();

	// send data to the pixel shader
	ps->SetFloat3("colorTint", DirectX::XMFLOAT3(colorTint.x, colorTint.y, colorTint.z));
	ps->SetFloat3("cameraPosition", camera->GetTransform()->GetPosition());
	ps->SetFloat("roughness", roughness);
	ps->SetFloat2("uvScale", uvScale);
	ps->SetFloat2("uvOffset", uvOffset);
	ps->CopyAllBufferData();

	// loop through shader resource views and sampler states
	for (auto& t : textureSRVs) { ps->SetShaderResourceView(t.first.c_str(), t.second.Get()); }
	for (auto& s : samplers) { ps->SetSamplerState(s.first.c_str(), s.second); }
}

void Material::AddTextureSRV(std::string _name, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _srv)
{
	textureSRVs.insert({ _name, _srv });
}

void Material::AddSampler(std::string _name, Microsoft::WRL::ComPtr<ID3D11SamplerState> _sampler)
{
	samplers.insert({_name, _sampler});
}

DirectX::XMFLOAT2 Material::GetUVOffset()
{
	return uvOffset;
}

void Material::SetUVOffset(DirectX::XMFLOAT2 offset)
{
	uvOffset = offset;
}

DirectX::XMFLOAT2 Material::GetUVScale()
{
	return uvScale;
}

void Material::SetUVScale(DirectX::XMFLOAT2 scale)
{
	uvScale = scale;
}

