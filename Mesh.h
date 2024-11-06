#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <vector>
#include <memory>
#include "Vertex.h"
#include "Graphics.h"

class Mesh
{
public:
	/// <summary>
	/// Constructor for Mesh
	/// </summary>
	/// <param name="name">mesh name</param>
	/// <param name="vertArray">vertex array</param>
	/// <param name="numVerts">number of vertices</param>
	/// <param name="indexArray">index array</param>
	/// <param name="numIndices">number of indices</param>
	Mesh(const char* name, Vertex* vertArray, size_t numVerts, unsigned int* indexArray, size_t numIndices);
	
	Mesh(const char* name, const std::wstring& filePath);
	/// <summary>
	/// Mesh class destructor.
	/// </summary>
	~Mesh();

	/// <summary>
	/// Get pointer to Vertex Buffer
	/// </summary>
	/// <returns>vertex buffer pointer</returns>
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();

	/// <summary>
	/// Get pointer to Index Buffer
	/// </summary>
	/// <returns>index buffer pointer</returns>
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();

	/// <summary>
	/// Mesh name
	/// </summary>
	/// <returns>char* of name</returns>
	const char* GetName();

	/// <summary>
	/// Vertex count 
	/// </summary>
	/// <returns>int number of vertices</returns>
	unsigned int GetVertexCount();

	/// <summary>
	/// Index count
	/// </summary>
	/// <returns>int number of indices</returns>
	unsigned int GetIndexCount();

	/// <summary>
	/// draw this mesh to the screen
	/// </summary>
	void Draw();

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertBuff;	// vertex buffer
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuff;	// index buffer
	const char* name;		// name of mesh
	int indices;			// number of indices
	int verts;				// number of vertices

	/// <summary>
	/// Creates the vertex and index buffers
	/// </summary>
	/// <param name="vertArray">Vertex array</param>
	/// <param name="numVerts">number of vertices</param>
	/// <param name="indexArray">index array</param>
	/// <param name="numIndices">number of indices</param>
	void CreateBuffers(Vertex* vertArray, size_t numVerts, unsigned int* indexArray, size_t numIndices);
};

