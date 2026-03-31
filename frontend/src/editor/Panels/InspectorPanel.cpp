#include "InspectorPanel.h"
#include "imgui.h"

void DrawInspectorPanel(SceneState& sceneState, EditorState& editorState)
{
    ImGui::Begin("Inspector");

    int index = editorState.selectedObjectIndex;
    if (index >= 0 && index < static_cast<int>(sceneState.objects.size())) {
        auto& obj = sceneState.objects[index];

        ImGui::Text("Selected Object");
        ImGui::Separator();
        ImGui::Text("Name: %s", obj.name.c_str());
        ImGui::Text("ID: %d", obj.id);

        ImGui::InputFloat2("Position", obj.position);
        ImGui::InputFloat2("Scale", obj.scale);
    }
    else {
        ImGui::Text("No object selected");
    }

    ImGui::End();
}