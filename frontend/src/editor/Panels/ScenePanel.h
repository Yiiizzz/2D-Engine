#pragma once

#include "../../../backend/core/SceneState.h"
#include "../../../backend/render/SceneViewportImage.h"
#include "../../EditorState.h"

void DrawScenePanel(SceneState& sceneState, EditorState& editorState, const SceneViewportImage& sceneImage);
