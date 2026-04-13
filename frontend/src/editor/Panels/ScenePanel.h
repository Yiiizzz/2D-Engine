#pragma once

#include "../../../backend/core/SceneState.h"
#include "../../../backend/render/SceneViewportImage.h"
#include "../../EditorState.h"

inline constexpr const char* kScenePanelWindowName = "Scene";

void DrawScenePanel(SceneState& sceneState, EditorState& editorState, const SceneViewportImage& sceneImage);
