#include <vector>
#include <memory>
#include "Graphics/Model.h"

using namespace DirectX;

Model::Model()
	: m_pVertexBuffer(nullptr)
	, m_pIndexBuffer(nullptr)
	, m_vertexCount(0)
	, m_indexCount(0)
{
}

Model::Model(const Model& kOther)
{
	m_indexCount = kOther.m_indexCount;
	m_vertexCount = kOther.m_vertexCount;
	m_pVertexBuffer = kOther.m_pVertexBuffer;
	m_pIndexBuffer = kOther.m_pIndexBuffer;
}

Model::~Model()
{
}

bool Model::Initialize(ID3D11Device* pDevice)
{
	// Initialize the vertex and index buffers.
	return InitializeBuffers(pDevice);
}

void Model::Shutdown()
{
	ShutdownBuffers();
}

// This will be called from Graphics::Render function.
void Model::Render(ID3D11DeviceContext* pDeviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(pDeviceContext);
}

int Model::GetIndexCount()
{
	return m_indexCount;
}

// Where we handle dreating the vertex and index buffers.
bool Model::InitializeBuffers(ID3D11Device* pDevice)
{
	// Set the number of vertices in the vertex array.
	m_vertexCount = 3;
	// Set the number of indices in the index array.
	m_indexCount = 3;

	// Create the vertex array.
	Vertex* pVertices = new Vertex[m_vertexCount];
	if (!pVertices)
	{
		return false;
	}

	// Create the index array.
	unsigned long* pIndices = new unsigned long[m_indexCount];
	if (!pIndices)
	{
		return false;
	}

	// Please note that I create the points in the clockwise order of drawing them.
	// If you do this counter clockwise it will think the triangle is facing the opposite direction and not draw it due to back face culling. 
	// Always remember that the order in which you send your vertices to the GPU is very important. 
	// The color is set here as well since it is part of the vertex description. I set the color to green.

	// Load the vertex array with data.
	pVertices[0].m_position = XMFLOAT3(-1.0f, -1.0f, 0.0f); // Bottom left.
	pVertices[0].m_color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	pVertices[1].m_position = XMFLOAT3(0.0f, 1.0f, 0.0f); // Bottom left.
	pVertices[1].m_color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	pVertices[2].m_position = XMFLOAT3(1.0f, -1.0f, 0.0f); // Bottom left.
	pVertices[2].m_color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	// Load the index array with data.
	pIndices[0] = 0; // Bottom Left
	pIndices[1] = 1; // Top middle.
	pIndices[2] = 2; // Bottom right.

	// Steps to creating the vertex buffer and index buffer.
	// 1. Fill out a description of the buffer.
	// 2. Fill out a subresource pointer which will point to either your vertex or index array.
	// 3. Call CreateBuffer using the D3DD device and it will return a pointer to your new buffer.

	// Set up the description of the static vertex buffer.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
		
	// Give the subresource structure a pointer to the vertex data.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = pVertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	HRESULT result = pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_pVertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the static index buffer.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = pIndices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = pDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_pIndexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] pVertices;
	pVertices = nullptr;

	delete[] pIndices;
	pIndices = nullptr;

	return true;
}

void Model::ShutdownBuffers()
{
	// Release the index buffer.
	if (m_pIndexBuffer)
	{
		m_pIndexBuffer->Release();
		m_pIndexBuffer = nullptr;
	}

	// Release the vertex buffer.
	if (m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = nullptr;
	}
}

// Called from the Render function.
// Purpose : Set the vertex buffer and index buffer as active on the input assemlber on the GPU.
void Model::RenderBuffers(ID3D11DeviceContext* pDeviceContext)
{
	// Set vertex buffer stride and offset.
	uint32_t stride = sizeof(Vertex);
	uint32_t offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// Set the index bufffer to active in the input assembler.
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer.
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
