//////////////////////////////////////////////////////////////////////
// Filename: System.cpp
//////////////////////////////////////////////////////////////////////

#include "System/System.h"

System::System()
    : m_applicationName(nullptr)
    , m_hInstance(nullptr)
    , m_hwnd(nullptr)
    , m_pInput(nullptr)
    , m_pGraphics(nullptr)
{
}

System::System(const System& other)
{
}

// Destructor is left empty because certain windows functions like ExitThread() 
// are know for not calling your class destructor resulting in memory leaks.
// It will be fine if you use safer functions but being careful is fine.
System::~System()
{
}

bool System::Initialize()
{
    // Initialize the width and height of the screen to zero before sending the variables into the function.
    int screenWidth(0);
    int screenHeight(0);

    // Initialize the windows api.
    InitializeWindows(screenWidth, screenHeight);

    // Create the input object. This object will be used to handle reading the keyboard input from the user.
    m_pInput = std::make_unique<Input>();
    if (!m_pInput)
    {
        return false;
    }

    // Initialize the input objects.
    m_pInput->Initialize();

    // Create the graphics object. This object will handle rendering all the graphics for this application.
    m_pGraphics = std::make_unique<Graphics>();
    if (!m_pGraphics)
    {
        return false;
    }

    // Initialize the graphics object.
    if(!m_pGraphics->Initialize(screenWidth, screenHeight, m_hwnd))
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------
// Shutdown and release everything associated with graphics 
// and input object.
//-----------------------------------------------------------------
void System::Shutdown()
{
    // Release the graphics object.
    if (m_pGraphics)
    {
        m_pGraphics->Shutdown();
        m_pGraphics.reset();
        m_pGraphics = nullptr;
    }

    // Release the input object
    if (m_pInput)
    {
        m_pInput.reset();
        m_pInput = nullptr;
    }

    // Shutdown the window.
    ShutdownWindows();
}

//-----------------------------------------------------------------
// Where our application will loop and to all the
// application processing until we decide to quit.
//
// While not done
//    - Check for windows system message
//    - Process system messages
//    - Process application loop
//    - Check if user wanted to quit during the
//      frame processing.
//-----------------------------------------------------------------
void System::Run()
{
    MSG msg;
    bool done(false);
    bool result(false);

    // Initialize the message structure
    ZeroMemory(&msg, sizeof(MSG));

    // Loop until there is a quit message from the window or the user.
    while (!done)
    {
        // Handle the windows message.
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // If windows signals to end the application then exit out.
        if (msg.message == WM_QUIT)
        {
            done = true;
        }
        else
        {
            // Otherwise do the frame processing.
            result = Frame();
            if (!result)
            {
                done = true;
            }
        }
    }
}

//-----------------------------------------------------------------
// Where all the processing for our application is done.
//-----------------------------------------------------------------
bool System::Frame()
{
    // Check if the user pressed excape and wants to exti the application.
    if (m_pInput->IsKeyDown(VK_ESCAPE))
    {
        return false;
    }

    // Do the frame processing for the graphics object.
    if (!m_pGraphics->Frame())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------
// Where we direct the windows system message into.
//-----------------------------------------------------------------
LRESULT System::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
    switch (umsg)
    {
    // Check if a key has been pressed on the keyboard.
    case WM_KEYDOWN:
    {
        // If a key is pressed send it to the input object so it can recore that state.
        m_pInput->KeyDown(static_cast<unsigned int>(wparam));
        return 0;
    }
    break;

    // Check if a key has been released on the keyboard.
    case WM_KEYUP:
    {
        // If a key is release then send it to the input object so it can unset the state for that key.
        m_pInput->KeyUp(static_cast<unsigned int>(wparam));
        return 0;
    }
    break;

    // Any other messages sned to the default message handler as our application won't make use of them.
    default:
    {
        return DefWindowProc(hwnd, umsg, wparam, lparam);
    }
    break;
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Where we put the code to build the window we will use to render to.
// Returns screenWidth and screenHeight back to the calling function 
// so we can make use of them throughout the application.
//
// We are creating the window with some default setting to initialize
// a plain black window with no borders.
//
// Window will me made depending on the global variable called FULL_SCREEN.
// If it is set to true, then we make the screen cover the entire users desktop
// window. If it is set to false we just make a 800x600 window in the middle
// of the screen.
//-----------------------------------------------------------------------------
void System::InitializeWindows(int& screenWidth, int& screenHeight)
{
    WNDCLASSEX wc;
    DEVMODE dmScreenSettings;
    int posX;
    int posY;

    // Get an external pointer to this object.
    s_pApplicationHandle = this;

    // Get the instance of this application.
    m_hInstance = GetModuleHandle(nullptr);

    // Give the application a name.
    m_applicationName = L"Engine";

    // Setup the windows class with default settings.
    wc.style            = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc      = WndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = m_hInstance;
    wc.hIcon            = LoadIcon(nullptr, IDI_WINLOGO);
    wc.hIconSm          = wc.hIcon;
    wc.hCursor          = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground    = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wc.lpszMenuName     = nullptr;
    wc.lpszClassName    = m_applicationName;
    wc.cbSize           = sizeof(WNDCLASSEX);

    // Register the window class.
    RegisterClassEx(&wc);

    // Determine the rsolution of the clients desktop screen.
    screenWidth  = GetSystemMetrics(SM_CXSCREEN);
    screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Setup the screen settings depending ton whether it is running in full screen or in windowed mode.
    if (FULL_SCREEN)
    {
        // If full screen set the screen to maximum size of the users desktop and 32bit.
        memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
        dmScreenSettings.dmSize = sizeof(dmScreenSettings);
        dmScreenSettings.dmPelsWidth = static_cast<unsigned long>(screenWidth);
        dmScreenSettings.dmPelsHeight = static_cast<unsigned long>(screenHeight);
        dmScreenSettings.dmBitsPerPel = 32;
        dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        // Change the display settings to full screen.
        ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

        // Set the position of the window to the top left corner.
        posX = posY = 0;
    }
    else
    {
        // If windowed then set it to 800x600 resolution.
        screenWidth = 800;
        screenHeight = 600;

        // Place the window in the middle of the screen.
        posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
        posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
    }

    // Create the window with the screen settings and get the handle to it.
    m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
                        WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, posX, posY, 
                        screenWidth, screenHeight, nullptr, nullptr, m_hInstance, nullptr);

    // Bring the window up on the screen and set it as main focus.
    ShowWindow(m_hwnd, SW_SHOW);
    SetForegroundWindow(m_hwnd);
    SetFocus(m_hwnd);

    // Hide the mouse cursor.
    ShowCursor(false);
}

//-----------------------------------------------------------------------------
// Returns the screen settings back to normal and releases the window and
// handles associated with it.
//-----------------------------------------------------------------------------
void System::ShutdownWindows()
{
    // Show the mouse cursor.
    ShowCursor(true);

    // Fix the display settings if leaving full screen mode.
    if (FULL_SCREEN)
    {
        ChangeDisplaySettings(nullptr, 0);
    }

    // Remove the window.
    DestroyWindow(m_hwnd);
    m_hwnd = nullptr;

    // Remove the application instance.
    UnregisterClass(m_applicationName, m_hInstance);
    m_hInstance = nullptr;

    // Release the pointer to this class.
    s_pApplicationHandle = nullptr;
}

//-----------------------------------------------------------------------------
// Where windows sends its messages to.
//-----------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
    switch (umessage)
    {
    // Check if the window is being destroyed.
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }
        break;

    // Check if the window is being closed.
    case WM_CLOSE:
    {
        PostQuitMessage(0);
        return 0;
    }
        break;

    // All other messages pass to the mesage handler in the systme class.
    default:
        return s_pApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
        break;
    }
}