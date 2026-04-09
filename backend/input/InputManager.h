#pragma once

class WindowManager;
struct SceneState;
struct EditorState;

class InputManager {
public:
    InputManager();
    void processEvents(WindowManager& windowManager, SceneState& sceneState, EditorState& editorState);
    bool shouldQuit() const;

private:
    bool m_QuitRequested = false;
    bool m_PreviousEscapePressed = false;
    bool m_PreviousFullscreenPressed = false;
    bool m_PreviousResize1Pressed = false;
    bool m_PreviousResize2Pressed = false;
    bool m_PreviousResize3Pressed = false;
    bool m_PreviousLeftMousePressed = false;
};
