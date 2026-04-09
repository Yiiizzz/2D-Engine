#pragma once

struct GLFWwindow;

class ImGuiLayer {
public:
    bool Init(GLFWwindow* window);
    void Shutdown();
    void BeginFrame();
    void EndFrame();

private:
    bool CreateDeviceObjects();
    void DestroyDeviceObjects();
    bool CreateFontsTexture();
    void DestroyFontsTexture();
    void UpdateMouseData();
    void UpdateMouseCursor() const;

    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void CharCallback(GLFWwindow* window, unsigned int c);

private:
    GLFWwindow* m_Window = nullptr;
    double m_Time = 0.0;
    bool m_MousePressed[5] = { false, false, false, false, false };
    void* m_MouseCursors[9] = {};
    unsigned int m_FontTexture = 0;
    unsigned int m_ShaderHandle = 0;
    unsigned int m_VertHandle = 0;
    unsigned int m_FragHandle = 0;
    int m_AttribLocationTex = 0;
    int m_AttribLocationProjMtx = 0;
    unsigned int m_AttribLocationVtxPos = 0;
    unsigned int m_AttribLocationVtxUV = 0;
    unsigned int m_AttribLocationVtxColor = 0;
    unsigned int m_VboHandle = 0;
    unsigned int m_ElementsHandle = 0;
    unsigned int m_VaoHandle = 0;
};
