#include "RenderCommand.h"

#include "VertexArray.h"

std::unique_ptr<RendererAPI> RenderCommand::s_RendererAPI = nullptr;

void RenderCommand::Init(GraphicsAPI api) {
    RendererAPI::SetAPI(api);
    s_RendererAPI = RendererAPI::Create(api);
    s_RendererAPI->Init();
}

void RenderCommand::SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
    s_RendererAPI->SetViewport(x, y, width, height);
}

void RenderCommand::SetClearColor(const Color& color) {
    s_RendererAPI->SetClearColor(color);
}

void RenderCommand::Clear() {
    s_RendererAPI->Clear();
}

void RenderCommand::DrawIndexed(const VertexArray& vertexArray, unsigned int indexCount) {
    s_RendererAPI->DrawIndexed(vertexArray, indexCount);
}
