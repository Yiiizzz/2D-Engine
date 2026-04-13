#pragma once

#include <string>

#include "../core/Ref.h"
#include "Math.h"
#include "OrthographicCamera.h"
#include "SceneViewportImage.h"

struct GLFWwindow;
class OrthographicCamera;
struct SceneState;
class ResourceManager;
class ShaderLibrary;

class Renderer2D {
public:
    bool init(GLFWwindow* window);
    void destroy();
    void clear();
    void resizeSceneRenderTarget(int width, int height);
    void renderScene(const SceneState& sceneState, ResourceManager& resourceManager, float deltaSeconds);
    SceneViewportImage getSceneViewportImage() const;
    void OnMouseScrolled(float xOffset, float yOffset);

    static void Init(const Ref<ShaderLibrary>& shaderLibrary, const std::string& shaderName = "Renderer2D_Quad");
    static void Shutdown();
    static void BeginScene(const OrthographicCamera& camera);
    static void EndScene();
    static void DrawQuad(const Transform& transform, const Vector4& color);
    static void DrawQuad(const Vector3& position, const Vector3& size, const Vector4& color);

private:
    void createSceneRenderTarget();
    void destroySceneRenderTarget();
    void updateCamera(float deltaSeconds);

private:
    GLFWwindow* m_Window = nullptr;
    Ref<ShaderLibrary> m_ShaderLibrary;
    OrthographicCamera m_SceneCamera{ 0.0f, 1.0f, 1.0f, 0.0f };
    unsigned int m_SceneFramebuffer = 0;
    unsigned int m_SceneColorAttachment = 0;
    unsigned int m_SceneDepthAttachment = 0;
    int m_SceneRenderTargetWidth = 1;
    int m_SceneRenderTargetHeight = 1;
    bool m_InstanceInitialized = false;
    bool m_CameraInitialized = false;
    float m_CameraPositionX = 0.0f;
    float m_CameraPositionY = 0.0f;
    float m_CameraRotation = 0.0f;
    float m_ZoomLevel = 1.0f;
    float m_CameraTranslationSpeed = 260.0f;
    float m_CameraRotationSpeed = 1.5f;
};
