#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

// This is responsible for encapsulatin the geometry for 3D models.
class Model
{
private:
	// This typedef must match the layout in the ColorShader
	struct Vertex
	{
		DirectX::XMFLOAT3 m_position;
		DirectX::XMFLOAT4 m_color;
	};

public:
	Model();
	Model(const Model& kOther);
	~Model();

	bool Initialize(ID3D11Device* pDevice);
	void Shutdown();
	void Render(ID3D11DeviceContext* pDeviceContext);

	int GetIndexCount();

private:
	bool InitializeBuffers(ID3D11Device* pDevice);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext* pDeviceContext);

private:
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	int m_vertexCount;
	int m_indexCount;
};

