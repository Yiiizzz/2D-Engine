#pragma once
#include "SceneState.h"
#include "../../frontend/src/EditorState.h"
#include "../script/ScriptRuntime.h"

class GameLoop {
public:
    void update(SceneState& sceneState, EditorState& editorState);

private:
    ScriptRuntime scriptRuntime;
    unsigned long long lastTickMs = 0;
    bool wasPlaying = false;
};
