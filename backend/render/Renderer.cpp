#include "Renderer.h"

#include "OrthographicCamera.h"
#include "RenderCommand.h"
#include "Shader.h"
#include "VertexArray.h"

Renderer::SceneData Renderer::s_SceneData;

void Renderer::Init(GraphicsAPI api) {
    RenderCommand::Init(api);
}

void Renderer::Shutdown() {
    s_SceneData.Camera = nullptr;
}

void Renderer::OnWindowResize(unsigned int width, unsigned int height) {
    RenderCommand::SetViewport(0, 0, width, height);
}

void Renderer::BeginScene(const OrthographicCamera& camera) {
    s_SceneData.Camera = &camera;
}

void Renderer::EndScene() {
}

void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray) {
    if (s_SceneData.Camera == nullptr) {
        return;
    }

    shader->Bind();
    shader->SetMat4("u_ViewProjection", s_SceneData.Camera->GetViewProjectionMatrix());
    vertexArray->Bind();
    RenderCommand::DrawIndexed(*vertexArray);
}
