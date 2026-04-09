#pragma once

#include <memory>

#include "../core/SceneState.h"
#include "OrthographicCamera.h"
#include "SceneViewportImage.h"

class IndexBuffer;
class ResourceManager;
class Shader;
class VertexArray;
class VertexBuffer;

class Renderer2D {
public:
    Renderer2D();

    bool init(void* windowHandle);
    void clear();
    bool resizeSceneRenderTarget(int width, int height);
    void renderScene(const SceneState& sceneState, ResourceManager& resourceManager);
    SceneViewportImage getSceneViewportImage() const;
    void present();
    void destroy();

private:
    bool createSceneRenderTarget(int width, int height);
    void destroySceneRenderTarget();
    void createQuadResources();

private:
    unsigned int m_Framebuffer = 0;
    unsigned int m_ColorAttachment = 0;
    unsigned int m_DepthAttachment = 0;
    int m_Width = 0;
    int m_Height = 0;
    std::shared_ptr<VertexArray> m_VertexArray;
    std::shared_ptr<VertexBuffer> m_VertexBuffer;
    std::shared_ptr<IndexBuffer> m_IndexBuffer;
    std::shared_ptr<Shader> m_Shader;
    OrthographicCamera m_Camera;
};
