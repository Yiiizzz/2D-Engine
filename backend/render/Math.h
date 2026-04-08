#pragma once

class Matrix4 {
public:
    Matrix4();

    const float* Data() const;
    float* Data();

    static Matrix4 Identity();
    static Matrix4 Orthographic(float left, float right, float bottom, float top, float nearClip, float farClip);
    static Matrix4 Translation(float x, float y, float z);

    Matrix4 operator*(const Matrix4& other) const;

private:
    float m_Elements[16];
};
