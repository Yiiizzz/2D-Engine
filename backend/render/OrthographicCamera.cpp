#include "OrthographicCamera.h"

#include <cmath>

Matrix4::Matrix4() : m_Elements{ 0.0f } {}

const float* Matrix4::Data() const {
    return m_Elements;
}

float* Matrix4::Data() {
    return m_Elements;
}

Matrix4 Matrix4::Identity() {
    Matrix4 result;
    result.m_Elements[0] = 1.0f;
    result.m_Elements[5] = 1.0f;
    result.m_Elements[10] = 1.0f;
    result.m_Elements[15] = 1.0f;
    return result;
}

Matrix4 Matrix4::Orthographic(float left, float right, float bottom, float top, float nearClip, float farClip) {
    Matrix4 result = Identity();

    result.m_Elements[0] = 2.0f / (right - left);
    result.m_Elements[5] = 2.0f / (top - bottom);
    result.m_Elements[10] = -2.0f / (farClip - nearClip);
    result.m_Elements[12] = -(right + left) / (right - left);
    result.m_Elements[13] = -(top + bottom) / (top - bottom);
    result.m_Elements[14] = -(farClip + nearClip) / (farClip - nearClip);

    return result;
}

Matrix4 Matrix4::Translation(float x, float y, float z) {
    Matrix4 result = Identity();
    result.m_Elements[12] = x;
    result.m_Elements[13] = y;
    result.m_Elements[14] = z;
    return result;
}

Matrix4 Matrix4::Scale(float x, float y, float z) {
    Matrix4 result = Identity();
    result.m_Elements[0] = x;
    result.m_Elements[5] = y;
    result.m_Elements[10] = z;
    return result;
}

Matrix4 Matrix4::RotationX(float radians) {
    Matrix4 result = Identity();
    const float cosine = std::cos(radians);
    const float sine = std::sin(radians);

    result.m_Elements[5] = cosine;
    result.m_Elements[6] = sine;
    result.m_Elements[9] = -sine;
    result.m_Elements[10] = cosine;

    return result;
}

Matrix4 Matrix4::RotationY(float radians) {
    Matrix4 result = Identity();
    const float cosine = std::cos(radians);
    const float sine = std::sin(radians);

    result.m_Elements[0] = cosine;
    result.m_Elements[2] = -sine;
    result.m_Elements[8] = sine;
    result.m_Elements[10] = cosine;

    return result;
}

Matrix4 Matrix4::RotationZ(float radians) {
    Matrix4 result = Identity();
    const float cosine = std::cos(radians);
    const float sine = std::sin(radians);

    result.m_Elements[0] = cosine;
    result.m_Elements[1] = sine;
    result.m_Elements[4] = -sine;
    result.m_Elements[5] = cosine;

    return result;
}

Matrix4 Matrix4::operator*(const Matrix4& other) const {
    Matrix4 result;

    for (int row = 0; row < 4; ++row) {
        for (int column = 0; column < 4; ++column) {
            float sum = 0.0f;
            for (int i = 0; i < 4; ++i) {
                sum += m_Elements[row + i * 4] * other.m_Elements[i + column * 4];
            }
            result.m_Elements[row + column * 4] = sum;
        }
    }

    return result;
}

OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
    : m_ProjectionMatrix(Matrix4::Orthographic(left, right, bottom, top, -1.0f, 1.0f)),
      m_ViewMatrix(Matrix4::Identity()),
      m_ViewProjectionMatrix(m_ProjectionMatrix * m_ViewMatrix) {
}

void OrthographicCamera::SetProjection(float left, float right, float bottom, float top) {
    m_ProjectionMatrix = Matrix4::Orthographic(left, right, bottom, top, -1.0f, 1.0f);
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void OrthographicCamera::SetPosition(float x, float y, float z) {
    m_Position[0] = x;
    m_Position[1] = y;
    m_Position[2] = z;
    RecalculateViewMatrix();
}

void OrthographicCamera::SetRotation(float rotationRadians) {
    m_Rotation = rotationRadians;
    RecalculateViewMatrix();
}

const Matrix4& OrthographicCamera::GetProjectionMatrix() const {
    return m_ProjectionMatrix;
}

const Matrix4& OrthographicCamera::GetViewMatrix() const {
    return m_ViewMatrix;
}

const Matrix4& OrthographicCamera::GetViewProjectionMatrix() const {
    return m_ViewProjectionMatrix;
}

float OrthographicCamera::GetRotation() const {
    return m_Rotation;
}

void OrthographicCamera::RecalculateViewMatrix() {
    const Matrix4 inverseRotation = Matrix4::RotationZ(-m_Rotation);
    const Matrix4 inverseTranslation = Matrix4::Translation(-m_Position[0], -m_Position[1], -m_Position[2]);
    m_ViewMatrix = inverseRotation * inverseTranslation;
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}
