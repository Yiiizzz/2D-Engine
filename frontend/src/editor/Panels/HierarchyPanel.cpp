#include "HierarchyPanel.h"
#include "imgui.h"

void DrawHierarchyPanel(SceneState& sceneState, EditorState& editorState)
{
    ImGui::Begin("Hierarchy");

    for (int i = 0; i < static_cast<int>(sceneState.objects.size()); ++i) {
        bool selected = (editorState.selectedObjectIndex == i);
        if (ImGui::Selectable(sceneState.objects[i].name.c_str(), selected)) {
            editorState.selectedObjectIndex = i;
        }
    }

    ImGui::End();
}