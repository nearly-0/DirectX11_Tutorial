////////////////////////////////////////////////////////////////////////////////
// Filename: Direct3D.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Graphics/Direct3D.h"

Direct3D::Direct3D()
{
}

Direct3D::Direct3D(const Direct3D&)
{
}

Direct3D::~Direct3D()
{
}

bool Direct3D::Initialize(int, int, bool, HWND, bool, float, float)
{
    return false;
}

void Direct3D::Shutdown()
{
}

void Direct3D::BeginScene(float, float, float, float)
{
}

void Direct3D::EndScene()
{
}

ID3D11Device* Direct3D::GetDevice()
{
    return nullptr;
}

ID3D11DeviceContext* Direct3D::GetDeviceContext()
{
    return nullptr;
}

void Direct3D::GetProjectionMatrix(DirectX::XMMATRIX&)
{
}

void Direct3D::GetWorldMatrix(DirectX::XMMATRIX&)
{
}

void Direct3D::GetOrthoMatrix(DirectX::XMMATRIX&)
{
}

void Direct3D::GetVideoCardInfo(char*, int&)
{
}
