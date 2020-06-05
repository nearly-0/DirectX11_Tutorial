//////////////////////////////////////////////////////////////////////
// Filename: System.h
//////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////
// PRE-PROCESSING DIRECTIVES //
///////////////////////////////
#define WIN32_LEAN_AND_MEAN

//////////////
// INCLUDES //
//////////////
#include <windows.h>
#include <memory>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Input/Input.h"
#include "Graphics/Graphics.h"

////////////////////////////////////////////////////////////////////////////////////////////////
// Class name: System
//
// Desription
//  : Initialize, Shutdown, and Run the WinMain defined here.
//    It also handles MessageHandler function to handle the windows system messages that will
//    get sent to the application while it is runing.
////////////////////////////////////////////////////////////////////////////////////////////////
class System
{
public:
    System();
    System(const System&);
    ~System();

    bool Initialize();
    void Shutdown();
    void Run();

    LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
    bool Frame();
    void InitializeWindows(int&, int&);
    void ShutdownWindows();

private:
    LPCWSTR m_applicationName;
    HINSTANCE m_hInstance;
    HWND m_hwnd;

    std::unique_ptr<Input> m_pInput;
    std::unique_ptr<Graphics> m_pGraphics;
};

/////////////////////////
// FUNCTION PROTOTYPES //
/////////////////////////
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

/////////////
// GLOBALS //
/////////////
static System* s_pApplicationHandle = nullptr;