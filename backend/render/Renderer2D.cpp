#include "Renderer2D.h"

#include "Buffer.h"
#include "Material.h"
#include "OrthographicCamera.h"
#include "Renderer.h"
#include "ShaderLibrary.h"
#include "Texture.h"
#include "VertexArray.h"
#include "../core/SceneState.h"
#include "../resource/ResourceManager.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <unordered_map>
#include <stdexcept>

namespace {
class SimpleTexture2D final : public Texture2D {
public:
    SimpleTexture2D(unsigned int rendererId, unsigned int width, unsigned int height, bool owns)
        : m_RendererID(rendererId), m_Width(width), m_Height(height), m_Owns(owns) {}

    ~SimpleTexture2D() override {
        if (m_Owns && m_RendererID != 0) {
            glDeleteTextures(1, &m_RendererID);
        }
    }

    unsigned int GetWidth() const override { return m_Width; }
    unsigned int GetHeight() const override { return m_Height; }
    void Bind(unsigned int slot) const override {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
    }

private:
    unsigned int m_RendererID = 0;
    unsigned int m_Width = 0;
    unsigned int m_Height = 0;
    bool m_Owns = false;
};

struct Renderer2DData {
    Ref<VertexArray> QuadVertexArray;
    Ref<VertexBuffer> QuadVertexBuffer;
    Ref<IndexBuffer> QuadIndexBuffer;
    Ref<Material> QuadMaterial;
    Ref<Texture2D> WhiteTexture;
    std::unordered_map<unsigned int, Ref<Texture2D>> TextureCache;
};

Renderer2DData s_Data;
}

bool Renderer2D::init(GLFWwindow* window) {
    m_Window = window;
    const std::filesystem::path projectRoot = std::filesystem::path(__FILE__).parent_path().parent_path().parent_path();
    const std::filesystem::path shaderPath = projectRoot / "assets" / "shaders" / "Renderer2D_Quad.glsl";

    m_ShaderLibrary = CreateRef<ShaderLibrary>();
    m_ShaderLibrary->Load("Renderer2D_Quad", shaderPath.string());
    Renderer2D::Init(m_ShaderLibrary);
    m_CameraInitialized = false;
    createSceneRenderTarget();
    m_InstanceInitialized = true;
    return true;
}

void Renderer2D::destroy() {
    destroySceneRenderTarget();
    if (m_InstanceInitialized) {
        Renderer2D::Shutdown();
        m_ShaderLibrary.reset();
        m_InstanceInitialized = false;
    }
}

void Renderer2D::clear() {
}

void Renderer2D::resizeSceneRenderTarget(int width, int height) {
    const int clampedWidth = std::max(width, 1);
    const int clampedHeight = std::max(height, 1);
    if (clampedWidth == m_SceneRenderTargetWidth && clampedHeight == m_SceneRenderTargetHeight) {
        return;
    }

    m_SceneRenderTargetWidth = clampedWidth;
    m_SceneRenderTargetHeight = clampedHeight;
    createSceneRenderTarget();
}

void Renderer2D::renderScene(const SceneState& sceneState, ResourceManager& resourceManager, float deltaSeconds) {
    static_cast<void>(resourceManager);
    if (!m_InstanceInitialized || m_SceneFramebuffer == 0) {
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, m_SceneFramebuffer);
    glViewport(0, 0, m_SceneRenderTargetWidth, m_SceneRenderTargetHeight);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!m_CameraInitialized) {
        m_CameraPositionX = m_SceneRenderTargetWidth * 0.5f;
        m_CameraPositionY = m_SceneRenderTargetHeight * 0.5f;
        m_CameraInitialized = true;
    }

    updateCamera(deltaSeconds);

    const float halfWidth = (m_SceneRenderTargetWidth * 0.5f) * m_ZoomLevel;
    const float halfHeight = (m_SceneRenderTargetHeight * 0.5f) * m_ZoomLevel;
    m_SceneCamera.SetProjection(
        m_CameraPositionX - halfWidth,
        m_CameraPositionX + halfWidth,
        m_CameraPositionY - halfHeight,
        m_CameraPositionY + halfHeight);
    m_SceneCamera.SetPosition(m_CameraPositionX, m_CameraPositionY, 0.0f);
    m_SceneCamera.SetRotation(m_CameraRotation);

    Renderer2D::BeginScene(m_SceneCamera);
    for (std::size_t index = 0; index < sceneState.objects.size(); ++index) {
        const GameObject& object = sceneState.objects[index];

        Transform transform;
        const float width = 64.0f * std::max(object.scale[0], 0.0f);
        const float height = 64.0f * std::max(object.scale[1], 0.0f);
        transform.Translation = {
            object.position[0] + width * 0.5f,
            object.position[1] + height * 0.5f,
            0.0f
        };
        transform.Rotation = { 0.0f, 0.0f, object.rotation * 0.01745329252f };
        transform.Scale = { width, height, 1.0f };

        Vector4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
        if (!object.texturePath.empty()) {
            if (const TextureResourceInfo* textureInfo = resourceManager.getTexture(object.texturePath)) {
                const unsigned int rendererId = textureInfo->rendererId;
                if (rendererId != 0) {
                    auto found = s_Data.TextureCache.find(rendererId);
                    if (found == s_Data.TextureCache.end()) {
                        auto wrapped = CreateRef<SimpleTexture2D>(
                            rendererId,
                            static_cast<unsigned int>(textureInfo->width),
                            static_cast<unsigned int>(textureInfo->height),
                            false);
                        found = s_Data.TextureCache.emplace(rendererId, wrapped).first;
                    }
                    s_Data.QuadMaterial->SetTexture("u_Texture", found->second, 0);
                } else {
                    s_Data.QuadMaterial->SetTexture("u_Texture", s_Data.WhiteTexture, 0);
                }
            } else {
                s_Data.QuadMaterial->SetTexture("u_Texture", s_Data.WhiteTexture, 0);
            }
        } else {
            s_Data.QuadMaterial->SetTexture("u_Texture", s_Data.WhiteTexture, 0);
        }

        s_Data.QuadMaterial->SetFloat4("u_Color", color);
        Renderer::Submit(s_Data.QuadMaterial, s_Data.QuadVertexArray, transform);
    }
    Renderer2D::EndScene();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

SceneViewportImage Renderer2D::getSceneViewportImage() const {
    SceneViewportImage image;
    image.Handle = reinterpret_cast<void*>(static_cast<intptr_t>(m_SceneColorAttachment));
    image.Width = static_cast<float>(m_SceneRenderTargetWidth);
    image.Height = static_cast<float>(m_SceneRenderTargetHeight);
    return image;
}

void Renderer2D::Init(const Ref<ShaderLibrary>& shaderLibrary, const std::string& shaderName) {
    const float vertices[] = {
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, 0.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f,  0.0f, 1.0f
    };

    const unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    s_Data.QuadVertexArray = VertexArray::Create();
    s_Data.QuadVertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
    s_Data.QuadVertexBuffer->SetLayout({
        { ShaderDataType::Float3, "a_Position" },
        { ShaderDataType::Float2, "a_TexCoord" }
    });
    s_Data.QuadIndexBuffer = IndexBuffer::Create(indices, 6);

    s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);
    s_Data.QuadVertexArray->SetIndexBuffer(s_Data.QuadIndexBuffer);

    s_Data.QuadMaterial = CreateRef<Material>(shaderLibrary->Get(shaderName));

    unsigned int whiteTexture = 0;
    glGenTextures(1, &whiteTexture);
    glBindTexture(GL_TEXTURE_2D, whiteTexture);
    const unsigned int whitePixel = 0xffffffff;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &whitePixel);
    s_Data.WhiteTexture = CreateRef<SimpleTexture2D>(whiteTexture, 1, 1, true);

    s_Data.QuadMaterial->SetTexture("u_Texture", s_Data.WhiteTexture, 0);
    s_Data.QuadMaterial->SetFloat4("u_Color", { 1.0f, 1.0f, 1.0f, 1.0f });
}

void Renderer2D::Shutdown() {
    s_Data.QuadMaterial.reset();
    s_Data.QuadIndexBuffer.reset();
    s_Data.QuadVertexBuffer.reset();
    s_Data.QuadVertexArray.reset();
    s_Data.WhiteTexture.reset();
    s_Data.TextureCache.clear();
}

void Renderer2D::BeginScene(const OrthographicCamera& camera) {
    Renderer::BeginScene(camera);
}

void Renderer2D::EndScene() {
    Renderer::EndScene();
}

void Renderer2D::DrawQuad(const Transform& transform, const Vector4& color) {
    s_Data.QuadMaterial->SetFloat4("u_Color", color);
    Renderer::Submit(s_Data.QuadMaterial, s_Data.QuadVertexArray, transform);
}

void Renderer2D::DrawQuad(const Vector3& position, const Vector3& size, const Vector4& color) {
    Transform transform;
    transform.Translation = position;
    transform.Scale = size;
    DrawQuad(transform, color);
}

void Renderer2D::OnMouseScrolled(float xOffset, float yOffset) {
    static_cast<void>(xOffset);
    m_ZoomLevel -= yOffset * 0.1f;
    m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
}

void Renderer2D::createSceneRenderTarget() {
    destroySceneRenderTarget();

    glGenFramebuffers(1, &m_SceneFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_SceneFramebuffer);

    glGenTextures(1, &m_SceneColorAttachment);
    glBindTexture(GL_TEXTURE_2D, m_SceneColorAttachment);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        m_SceneRenderTargetWidth,
        m_SceneRenderTargetHeight,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_SceneColorAttachment, 0);

    glGenRenderbuffers(1, &m_SceneDepthAttachment);
    glBindRenderbuffer(GL_RENDERBUFFER, m_SceneDepthAttachment);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_SceneRenderTargetWidth, m_SceneRenderTargetHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_SceneDepthAttachment);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        throw std::runtime_error("Scene framebuffer is incomplete.");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer2D::destroySceneRenderTarget() {
    if (m_SceneDepthAttachment != 0) {
        glDeleteRenderbuffers(1, &m_SceneDepthAttachment);
        m_SceneDepthAttachment = 0;
    }

    if (m_SceneColorAttachment != 0) {
        glDeleteTextures(1, &m_SceneColorAttachment);
        m_SceneColorAttachment = 0;
    }

    if (m_SceneFramebuffer != 0) {
        glDeleteFramebuffers(1, &m_SceneFramebuffer);
        m_SceneFramebuffer = 0;
    }
}

void Renderer2D::updateCamera(float deltaSeconds) {
    if (m_Window == nullptr) {
        return;
    }

    const float moveSpeed = m_CameraTranslationSpeed * deltaSeconds;
    if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS) {
        m_CameraPositionX -= moveSpeed;
    }
    if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS) {
        m_CameraPositionX += moveSpeed;
    }
    if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS) {
        m_CameraPositionY += moveSpeed;
    }
    if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS) {
        m_CameraPositionY -= moveSpeed;
    }

    if (glfwGetKey(m_Window, GLFW_KEY_Q) == GLFW_PRESS) {
        m_CameraRotation += m_CameraRotationSpeed * deltaSeconds;
    }
    if (glfwGetKey(m_Window, GLFW_KEY_E) == GLFW_PRESS) {
        m_CameraRotation -= m_CameraRotationSpeed * deltaSeconds;
    }
}
