#pragma once

#include <string>

#include "../core/Ref.h"
#include "Math.h"

class OrthographicCamera;
class ShaderLibrary;

class Renderer2D {
public:
    static void Init(const Ref<ShaderLibrary>& shaderLibrary, const std::string& shaderName = "Renderer2D_Quad");
    static void Shutdown();
    static void BeginScene(const OrthographicCamera& camera);
    static void EndScene();
    static void DrawQuad(const Transform& transform, const Vector4& color);
    static void DrawQuad(const Vector3& position, const Vector3& size, const Vector4& color);
};
