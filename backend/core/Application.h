#pragma once

#include <chrono>

#include "../../app/OrthographicCameraController.h"
#include "Timestep.h"
#include "../render/Math.h"
#include "../render/Renderer2D.h"
#include "../render/ShaderLibrary.h"
#include "../window/WindowManager.h"

class Application {
public:
    bool Init();
    void Run();
    void Shutdown();

private:
    void Update(Timestep timestep);

private:
    WindowManager m_Window;
    ShaderLibrary m_ShaderLibrary;
    OrthographicCameraController m_CameraController{ 16.0f / 9.0f, true };
    Transform m_QuadTransform;
    std::chrono::steady_clock::time_point m_LastFrameTime{};
    float m_QuadRotation = 0.0f;
    float m_QuadOffsetX = 0.0f;
    float m_QuadDirection = 1.0f;
    bool m_Initialized = false;
};
