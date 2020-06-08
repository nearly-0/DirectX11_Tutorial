//////////////
// INCLUDES //
//////////////
#include <vector>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Filename: Direct3D.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Graphics/Direct3D.h"

using namespace DirectX;

Direct3D::Direct3D()
    : m_pSwapChain(nullptr)
    , m_pDevice(nullptr)
    , m_pDeviceContext(nullptr)
    , m_pRenderTargetView(nullptr)
    , m_pDepthStencilBuffer(nullptr)
    , m_pDepthStencilState(nullptr)
    , m_pDepthStencilView(nullptr)
    , m_pRasterState(nullptr)
{
}

Direct3D::Direct3D(const Direct3D&)
{
}

Direct3D::~Direct3D()
{
}

// Setup for Direct3D for DirectX 11.
// 
bool Direct3D::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullScreen, float screenDepth, float screenNear)
{
    HRESULT result;

    IDXGIFactory* pFactory = nullptr;
    IDXGIAdapter* pAdapter = nullptr;
    IDXGIOutput* pAdapterOutput = nullptr;

    unsigned int numModes = 0;
    unsigned int numerator = 0;
    unsigned int denominator = 0;

    size_t stringLength;

    std::vector<DXGI_MODE_DESC> displayModeList;

    DXGI_MODE_DESC* pDisplayModeList = nullptr;
    DXGI_ADAPTER_DESC adapterDesc;

    int error = 0;

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    D3D_FEATURE_LEVEL    featureLevel;
    ID3D11Texture2D* pBackBuffer;

    D3D11_TEXTURE2D_DESC            depthBufferDesc;
    D3D11_DEPTH_STENCIL_DESC        depthStencilDesc;
    D3D11_DEPTH_STENCIL_VIEW_DESC   depthStencilViewDesc;
    D3D11_RASTERIZER_DESC           rasterDesc;
    D3D11_VIEWPORT                  viewport;

    float fieldOfView;
    float screenAspect;

    // Store the vsync setting.
    m_vsyncEnabled = vsync;

    // Getting the refesh rate from the video card/monitor.
    // If we just set the refresh rate to a default value which may not exist on all computers then
    // DirectX will respond by performing a blit instead of a buffer flip which will degrade
    // performance and give us annoying erros in the debug output.
    {
        // Create a DirectX graphics interface factory.
        result = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&pFactory));
        if (FAILED(result))
        {
            return false;
        }

        // Use the factory to create an adapter for the primary graphics interface (video card).
        result = pFactory->EnumAdapters(0, &pAdapter);
        if (FAILED(result))
        {
            return false;
        }

        // Enumerate the primary adapter output (monitor).
        result = pAdapter->EnumOutputs(0, &pAdapterOutput);
        if (FAILED(result))
        {
            return false;
        }

        // Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
        result = pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, nullptr);
        if (FAILED(result))
        {
            return false;
        }

        // Create a list to hold all the possible display modes for this monitor/video card combination.
        displayModeList.resize(numModes);
        if (displayModeList.empty())
        {
            return false;
        }

        // Now fill the display mode list structures.
        result = pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, &displayModeList[0]);
        if (FAILED(result))
        {
            return false;
        }

        // Now go through all the display modes and find the one that matches the screen width and height.
        // When a match is found store the numerator and denominator of the refresh rate for tha monitor.
        for (size_t idx = 0; idx < numModes; ++idx)
        {
            if (displayModeList[idx].Width == static_cast<unsigned int>(screenWidth))
            {
                if (displayModeList[idx].Height== static_cast<unsigned int>(screenHeight))
                {
                    numerator = displayModeList[idx].RefreshRate.Numerator;
                    denominator = displayModeList[idx].RefreshRate.Denominator;
                }
            }
        }
    }

    // Retriving the name of the video card and the amount of video memory.
    {
        // Get the adapter (video card) description.
        result = pAdapter->GetDesc(&adapterDesc);
        if (FAILED(result))
        {
            return false;
        }

        // Store the dedicated video card memory in megabytes.
        m_videoCardMemory = static_cast<int>(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

        // Convert the name of the video card to a character array and store it.
        error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
        if (error != 0)
        {
            return false;
        }
    }

    // Release the structure and interfaces used to get numerator and denominator.
    {
        // Release the adapter output.
        pAdapterOutput->Release();
        pAdapterOutput = nullptr;

        // Release the adapter.
        pAdapter->Release();
        pAdapter = nullptr;

        // Release the factory.
        pFactory->Release();
        pFactory = nullptr;
    }

    // Fill out the description of the swap chain.
    // [ Swap Chain ]
    //     : The front and back buffer to which the graphics will be drawn.
    //       Generally you usidng asingle back buffer, dao all your drawing to it,
    //       and then swap it to the front buffer which then displays on the user's screen.
    //       So, basically double buffer.
    {
        // Initialize the swap chain description.
        ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

        // Set to a single back buffer.
        swapChainDesc.BufferCount = 1;

        // Set the width and height of the back buffer.
        swapChainDesc.BufferDesc.Width = screenWidth;
        swapChainDesc.BufferDesc.Height = screenHeight;

        // Set regular 32-bit surface for the back buffer.
        swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

        // Set the refesh rate of the back buffer.
        if (m_vsyncEnabled)
        {
            swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
            swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
        }
        else
        {
            swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
            swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
        }

        // Set the usage of the back buffer.
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

        // Set the handle for the window to render to.
        swapChainDesc.OutputWindow = hwnd;

        // Turn multismapling off.
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;

        // Set to full screen or windowed mode.
        swapChainDesc.Windowed = !fullScreen;

        // Set the scan line ordering and scaling to unspecified.
        swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

        // Discard the back buffer contest after presenting.
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        // Don't set the advanced flags.
        swapChainDesc.Flags = 0;
    }

    // Setting feature level.
    // This tells direct x what version we plan to use.
    {
        // Set the feature level to DirectX 11.
        featureLevel = D3D_FEATURE_LEVEL_11_0;
    }

    // Creating swap chain
    // This call to create the device will fail if the primary video card is not compatible with DirectX 11.
    // Some machins may have the primary card as a DirectX 10 video card and the secondary card as a DirectX 11 video card.
    // Also some hybrid graphics cards work that way with the primary being the low power Intel cardand the secondary being the high power Nvidia card.
    // You will need to not use the default device and instead enumerate all the video cards in the machine and have
    // the user choose which one to use and then specify that card when creating the device.
    {
        // Create the swap chain, Direct3D device, and Direct3D device context.
        result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &m_pSwapChain, &m_pDevice, nullptr, &m_pDeviceContext);

        if (FAILED(result))
        {
            return false;
        }
    }

    // Attach the back buffer to our swap chain.
    {
        // Get the pointer to the back buffer.
        result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&pBackBuffer));
        if (FAILED(result))
        {
            return false;
        }

        // Create the render target view with the back buffer pointer.
        result = m_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pRenderTargetView);
        if (FAILED(result))
        {
            return false;
        }

        // Release pointer to the back buffer as we no longer need it.
        pBackBuffer->Release();
        pBackBuffer = nullptr;
    }

    // Set up a depth buffer description.
    // This is required to create a depth buffer so that our polygons can be rendered properly in 3D space.
    // Also, we weill attach a stencil buffer to our depth buffer.
    //
    // [ Stencil Buffer ]
    //     : Used to achieve effects such as motion blur, volumetric shadows, and other things.
    {
        // Inittialize the description of the depth buffer.
        ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

        // Set up the description of the depth buffer.
        depthBufferDesc.Width = screenWidth;
        depthBufferDesc.Height = screenHeight;
        depthBufferDesc.MipLevels = 1;
        depthBufferDesc.ArraySize = 1;
        depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthBufferDesc.SampleDesc.Count = 1;
        depthBufferDesc.SampleDesc.Quality = 0;
        depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthBufferDesc.CPUAccessFlags = 0;
        depthBufferDesc.MiscFlags = 0;

        // [ CreateTexture2D() ] : The function to make the 2D texture.
        
        // Create the texture for the depth buffer using the filled out description.
        // Note that CreateTexture2D is used, becuase on your polygons are sorted and then rasterized they just end up being colored pixeld in this 2D buffer.
        // Then this 2D buffer is drawn to the screen.
        result = m_pDevice->CreateTexture2D(&depthBufferDesc, nullptr, &m_pDepthStencilBuffer);
        if (FAILED(result))
        {
            return false;
        }
    }

    // Setup the depth stencil description.
    // This allows us to control what type of depth test Direct3D will do for each pixel.
    {
        // Initialize the description of the stencil state.
        ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

        // Set up the description of the stencil state.
        depthStencilDesc.DepthEnable = true;
        depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

        depthStencilDesc.StencilEnable = true;
        depthStencilDesc.StencilReadMask = 0xFF;
        depthStencilDesc.StencilWriteMask = 0xFF;

        // Stencil operation if pixel is front-facing.
        depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
        depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        // Stencil operations if pixel is back-facing.
        depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
        depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        // Create the depth stencil state.
        result = m_pDevice->CreateDepthStencilState(&depthStencilDesc, &m_pDepthStencilState);
        if (FAILED(result))
        {
            return false;
        }

        // Set the depth stencil state.
        m_pDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 1);
    }

    // Create the desription of the view of the depth stencil buffer.
    // This process is required so that Direct3D knows to use the depth buffer as a depth stencil texture.
    {
        // Initialize the depth stencil view.
        ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

        // Set up the depth stencil view description.
        depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        depthStencilViewDesc.Texture2D.MipSlice = 0;

        // Create the depth stencil view.
        result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
        if (FAILED(result))
        {
            return false;
        }

        // [ OMSetRenderTarget ] : Binds the render target view and the depth stencil buffer to the output render pipeline.
        // This way the graphics that the pipeline renders will get drawn to our back buffer that we previously created. 
        // With the graphics written to the back buffer we can then swap it to the front and display our graphics on the user's screen.
        m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
    }

    // Create Extras.
    {
        // Setup the raster description which will determine how and what polygons will be drawn.
        {
            // This allows us to control over how polygons are rendered.
            // Like making our scenes render in wireframe mode or have DirectX draw both the front and back faces of polygons.
            // By default, DirectX already has a resterizer state set up and working the exact same as the one below.
            rasterDesc.AntialiasedLineEnable = false;
            rasterDesc.CullMode = D3D11_CULL_BACK;
            rasterDesc.DepthBias = 0;
            rasterDesc.DepthBiasClamp = 0.0f;
            rasterDesc.DepthClipEnable = true;
            rasterDesc.FillMode = D3D11_FILL_SOLID;
            rasterDesc.FrontCounterClockwise = false;
            rasterDesc.MultisampleEnable = false;
            rasterDesc.ScissorEnable = false;
            rasterDesc.SlopeScaledDepthBias = 0.0f;

            // Create the rasterizer state from the description we just filled out.
            result = m_pDevice->CreateRasterizerState(&rasterDesc, &m_pRasterState);
            if (FAILED(result))
            {
                return false;
            }

            // Now set the rasterizer state.
            m_pDeviceContext->RSSetState(m_pRasterState);
        }

        // Setup the viewport for rendering.
        {
            // This allows Direct3D can map clip space coordinates to the render target space.
            // Set this to be the entire size of th window.
            viewport.Width = static_cast<float>(screenWidth);
            viewport.Height = static_cast<float>(screenHeight);
            viewport.MinDepth = 0.0f;
            viewport.MaxDepth = 1.0f;
            viewport.TopLeftX = 0.0f;
            viewport.TopLeftY = 0.0f;

            // Create the viewport.
            m_pDeviceContext->RSSetViewports(1, &viewport);
        }

        // Setup the projection matrix.
        {
            // This is sued to translate the 3D scene into the 2D viewport space.
            // We will need to keep a copy of this matrix sothat we can pass it to our shaders that will be used to render our scenes.
            fieldOfView = 3.141592654f / 4.0f;
            screenAspect = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);

            // Create the projection matrix for 3D rendering.
            m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);
        }

        // Setup the world matrix.
        {
            // This is used to conver the vertices of our objects into vertices in the 3D scene.
            // This is also used to rotate, translate, and scale our objects in 3D space.
            // Copy of this will be needed to be passed to the shaders for rendering also.

            // Initialize the world matrix to the identity matrix.
            // The view matrix is used to calculate the position of where we are looing at the scene from.
            // You can think of it as a camera and you only view the scene through this camera.
            m_worldMatrix = XMMatrixIdentity();
        }

        // Setup an orthographics projection matrix.
        {
            // This matrix is used for rendering 2D elements like user interfaces on the screen allowing us to skip the 3D rendering.

            // Create an orthographics projection matrix for 2D rendering.
            m_orthoMatrix = XMMatrixOrthographicLH(static_cast<float>(screenWidth), static_cast<float>(screenHeight), screenNear, screenDepth);
        }
    }

    return true;
}

// Release and clean up all the pointers used in the Initialize function.
// Note that we force the swap chain to go into windows mode first before releasing any pointers.
// If this is not done and you try to release the swap chain in full screen mode, it will throw some exceptios.
void Direct3D::Shutdown()
{
    // Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
    if (m_pSwapChain)
    {
        m_pSwapChain->SetFullscreenState(false, nullptr);
    }

    if (m_pRasterState)
    {
        m_pRasterState->Release();
        m_pRasterState = 0;
    }

    if (m_pDepthStencilView)
    {
        m_pDepthStencilView->Release();
        m_pDepthStencilView = 0;
    }

    if (m_pDepthStencilState)
    {
        m_pDepthStencilState->Release();
        m_pDepthStencilState = 0;
    }

    if (m_pDepthStencilBuffer)
    {
        m_pDepthStencilBuffer->Release();
        m_pDepthStencilBuffer = 0;
    }

    if (m_pRenderTargetView)
    {
        m_pRenderTargetView->Release();
        m_pRenderTargetView = 0;
    }

    if (m_pDeviceContext)
    {
        m_pDeviceContext->Release();
        m_pDeviceContext = 0;
    }

    if (m_pDevice)
    {
        m_pDevice->Release();
        m_pDevice = 0;
    }

    if (m_pSwapChain)
    {
        m_pSwapChain->Release();
        m_pSwapChain = 0;
    }
}

void Direct3D::BeginScene(float red, float green, float blue, float alpha)
{
    float color[4];

    // Setup the color to clear the buffer to.
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = alpha;

    // Clear the back buffer.
    m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);

    // Clear the depth buffer.
    m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

// Tells the swap chain to display our 3d scene once all the drawing has completed at the end of each frame.
void Direct3D::EndScene()
{
    // Present the back buffer to the screen since rendering is complete.
    if (m_vsyncEnabled)
    {
        // Lock to screen refesh rate.
        m_pSwapChain->Present(1, 0);
    }
    else
    {
        // Presnt as fast as possible.
        m_pSwapChain->Present(0, 0);
    }
}

ID3D11Device* Direct3D::GetDevice()
{
    return m_pDevice;
}

ID3D11DeviceContext* Direct3D::GetDeviceContext()
{
    return m_pDeviceContext;
}

void Direct3D::GetProjectionMatrix(DirectX::XMMATRIX& projectionMatrix)
{
    projectionMatrix = m_projectionMatrix;
}

void Direct3D::GetWorldMatrix(DirectX::XMMATRIX& worldMatrix)
{
    worldMatrix = m_worldMatrix;
}

void Direct3D::GetOrthoMatrix(DirectX::XMMATRIX& orthoMatrix)
{
    orthoMatrix = m_orthoMatrix;
}

// Returns the name ofthe video card and the amount of video memory.
// Knowing the video card name can help in debugging on different configurations.
void Direct3D::GetVideoCardInfo(char* pCardName, int& memory)
{
    strcpy_s(pCardName, 128, m_videoCardDescription);
    memory = m_videoCardMemory;
}
