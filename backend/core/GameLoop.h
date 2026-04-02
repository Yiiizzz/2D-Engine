#pragma once
#include "SceneState.h"
#include "../../frontend/src/EditorState.h"

class GameLoop {
public:
    void update(SceneState& sceneState, EditorState& editorState);
};