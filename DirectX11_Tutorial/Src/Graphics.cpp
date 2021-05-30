#include "Graphics/Graphics.h"

using namespace DirectX;

Graphics::Graphics()
    : m_pDirect3D(nullptr)
    , m_pCamera(nullptr)
    , m_pColorShader(nullptr)
{
}

Graphics::Graphics(const Graphics& other)
{
}

Graphics::~Graphics()
{
}

// Create the Direct3D object and then call the Direct3D initialization function.
// The width, height and handle will be sent to this function.
bool Graphics::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
    bool result = false;

    // Create the Direct3D object>
    m_pDirect3D = std::make_unique<Direct3D>();
    if (!m_pDirect3D.get())
    {
        return false;
    }

    // Initialize the Direct3D object.
    result = m_pDirect3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
        return false;
    }

    // Create the camera object
    m_pCamera = std::make_unique<Camera>();
    if (!m_pCamera.get())
    {
        return false;
    }

    // Set the initial position of the camera.
    m_pCamera->SetPosition(0.f, 0.f, -10.f);

    // Create the model object.
    m_pModel = std::make_unique<Model>();
    if (!m_pModel.get())
    {
        return false;
    }

    result = m_pModel->Initialize(m_pDirect3D->GetDevice());
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
        return false;
    }

    m_pColorShader = std::make_unique<ColorShader>();
    if (!m_pColorShader.get())
    {
        return false;
    }

    result = m_pColorShader->Initialize(m_pDirect3D->GetDevice(), hwnd);
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
        return false;
    }

    return true;
}

// Shut down of graphics objects occur here.
// Note that we are checking if the pointer was initialized of not.
// If it wasn't we can assume it was never set up and not try to shut it down.
void Graphics::Shutdown()
{
    if (m_pColorShader)
    {
        m_pColorShader->Shutdown();
        m_pColorShader.reset();
        m_pColorShader = nullptr;
    }

    if (m_pModel)
    {
        m_pModel->Shutdown();
        m_pModel.reset();
        m_pModel = nullptr;
    }

    if (m_pCamera)
    {
        m_pCamera.reset();
        m_pCamera = nullptr;
    }

    // Release the Direct3D object.
    if (m_pDirect3D)
    {
        m_pDirect3D->Shutdown();
        m_pDirect3D.reset();
        m_pDirect3D = nullptr;
    }
}

bool Graphics::Frame()
{
    // Render the graphics scene.
    if (!Render())
    {
        return false;
    }
    return true;
}

bool Graphics::Render()
{
    // Clear the buffers to begin the scene.
    m_pDirect3D->BeginScene(0.f, 0.f, 0.f, 1.0f);

    // Generate the view matrix based on the camera's position
    m_pCamera->Render();

    // Get the world, view, and projection matrices from the camera and d3d objects.
    XMMATRIX worldMatrix;
    m_pDirect3D->GetWorldMatrix(worldMatrix);
    
    XMMATRIX viewMatrix;
    m_pCamera->GetViewMatrix(viewMatrix);
    
    XMMATRIX projectionMatrix;
    m_pDirect3D->GetProjectionMatrix(projectionMatrix);

    // Put the model vertex and index buffers on the graphics pipeline to perpare them for drawing.
    m_pModel->Render(m_pDirect3D->GetDeviceContext());

    // Render the model using the color shader.
    if (!m_pColorShader->Render(m_pDirect3D->GetDeviceContext(), m_pModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix))
    {
        return false;
    }

    // Present the rendered scene to the screen.
    m_pDirect3D->EndScene();

    return true;
}
