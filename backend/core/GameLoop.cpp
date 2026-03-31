#include "GameLoop.h"

void GameLoop::update(SceneState& sceneState, EditorState& editorState)
{
    if (editorState.mode != EditorMode::Play) {
        return;
    }

    if (!sceneState.objects.empty()) {
        sceneState.objects[0].position[0] += 0.1f;
    }
    // 暂时先不做复杂逻辑
    // 后面这里负责移动、动画、碰撞、脚本等
}