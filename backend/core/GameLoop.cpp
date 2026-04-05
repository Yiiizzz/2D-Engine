#include "GameLoop.h"
#include <SDL3/SDL.h>

void GameLoop::update(SceneState& sceneState, EditorState& editorState)
{
    const Uint64 now = SDL_GetTicks();
    float deltaTime = 1.0f / 60.0f;
    if (lastTickMs != 0 && now > lastTickMs) {
        deltaTime = static_cast<float>(now - lastTickMs) / 1000.0f;
    }
    lastTickMs = now;

    if (editorState.mode != EditorMode::Play) {
        if (wasPlaying) {
            scriptRuntime.reset();
            wasPlaying = false;
        }
        return;
    }

    wasPlaying = true;
    editorState.scriptStatus.clear();
    for (auto& object : sceneState.objects) {
        if (object.scriptPath.empty()) {
            continue;
        }

        scriptRuntime.execute(object, object.scriptPath, editorState.projectRootPath, deltaTime, editorState.scriptStatus);
    }
    // 暂时先不做复杂逻辑
    // 后面这里负责移动、动画、碰撞、脚本等
}
