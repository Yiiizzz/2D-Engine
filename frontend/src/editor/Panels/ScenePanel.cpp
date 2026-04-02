#include "ScenePanel.h"
#include "imgui.h"

void DrawScenePanel(SceneState& sceneState, EditorState& editorState)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

    ImGui::Begin("Scene");

    const char* modeText = "Edit";
    if (editorState.mode == EditorMode::Play) modeText = "Play";
    else if (editorState.mode == EditorMode::Pause) modeText = "Pause";

    // ===== 顶部状态 =====
    ImGui::Text("Scene Overview");
    ImGui::Separator();
    ImGui::Text("Mode: %s", modeText);
    ImGui::Text("Object count: %d", static_cast<int>(sceneState.objects.size()));

    int index = editorState.selectedObjectIndex;
    if (index >= 0 && index < static_cast<int>(sceneState.objects.size())) {
        ImGui::Text("Selected: %s", sceneState.objects[index].name.c_str());
    }
    else {
        ImGui::Text("Selected: None");
    }

    // ===== 简单操作按钮 =====
    ImGui::Separator();

    if (ImGui::Button("Reset Selected Position")) {
        if (index >= 0 && index < static_cast<int>(sceneState.objects.size())) {
            sceneState.objects[index].position[0] = 0.0f;
            sceneState.objects[index].position[1] = 0.0f;
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Reset Selected Scale")) {
        if (index >= 0 && index < static_cast<int>(sceneState.objects.size())) {
            sceneState.objects[index].scale[0] = 1.0f;
            sceneState.objects[index].scale[1] = 1.0f;
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Add Test Object")) {
        GameObject newObject;
        newObject.id = static_cast<int>(sceneState.objects.size());
        newObject.name = "New Object " + std::to_string(newObject.id);
        newObject.position[0] = 0.0f;
        newObject.position[1] = 0.0f;
        newObject.scale[0] = 1.0f;
        newObject.scale[1] = 1.0f;

        sceneState.objects.push_back(newObject);
        editorState.selectedObjectIndex = static_cast<int>(sceneState.objects.size()) - 1;
    }

    ImGui::SameLine();

    if (ImGui::Button("Delete Selected")) {
        if (index >= 0 && index < static_cast<int>(sceneState.objects.size())) {
            sceneState.objects.erase(sceneState.objects.begin() + index);

            if (sceneState.objects.empty()) {
                editorState.selectedObjectIndex = -1;
            }
            else if (index >= static_cast<int>(sceneState.objects.size())) {
                editorState.selectedObjectIndex = static_cast<int>(sceneState.objects.size()) - 1;
            }
        }
    }

    // ===== 全部对象列表 =====
    ImGui::Separator();
    ImGui::Text("Scene Objects:");

    for (int i = 0; i < static_cast<int>(sceneState.objects.size()); ++i) {
        auto& obj = sceneState.objects[i];
        bool selected = (editorState.selectedObjectIndex == i);

        // 左边：对象名可点击
        if (ImGui::Selectable(obj.name.c_str(), selected, 0, ImVec2(150, 0))) {
            editorState.selectedObjectIndex = i;
        }

        // 右边：显示位置和缩放
        ImGui::SameLine(180);
        ImGui::Text("Pos(%.1f, %.1f)", obj.position[0], obj.position[1]);

        ImGui::SameLine(320);
        ImGui::Text("Scale(%.1f, %.1f)", obj.scale[0], obj.scale[1]);
    }

    // ===== 当前选中对象详细信息 =====
    ImGui::Separator();
    if (index >= 0 && index < static_cast<int>(sceneState.objects.size())) {
        auto& obj = sceneState.objects[index];
        ImGui::Text("Selected Object Details");
        ImGui::Text("Name: %s", obj.name.c_str());
        ImGui::Text("ID: %d", obj.id);
        ImGui::Text("Position: %.1f, %.1f", obj.position[0], obj.position[1]);
        ImGui::Text("Scale: %.1f, %.1f", obj.scale[0], obj.scale[1]);
    }
    else {
        ImGui::Text("No object selected");
    }

    ImGui::End();

    ImGui::PopStyleVar(2);
}