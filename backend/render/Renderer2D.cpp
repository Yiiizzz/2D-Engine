#include "Renderer2D.h"

#include "Buffer.h"
#include "RenderCommand.h"
#include "Shader.h"
#include "VertexArray.h"
#include "../resource/ResourceManager.h"

#include <glad/glad.h>

#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>

namespace {
Matrix4 CreateTransform(float x, float y, float z, float width, float height, float rotationDegrees) {
    const float radians = rotationDegrees * 0.017453292519943295769f;
    const float cosine = std::cos(radians);
    const float sine = std::sin(radians);

    Matrix4 transform = Matrix4::Identity();
    float* m = transform.Data();
    m[0] = cosine * width;
    m[1] = sine * width;
    m[4] = -sine * height;
    m[5] = cosine * height;
    m[12] = x;
    m[13] = y;
    m[14] = z;
    return transform;
}
}

Renderer2D::Renderer2D()
    : m_Camera(0.0f, 1280.0f, 720.0f, 0.0f) {
}

bool Renderer2D::init(void* windowHandle) {
    createQuadResources();
    return createSceneRenderTarget(1280, 720);
}

void Renderer2D::clear() {
    RenderCommand::SetClearColor({ 0.08f, 0.10f, 0.14f, 1.0f });
    RenderCommand::Clear();
}

bool Renderer2D::resizeSceneRenderTarget(int width, int height) {
    width = width > 0 ? width : 1;
    height = height > 0 ? height : 1;

    if (m_Width == width && m_Height == height && m_Framebuffer != 0) {
        return true;
    }

    return createSceneRenderTarget(width, height);
}

void Renderer2D::renderScene(const SceneState& sceneState, ResourceManager& resourceManager) {
    if (m_Framebuffer == 0) {
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
    RenderCommand::SetViewport(0, 0, static_cast<unsigned int>(m_Width), static_cast<unsigned int>(m_Height));
    RenderCommand::SetClearColor({ 0.05f, 0.05f, 0.06f, 1.0f });
    RenderCommand::Clear();

    m_Camera.SetProjection(0.0f, static_cast<float>(m_Width), static_cast<float>(m_Height), 0.0f);
    m_Shader->Bind();
    m_Shader->SetMat4("u_ViewProjection", m_Camera.GetViewProjectionMatrix());
    m_VertexArray->Bind();

    for (const GameObject& object : sceneState.objects) {
        const TextureResourceInfo* texture = resourceManager.getTexture(object.texturePath);
        if (texture == nullptr || texture->rendererId == 0) {
            continue;
        }

        const float width = 64.0f * object.scale[0];
        const float height = 64.0f * object.scale[1];
        const Matrix4 transform = CreateTransform(object.position[0], object.position[1], 0.0f, width, height, object.rotation);
        m_Shader->SetMat4("u_Transform", transform);
        glBindTexture(GL_TEXTURE_2D, texture->rendererId);
        RenderCommand::DrawIndexed(*m_VertexArray);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

SceneViewportImage Renderer2D::getSceneViewportImage() const {
    SceneViewportImage image;
    image.Handle = reinterpret_cast<void*>(static_cast<uintptr_t>(m_ColorAttachment));
    image.Width = static_cast<float>(m_Width);
    image.Height = static_cast<float>(m_Height);
    return image;
}

void Renderer2D::present() {
}

void Renderer2D::destroy() {
    destroySceneRenderTarget();
    m_Shader.reset();
    m_IndexBuffer.reset();
    m_VertexBuffer.reset();
    m_VertexArray.reset();
}

bool Renderer2D::createSceneRenderTarget(int width, int height) {
    destroySceneRenderTarget();

    m_Width = width;
    m_Height = height;

    glGenFramebuffers(1, &m_Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);

    glGenTextures(1, &m_ColorAttachment);
    glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

    glGenRenderbuffers(1, &m_DepthAttachment);
    glBindRenderbuffer(GL_RENDERBUFFER, m_DepthAttachment);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthAttachment);

    const bool complete = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (!complete) {
        std::cerr << "OpenGL framebuffer creation failed." << std::endl;
        destroySceneRenderTarget();
        return false;
    }

    return true;
}

void Renderer2D::destroySceneRenderTarget() {
    if (m_DepthAttachment != 0) {
        glDeleteRenderbuffers(1, &m_DepthAttachment);
        m_DepthAttachment = 0;
    }
    if (m_ColorAttachment != 0) {
        glDeleteTextures(1, &m_ColorAttachment);
        m_ColorAttachment = 0;
    }
    if (m_Framebuffer != 0) {
        glDeleteFramebuffers(1, &m_Framebuffer);
        m_Framebuffer = 0;
    }
}

void Renderer2D::createQuadResources() {
    const float vertices[] = {
         0.0f, 0.0f, 0.0f, 0.0f,
         1.0f, 0.0f, 1.0f, 0.0f,
         1.0f, 1.0f, 1.0f, 1.0f,
         0.0f, 1.0f, 0.0f, 1.0f
    };

    const unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    m_VertexArray = VertexArray::Create();
    m_VertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
    m_VertexBuffer->SetLayout({
        { ShaderDataType::Float2, "a_Position" },
        { ShaderDataType::Float2, "a_TexCoord" }
    });
    m_IndexBuffer = IndexBuffer::Create(indices, 6);
    m_VertexArray->AddVertexBuffer(m_VertexBuffer);
    m_VertexArray->SetIndexBuffer(m_IndexBuffer);

    const std::string vertexSource = R"(
        #version 330 core
        layout(location = 0) in vec2 a_Position;
        layout(location = 1) in vec2 a_TexCoord;

        uniform mat4 u_ViewProjection;
        uniform mat4 u_Transform;

        out vec2 v_TexCoord;

        void main() {
            v_TexCoord = a_TexCoord;
            gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 0.0, 1.0);
        }
    )";

    const std::string fragmentSource = R"(
        #version 330 core
        layout(location = 0) out vec4 color;

        in vec2 v_TexCoord;
        uniform sampler2D u_Texture;

        void main() {
            color = texture(u_Texture, v_TexCoord);
        }
    )";

    m_Shader = Shader::Create(vertexSource, fragmentSource);
    m_Shader->Bind();
}
