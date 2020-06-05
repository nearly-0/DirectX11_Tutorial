////////////////////////////////////////////////////////////////////////////////
// Class name: Direct3D
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <directxmath.h>

////////////////////////////////////////////////////////////////////////////////
// Class name: Direct3D
////////////////////////////////////////////////////////////////////////////////
class Direct3D
{
public:
    Direct3D();
    Direct3D(const Direct3D&);
    ~Direct3D();

    bool Initialize(int, int, bool, HWND, bool, float, float);
    void Shutdown();

    void BeginScene(float, float, float, float);
    void EndScene();

    ID3D11Device* GetDevice();
    ID3D11DeviceContext* GetDeviceContext();

    void GetProjectionMatrix(DirectX::XMMATRIX&);
    void GetWorldMatrix(DirectX::XMMATRIX&);
    void GetOrthoMatrix(DirectX::XMMATRIX&);

    void GetVideoCardInfo(char*, int&);

private:
    bool    m_vsync_enabled;
    int     m_videoCardMemory;
    char    m_videoCardDescription[128];

    IDXGISwapChain*             m_pSwapChain;
    ID3D11Device*               m_pDevice;
    ID3D11DeviceContext*        m_pDeviceContext;
    ID3D11RenderTargetView*     m_pRenderTargetView;
    ID3D11Texture2D*            m_pDepthStencilBuffer;
    ID3D11DepthStencilState*    m_pDepthStencilState;
    ID3D11DepthStencilView*     m_pDepthStendilView;
    ID3D11RasterizerState*      m_pRasterState;

    DirectX::XMMATRIX m_projectionMatrix;
    DirectX::XMMATRIX m_worldMatrix;
    DirectX::XMMATRIX m_orthoMatrix;
};

