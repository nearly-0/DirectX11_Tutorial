////////////////////////////////////////////////////////////////////////////////
// Filename: Graphics.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Graphics/Graphics.h"

Graphics::Graphics()
    : m_pDirect3D(nullptr)
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

    return true;
}

// Shut down of graphics objects occur here.
// Note that we are checking if the pointer was initialized of not.
// If it wasn't we can assume it was never set up and not try to shut it down.
void Graphics::Shutdown()
{
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
    m_pDirect3D->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);

    // Present the rendered scene to the screen.
    m_pDirect3D->EndScene();

    return true;
}
