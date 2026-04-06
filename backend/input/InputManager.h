#pragma once
#include <SDL3/SDL.h>

class WindowManager;
struct SceneState;
struct EditorState;

class InputManager {
private:
    bool quitRequested;

public:
    InputManager();
    void processEvents(WindowManager& windowManager, SceneState& sceneState, EditorState& editorState);
    bool shouldQuit() const;
};
