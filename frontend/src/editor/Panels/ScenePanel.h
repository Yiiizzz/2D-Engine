#pragma once
#include <SDL3/SDL.h>
#include "../../../backend/core/SceneState.h"
#include "../../EditorState.h"

inline constexpr const char* kScenePanelWindowName = "SceneView";

void DrawScenePanel(SceneState& sceneState, EditorState& editorState, SDL_Texture* sceneTexture);
