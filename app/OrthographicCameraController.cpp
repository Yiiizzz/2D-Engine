#include "OrthographicCameraController.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <algorithm>

OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotationEnabled)
    : m_Camera(-aspectRatio, aspectRatio, -1.0f, 1.0f),
      m_AspectRatio(aspectRatio),
      m_RotationEnabled(rotationEnabled) {
    RecalculateProjection();
}

void OrthographicCameraController::SetWindowHandle(GLFWwindow* windowHandle) {
    m_WindowHandle = windowHandle;
}

void OrthographicCameraController::OnUpdate(Timestep timestep) {
    if (m_WindowHandle == nullptr) {
        return;
    }

    if (glfwGetKey(m_WindowHandle, GLFW_KEY_A) == GLFW_PRESS) {
        m_CameraPositionX -= m_CameraTranslationSpeed * timestep.GetSeconds();
    }
    if (glfwGetKey(m_WindowHandle, GLFW_KEY_D) == GLFW_PRESS) {
        m_CameraPositionX += m_CameraTranslationSpeed * timestep.GetSeconds();
    }
    if (glfwGetKey(m_WindowHandle, GLFW_KEY_W) == GLFW_PRESS) {
        m_CameraPositionY += m_CameraTranslationSpeed * timestep.GetSeconds();
    }
    if (glfwGetKey(m_WindowHandle, GLFW_KEY_S) == GLFW_PRESS) {
        m_CameraPositionY -= m_CameraTranslationSpeed * timestep.GetSeconds();
    }

    if (m_RotationEnabled) {
        if (glfwGetKey(m_WindowHandle, GLFW_KEY_Q) == GLFW_PRESS) {
            m_CameraRotation += m_CameraRotationSpeed * timestep.GetSeconds();
        }
        if (glfwGetKey(m_WindowHandle, GLFW_KEY_E) == GLFW_PRESS) {
            m_CameraRotation -= m_CameraRotationSpeed * timestep.GetSeconds();
        }
    }

    m_Camera.SetPosition(m_CameraPositionX, m_CameraPositionY, 0.0f);
    if (m_RotationEnabled) {
        m_Camera.SetRotation(m_CameraRotation);
    }
}

void OrthographicCameraController::OnResize(unsigned int width, unsigned int height) {
    if (width == 0 || height == 0) {
        return;
    }

    m_AspectRatio = static_cast<float>(width) / static_cast<float>(height);
    RecalculateProjection();
}

void OrthographicCameraController::OnMouseScrolled(float xOffset, float yOffset) {
    static_cast<void>(xOffset);
    m_ZoomLevel -= yOffset * 0.25f;
    m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
    RecalculateProjection();
}

OrthographicCamera& OrthographicCameraController::GetCamera() {
    return m_Camera;
}

const OrthographicCamera& OrthographicCameraController::GetCamera() const {
    return m_Camera;
}

float OrthographicCameraController::GetZoomLevel() const {
    return m_ZoomLevel;
}

void OrthographicCameraController::SetZoomLevel(float zoomLevel) {
    m_ZoomLevel = std::max(zoomLevel, 0.25f);
    RecalculateProjection();
}

void OrthographicCameraController::RecalculateProjection() {
    m_Camera.SetProjection(
        -m_AspectRatio * m_ZoomLevel,
         m_AspectRatio * m_ZoomLevel,
        -m_ZoomLevel,
         m_ZoomLevel);
}
