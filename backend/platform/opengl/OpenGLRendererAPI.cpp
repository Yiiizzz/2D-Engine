#include "OpenGLRendererAPI.h"

#include "../../render/Buffer.h"
#include "../../render/VertexArray.h"

#include <glad/glad.h>

void OpenGLRendererAPI::Init() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
}

void OpenGLRendererAPI::SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
    glViewport(static_cast<int>(x), static_cast<int>(y), static_cast<int>(width), static_cast<int>(height));
}

void OpenGLRendererAPI::SetClearColor(const Color& color) {
    glClearColor(color.r, color.g, color.b, color.a);
}

void OpenGLRendererAPI::Clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRendererAPI::DrawIndexed(const VertexArray& vertexArray, unsigned int indexCount) {
    vertexArray.Bind();

    const unsigned int count = indexCount == 0
        ? vertexArray.GetIndexBuffer()->GetCount()
        : indexCount;

    glDrawElements(GL_TRIANGLES, static_cast<int>(count), GL_UNSIGNED_INT, nullptr);
}
