#include "Application.h"

#include "../render/Buffer.h"
#include "../render/RenderCommand.h"
#include "../render/Renderer.h"
#include "../render/Shader.h"
#include "../render/VertexArray.h"

#include <exception>
#include <iostream>
#include <string>

bool Application::Init() {
    try {
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
        Renderer::OnWindowResize(m_Window.GetWidth(), m_Window.GetHeight());
        CreateDemoScene();
        UpdateCamera();
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
        m_Window.PollEvents();
        UpdateCamera();

        RenderCommand::SetClearColor({ 0.08f, 0.10f, 0.14f, 1.0f });
        RenderCommand::Clear();

        Renderer::BeginScene(m_Camera);
        Renderer::Submit(m_Shader, m_VertexArray);
        Renderer::EndScene();

        m_Window.OnUpdate();
    }
}

void Application::Shutdown() {
    if (!m_Initialized && m_Window.GetNativeWindow() == nullptr) {
        return;
    }

    m_Shader.reset();
    m_IndexBuffer.reset();
    m_VertexBuffer.reset();
    m_VertexArray.reset();
    Renderer::Shutdown();
    m_Window.Shutdown();
    m_Initialized = false;
}

void Application::CreateDemoScene() {
    const float vertices[] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.3f, 0.2f,
         0.5f, -0.5f, 0.0f, 0.2f, 0.8f, 0.3f,
         0.5f,  0.5f, 0.0f, 0.2f, 0.4f, 1.0f,
        -0.5f,  0.5f, 0.0f, 1.0f, 0.8f, 0.2f
    };

    const unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    m_VertexArray = VertexArray::Create();
    m_VertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
    m_VertexBuffer->SetLayout({
        { ShaderDataType::Float3, "a_Position" },
        { ShaderDataType::Float3, "a_Color" }
    });

    m_IndexBuffer = IndexBuffer::Create(indices, static_cast<unsigned int>(sizeof(indices) / sizeof(unsigned int)));
    m_VertexArray->AddVertexBuffer(m_VertexBuffer);
    m_VertexArray->SetIndexBuffer(m_IndexBuffer);

    const std::string vertexShader = R"(
        #version 330 core
        layout(location = 0) in vec3 a_Position;
        layout(location = 1) in vec3 a_Color;

        uniform mat4 u_ViewProjection;

        out vec3 v_Color;

        void main() {
            v_Color = a_Color;
            gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
        }
    )";

    const std::string fragmentShader = R"(
        #version 330 core
        layout(location = 0) out vec4 color;

        in vec3 v_Color;

        void main() {
            color = vec4(v_Color, 1.0);
        }
    )";

    m_Shader = Shader::Create(vertexShader, fragmentShader);
}

void Application::UpdateCamera() {
    const float aspectRatio = static_cast<float>(m_Window.GetWidth()) / static_cast<float>(m_Window.GetHeight());
    const float orthoHeight = 0.9f;
    const float orthoWidth = orthoHeight * aspectRatio;
    m_Camera.SetProjection(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight);
    Renderer::OnWindowResize(m_Window.GetWidth(), m_Window.GetHeight());
}
