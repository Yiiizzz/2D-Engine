#pragma once

#include "../core/Ref.h"
#include "RendererAPI.h"

class RenderCommand {
public:
    static void Init(GraphicsAPI api);
    static void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
    static void SetClearColor(const Color& color);
    static void Clear();
    static void DrawIndexed(const VertexArray& vertexArray, unsigned int indexCount = 0);

private:
    static Scope<RendererAPI> s_RendererAPI;
};
