#pragma once

#include "backend/core/Timestep.h"
#include "backend/render/OrthographicCamera.h"

struct GLFWwindow;

class OrthographicCameraController {
public:
    OrthographicCameraController(float aspectRatio, bool rotationEnabled = false);

    void SetWindowHandle(GLFWwindow* windowHandle);
    void OnUpdate(Timestep timestep);
    void OnResize(unsigned int width, unsigned int height);
    void OnMouseScrolled(float xOffset, float yOffset);

    OrthographicCamera& GetCamera();
    const OrthographicCamera& GetCamera() const;

    float GetZoomLevel() const;
    void SetZoomLevel(float zoomLevel);

private:
    void RecalculateProjection();

private:
    OrthographicCamera m_Camera;
    GLFWwindow* m_WindowHandle = nullptr;
    float m_AspectRatio = 1.778f;
    float m_ZoomLevel = 1.0f;
    float m_CameraTranslationSpeed = 2.5f;
    float m_CameraRotationSpeed = 1.5f;
    float m_CameraPositionX = 0.0f;
    float m_CameraPositionY = 0.0f;
    float m_CameraRotation = 0.0f;
    bool m_RotationEnabled = false;
};
