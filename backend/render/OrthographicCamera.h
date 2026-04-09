#pragma once

#include "Math.h"

class OrthographicCamera {
public:
    OrthographicCamera(float left, float right, float bottom, float top);

    void SetProjection(float left, float right, float bottom, float top);
    void SetPosition(float x, float y, float z);
    void SetRotation(float rotationRadians);

    const Matrix4& GetProjectionMatrix() const;
    const Matrix4& GetViewMatrix() const;
    const Matrix4& GetViewProjectionMatrix() const;
    float GetRotation() const;

private:
    void RecalculateViewMatrix();

private:
    Matrix4 m_ProjectionMatrix;
    Matrix4 m_ViewMatrix;
    Matrix4 m_ViewProjectionMatrix;
    float m_Position[3] = { 0.0f, 0.0f, 0.0f };
    float m_Rotation = 0.0f;
};
