#pragma once

#include <windows.h>
#include <memory>

#include "Graphics/Direct3D.h"
#include "Graphics/Camera.h"
#include "Graphics/Model.h"
#include "Graphics/ColorShader.h"

constexpr bool FULL_SCREEN = false;
constexpr bool VSYNC_ENABLED = true;
constexpr float SCREEN_DEPTH = 1000.0f;
constexpr float SCREEN_NEAR = 0.1f;

class Graphics
{
public:
    explicit Graphics();
    explicit Graphics(const Graphics&);
    ~Graphics();

    bool Initialize(int, int, HWND);
    void Shutdown();
    bool Frame();

private:
    bool Render();

private:
    std::unique_ptr<Direct3D> m_pDirect3D;
    std::unique_ptr<Camera> m_pCamera;
    std::unique_ptr<Model> m_pModel;
    std::unique_ptr<ColorShader> m_pColorShader;
};

