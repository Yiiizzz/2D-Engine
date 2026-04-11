#include "Application.h"

#include "../render/RenderCommand.h"
#include "../render/Renderer.h"

#include <cmath>
#include <exception>
#include <filesystem>
#include <iostream>

bool Application::Init() {
    try {
        const std::filesystem::path projectRoot = std::filesystem::path(__FILE__).parent_path().parent_path().parent_path();
        const std::filesystem::path renderer2DShaderPath = projectRoot / "assets" / "shaders" / "Renderer2D_Quad.glsl";

        WindowSpecification specification;
        specification.Title = "Lancelot Render Layer Demo";
        specification.Width = 1280;
        specification.Height = 720;
        specification.VSync = true;
        specification.API = GraphicsAPI::OpenGL;

        if (!m_Window.Init(specification)) {
            return false;
        }

        Renderer::Init(m_Window.GetGraphicsAPI());
        m_CameraController.SetWindowHandle(m_Window.GetNativeWindow());
        m_Window.SetFramebufferResizeCallback([this](unsigned int width, unsigned int height) {
            if (width == 0 || height == 0) {
                return;
            }

            Renderer::OnWindowResize(width, height);
            m_CameraController.OnResize(width, height);
        });
        m_Window.SetMouseScrollCallback([this](float xOffset, float yOffset) {
            m_CameraController.OnMouseScrolled(xOffset, yOffset);
        });

        const unsigned int framebufferWidth = m_Window.GetFramebufferWidth();
        const unsigned int framebufferHeight = m_Window.GetFramebufferHeight();
        if (framebufferWidth > 0 && framebufferHeight > 0) {
            Renderer::OnWindowResize(framebufferWidth, framebufferHeight);
            m_CameraController.OnResize(framebufferWidth, framebufferHeight);
        }

        m_ShaderLibrary.Load("Renderer2D_Quad", renderer2DShaderPath.string());
        Renderer2D::Init(CreateRef<ShaderLibrary>(m_ShaderLibrary));
        m_LastFrameTime = std::chrono::steady_clock::now();
        m_Initialized = true;
        return true;
    } catch (const std::exception& exception) {
        std::cerr << "Application initialization failed: " << exception.what() << std::endl;
        Shutdown();
        return false;
    }
}

void Application::Run() {
    while (m_Initialized && !m_Window.ShouldClose()) {
        const auto frameStartTime = std::chrono::steady_clock::now();
        const std::chrono::duration<float> frameDuration = frameStartTime - m_LastFrameTime;
        m_LastFrameTime = frameStartTime;
        const Timestep timestep(frameDuration.count());

        m_Window.PollEvents();
        Update(timestep);

        if (m_Window.IsMinimized()) {
            continue;
        }

        RenderCommand::SetClearColor({ 0.08f, 0.10f, 0.14f, 1.0f });
        RenderCommand::Clear();

        Renderer2D::BeginScene(m_CameraController.GetCamera());
        Renderer2D::DrawQuad(m_QuadTransform, { 0.95f, 0.45f, 0.25f, 1.0f });
        Renderer2D::EndScene();

        m_Window.OnUpdate();
    }
}

void Application::Shutdown() {
    if (!m_Initialized && m_Window.GetNativeWindow() == nullptr) {
        return;
    }

    Renderer2D::Shutdown();
    Renderer::Shutdown();
    m_Window.Shutdown();
    m_Initialized = false;
}

void Application::Update(Timestep timestep) {
    constexpr float quadAngularSpeed = 1.2f;
    constexpr float quadMoveSpeed = 0.45f;
    constexpr float quadBounds = 0.45f;

    m_CameraController.OnUpdate(timestep);

    m_QuadRotation += quadAngularSpeed * timestep.GetSeconds();
    m_QuadOffsetX += m_QuadDirection * quadMoveSpeed * timestep.GetSeconds();

    if (m_QuadOffsetX > quadBounds) {
        m_QuadOffsetX = quadBounds;
        m_QuadDirection = -1.0f;
    } else if (m_QuadOffsetX < -quadBounds) {
        m_QuadOffsetX = -quadBounds;
        m_QuadDirection = 1.0f;
    }

    m_QuadTransform.Translation = { m_QuadOffsetX, 0.0f, 0.0f };
    m_QuadTransform.Rotation = { 0.0f, 0.0f, m_QuadRotation };
    const float scalePulse = 1.0f + 0.15f * std::sin(m_QuadRotation * 1.3f);
    m_QuadTransform.Scale = { scalePulse, scalePulse, 1.0f };
}
