#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

class ColorShader
{
private:
	/// Definition of cbuffer type that will be used with the vertex shader.
	/// This typedef must be exactly the same as the one in the vertex shader
	/// as the model data needs to math typedefs in the shader for proper rendering.
	struct MatrixBuffer
	{
		DirectX::XMMATRIX m_world;
		DirectX::XMMATRIX m_view;
		DirectX::XMMATRIX m_projection;
	};

public:
	ColorShader();
	ColorShader(const ColorShader& kOther);
	~ColorShader();

	bool Initialize(ID3D11Device* pDevice, HWND hwnd);
	void Shutdown();
	// sets the shader parameters and then draws the prepared model vertieces using the shader.
	bool Render(ID3D11DeviceContext* pDeviceContext, int indexCount, DirectX::XMMATRIX worldMatrix, DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix);

private:
	bool InitializeShader(ID3D11Device* pDevice, HWND hwnd, const WCHAR* pVertexShaderFile, const WCHAR* pPixelShaderFile);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob* pErrorMsg, HWND hwnd, const WCHAR* pShaderFileName);

	bool SetShaderParameters(ID3D11DeviceContext* pDeviceContext, DirectX::XMMATRIX worldMatrix, DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix);
	void RenderShader(ID3D11DeviceContext* pDeviceContext, int indexCount);

private:
	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader* m_pPixelShader;
	ID3D11InputLayout* m_pLayout;
	ID3D11Buffer* m_pMatrixBuffer;
};

