#include "Renderer2D.h"

#include "Buffer.h"
#include "Material.h"
#include "OrthographicCamera.h"
#include "Renderer.h"
#include "ShaderLibrary.h"
#include "VertexArray.h"

namespace {
struct Renderer2DData {
    Ref<VertexArray> QuadVertexArray;
    Ref<VertexBuffer> QuadVertexBuffer;
    Ref<IndexBuffer> QuadIndexBuffer;
    Ref<Material> QuadMaterial;
};

Renderer2DData s_Data;
}

void Renderer2D::Init(const Ref<ShaderLibrary>& shaderLibrary, const std::string& shaderName) {
    const float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };

    const unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    s_Data.QuadVertexArray = VertexArray::Create();
    s_Data.QuadVertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
    s_Data.QuadVertexBuffer->SetLayout({
        { ShaderDataType::Float3, "a_Position" }
    });
    s_Data.QuadIndexBuffer = IndexBuffer::Create(indices, 6);

    s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);
    s_Data.QuadVertexArray->SetIndexBuffer(s_Data.QuadIndexBuffer);

    s_Data.QuadMaterial = CreateRef<Material>(shaderLibrary->Get(shaderName));
    s_Data.QuadMaterial->SetFloat4("u_Color", { 1.0f, 1.0f, 1.0f, 1.0f });
}

void Renderer2D::Shutdown() {
    s_Data.QuadMaterial.reset();
    s_Data.QuadIndexBuffer.reset();
    s_Data.QuadVertexBuffer.reset();
    s_Data.QuadVertexArray.reset();
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
