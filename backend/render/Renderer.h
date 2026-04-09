#pragma once

#include <memory>

#include "GraphicsAPI.h"

class OrthographicCamera;
class Shader;
class VertexArray;

class Renderer {
public:
    static void Init(GraphicsAPI api);
    static void Shutdown();
    static void OnWindowResize(unsigned int width, unsigned int height);
    static void BeginScene(const OrthographicCamera& camera);
    static void EndScene();
    static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray);

private:
    struct SceneData {
        const OrthographicCamera* Camera = nullptr;
    };

    static SceneData s_SceneData;
};
