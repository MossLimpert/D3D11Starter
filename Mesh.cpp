#include "Mesh.h"

using namespace DirectX;

Mesh::Mesh(const char* _name, Vertex* vertArray, size_t numVerts, unsigned int* indexArray, size_t numIndices)
{
	name = _name;
	CreateBuffers(vertArray, numVerts, indexArray, numIndices);
}

Mesh::~Mesh()
{
}

Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetVertexBuffer()
{
	return vertBuff;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetIndexBuffer()
{
	return indexBuff;
}

const char* Mesh::GetName()
{
	return name;
}

unsigned int Mesh::GetVertexCount()
{
	return verts;
}

unsigned int Mesh::GetIndexCount()
{
	return indices;
}

void Mesh::Draw()
{
	UINT stride = sizeof(Vertex); // how far each jump in looking at mem locations is
	UINT offset = 0;
	// set active buffers
	Graphics::Context->IASetVertexBuffers(0, 1, vertBuff.GetAddressOf(), &stride, &offset);
	Graphics::Context->IASetIndexBuffer(indexBuff.Get(), DXGI_FORMAT_R32_UINT, 0);

	// now draw it
	Graphics::Context->DrawIndexed(this->indices, 0, 0);
}

void Mesh::CreateBuffers(Vertex* vertArray, size_t numVerts, unsigned int* indexArray, size_t numIndices)
{
	verts = (unsigned int)numVerts;
	indices = (unsigned int)numIndices;

	// create vertex buffer
	{
		D3D11_BUFFER_DESC vbd = {};
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(Vertex) * (UINT)numVerts;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;

		// create struct to hold initial vertex data
		D3D11_SUBRESOURCE_DATA initialVertexData = {};
		initialVertexData.pSysMem = vertArray;

		// actually create the buffer
		Graphics::Device->CreateBuffer(&vbd, &initialVertexData, vertBuff.GetAddressOf());
	}

	// create index buffer
	{
		D3D11_BUFFER_DESC ibd = {};
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.ByteWidth = sizeof(unsigned int) * (UINT)numIndices; // are unsigned int and UINT different???????? 
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags = 0;
		ibd.StructureByteStride = 0;

		// create struct to hold initial vertex data
		D3D11_SUBRESOURCE_DATA initialIndexData = {};
		initialIndexData.pSysMem = indexArray;

		// actually create the buffer
		Graphics::Device->CreateBuffer(&ibd, &initialIndexData, indexBuff.GetAddressOf());

	}
}
