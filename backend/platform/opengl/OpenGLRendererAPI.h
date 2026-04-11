#pragma once

#include "../../render/RendererAPI.h"

class OpenGLRendererAPI : public RendererAPI {
public:
    void Init() override;
    void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) override;
    void SetClearColor(const Color& color) override;
    void Clear() override;
    void DrawIndexed(const VertexArray& vertexArray, unsigned int indexCount = 0) override;
};
