////////////////////////////////////////////////////////////////////////////////
// Class name: Direct3D
////////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////
// LIBRARIES //
///////////////
/*
- d3d11.lib
    : Contains all the Direct3D functionality for setting up and drawing 3D graphics in DirectX 11.

- dxgi.lib
    : Cotains tools to inteface with the hardware on the computer to obtain information 
      about the refesth rate of the monitor, the video card beging used.

-d3dcompiler.lib
    : Contains functionality for compiling shaders which we will cover in the next tutorial.
*/

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
    explicit Direct3D();
    explicit Direct3D(const Direct3D&);
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
    bool    m_vsyncEnabled;
    int     m_videoCardMemory;
    char    m_videoCardDescription[128];

    IDXGISwapChain*             m_pSwapChain;
    ID3D11Device*               m_pDevice;
    ID3D11DeviceContext*        m_pDeviceContext;
    ID3D11RenderTargetView*     m_pRenderTargetView;
    ID3D11Texture2D*            m_pDepthStencilBuffer;
    ID3D11DepthStencilState*    m_pDepthStencilState;
    ID3D11DepthStencilView*     m_pDepthStencilView;
    ID3D11RasterizerState*      m_pRasterState;

    DirectX::XMMATRIX m_projectionMatrix;
    DirectX::XMMATRIX m_worldMatrix;
    DirectX::XMMATRIX m_orthoMatrix;
};

