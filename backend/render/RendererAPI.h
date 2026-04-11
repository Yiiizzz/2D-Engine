#pragma once

#include "../core/Ref.h"
#include "GraphicsAPI.h"

class VertexArray;

struct Color {
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 1.0f;
};

class RendererAPI {
public:
    virtual ~RendererAPI() = default;

    virtual void Init() = 0;
    virtual void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) = 0;
    virtual void SetClearColor(const Color& color) = 0;
    virtual void Clear() = 0;
    virtual void DrawIndexed(const VertexArray& vertexArray, unsigned int indexCount = 0) = 0;

    static Scope<RendererAPI> Create(GraphicsAPI api);
    static GraphicsAPI GetAPI();
    static void SetAPI(GraphicsAPI api);

private:
    static GraphicsAPI s_API;
};
