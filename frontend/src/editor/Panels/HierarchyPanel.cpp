#include "HierarchyPanel.h"

#include "../EditorActions.h"
#include "imgui.h"

#include <array>
#include <cstdint>
#include <cstdio>

void DrawHierarchyPanel(SceneState& sceneState, EditorState& editorState)
{
    ImGui::Begin("Hierarchy", &editorState.showHierarchy);

    std::array<char, 128> searchBuffer{};
    std::snprintf(searchBuffer.data(), searchBuffer.size(), "%s", editorState.hierarchySearch.c_str());
    if (ImGui::InputTextWithHint("##HierarchySearch", "Search objects...", searchBuffer.data(), searchBuffer.size())) {
        editorState.hierarchySearch = searchBuffer.data();
    }

    ImGui::SameLine();
    if (ImGui::Button("+")) {
        CreateEmptyObject(sceneState, editorState, "GameObject", "Hierarchy");
    }

    ImGuiTreeNodeFlags rootFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth;
    const bool sceneOpen = ImGui::TreeNodeEx("Active Scene", rootFlags, "Active Scene (%d)", static_cast<int>(sceneState.objects.size()));

    if (ImGui::BeginPopupContextWindow("HierarchyWindowContext", ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight)) {
        if (ImGui::MenuItem("Create Empty")) {
            CreateEmptyObject(sceneState, editorState, "GameObject", "Hierarchy context");
        }
        ImGui::EndPopup();
    }

    if (sceneOpen) {
        for (int i = 0; i < static_cast<int>(sceneState.objects.size()); ++i) {
            GameObject& object = sceneState.objects[i];
            if (!ContainsInsensitive(object.name, editorState.hierarchySearch)) {
                continue;
            }

            ImGuiTreeNodeFlags itemFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth;
            if (editorState.selectedObjectIndex == i) {
                itemFlags |= ImGuiTreeNodeFlags_Selected;
            }

            ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<intptr_t>(object.id)), itemFlags, "%s", object.name.c_str());
            if (ImGui::IsItemClicked()) {
                editorState.selectedObjectIndex = i;
                FocusAssetPath(editorState, object.texturePath);
            }

            if (ImGui::BeginPopupContextItem()) {
                if (ImGui::MenuItem("Duplicate")) {
                    editorState.selectedObjectIndex = i;
                    DuplicateSelectedObject(sceneState, editorState, "Hierarchy context");
                }

                if (ImGui::MenuItem("Delete")) {
                    editorState.selectedObjectIndex = i;
                    DeleteSelectedObject(sceneState, editorState);
                }

                ImGui::EndPopup();
            }
        }
        ImGui::TreePop();
    }

    ImGui::End();
}
