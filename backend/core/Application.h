#pragma once

#include <memory>

#include "../render/OrthographicCamera.h"
#include "../window/WindowManager.h"

class IndexBuffer;
class Shader;
class VertexArray;
class VertexBuffer;

class Application {
public:
    bool Init();
    void Run();
    void Shutdown();

private:
    void CreateDemoScene();
    void UpdateCamera();

private:
    WindowManager m_Window;
    OrthographicCamera m_Camera = OrthographicCamera(-1.6f, 1.6f, -0.9f, 0.9f);
    std::shared_ptr<VertexArray> m_VertexArray;
    std::shared_ptr<VertexBuffer> m_VertexBuffer;
    std::shared_ptr<IndexBuffer> m_IndexBuffer;
    std::shared_ptr<Shader> m_Shader;
    bool m_Initialized = false;
};
