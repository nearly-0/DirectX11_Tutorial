#include "System/System.h"

int WINAPI WinMain(
    HINSTANCE hInstance, 
    HINSTANCE hPrevInstance, 
    PSTR pScmdline, 
    int iCmdshow)
{
    bool result;


    // Create the system object.
    std::unique_ptr<System> pSystem = std::make_unique<System>();
    if (!pSystem)
    {
        return 0;
    }

    // Initialize and run the system object.
    result = pSystem->Initialize();
    if (result)
    {
        pSystem->Run();
    }

    // Shutdown and release the system object.
    pSystem->Shutdown();
    pSystem.reset();
    pSystem = nullptr;

    return 0;
}

/*
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    // Contains information about the window: 
    // - The application icon
    // - The background color of the window
    // - The name to display in the title bar, among other things
    // - Function pointer to your window procedure
    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;

    RegisterClass(&wc);

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"DirectX Tutorial",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        nullptr,       // Parent window    
        nullptr,       // Menu
        hInstance,  // Instance handle
        nullptr        // Additional application data
    );

    if (hwnd == nullptr)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    // Run the message loop.

    MSG msg = { };
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        // Related to keyboard input.
        // Translate keystrokes into characters.
        // Remember to call is before DiapatechMessage()
        TranslateMessage(&msg);

        // 1. The function that tells the operating system to call the window procedure of the window
        // that is the target of the message. In the words, the operating system looks up the window
        // handle in its table of windows, finds the function pointer associated with the window,
        // and invokes the function.
        //
        // 2. It also calls the windows procedure of the window that is the target of the message.
        DispatchMessage(&msg);
    }
    // If you want to exit the application and break out of the message loop, call
    // PostQuitMessage() function.
    // PostQuitMessage(0);


    return 0;
}



/**
* This is the window procedure
* You write code to handle messages that the application receives from Windows when events occur in this function. 
*
* @param hwnd : a handle to the window
* @param uMsg : the message code; for example, the VM_SIZE message indiciates the window was resized
* @param wParam & lParam : contain additional data that pertains to the message.
* @return LRESULT

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);



        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        EndPaint(hwnd, &ps);
    }
    return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
*/