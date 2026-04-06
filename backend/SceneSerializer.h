#pragma once
#include "core/SceneState.h"
#include "../frontend/src/EditorState.h"
#include <string>

bool SaveSceneToFile(const SceneState& sceneState, const std::string& sceneName, const std::string& path);
bool LoadSceneFromFile(SceneState& sceneState, EditorState& editorState, std::string& sceneName, const std::string& path);