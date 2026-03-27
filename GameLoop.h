#pragma once
#include "SceneState.h"
#include "EditorState.h"

class GameLoop {
public:
    void update(SceneState& sceneState, EditorState& editorState);
};