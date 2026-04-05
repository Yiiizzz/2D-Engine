#include "InspectorPanel.h"
#include "imgui.h"
#include <cstring>

void DrawInspectorPanel(SceneState& sceneState, EditorState& editorState)
{
    ImGui::Begin("Inspector");
    ImGui::PushTextWrapPos(0.0f);
    ImGui::PushItemWidth(-1.0f);

    static int lastSelectedIndex = -1;
    static char textureBuffer[128] = "";

    int index = editorState.selectedObjectIndex;
    if (index >= 0 && index < static_cast<int>(sceneState.objects.size())) {
        auto& obj = sceneState.objects[index];

        if (lastSelectedIndex != index) {
            strncpy(textureBuffer, obj.texturePath.c_str(), sizeof(textureBuffer));
            textureBuffer[sizeof(textureBuffer) - 1] = '\0';
            lastSelectedIndex = index;
        }

        ImGui::TextUnformatted("Selected Object");
        ImGui::Separator();
        ImGui::TextWrapped("Name: %s", obj.name.c_str());
        ImGui::TextDisabled("GameObject ID");
        ImGui::TextWrapped("%d", obj.id);
        ImGui::TextDisabled("Texture Resource ID");
        ImGui::TextWrapped("%llu", static_cast<unsigned long long>(obj.textureResourceId));
        ImGui::TextDisabled("Script Resource ID");
        ImGui::TextWrapped("%llu", static_cast<unsigned long long>(obj.scriptResourceId));
        ImGui::Spacing();

        ImGui::TextUnformatted("Rendering");
        ImGui::TextDisabled("Bind imported project assets or enter a relative texture path.");
        if (ImGui::InputText("Texture Path", textureBuffer, sizeof(textureBuffer))) {
            obj.texturePath = textureBuffer;
            const AssetRecord* record = editorState.assetRegistry.findByPath(obj.texturePath);
            obj.textureResourceId = record ? record->id : 0;
        }

        ImGui::TextWrapped("Bound Texture: %s", obj.texturePath.c_str());
        ImGui::Spacing();
        ImGui::TextUnformatted("Script");
        ImGui::TextDisabled("Bind a project C++ script to update this object while Play mode is active.");
        ImGui::TextWrapped("Bound Script: %s", obj.scriptPath.empty() ? "None" : obj.scriptPath.c_str());
        if (ImGui::BeginCombo("Script Asset", obj.scriptPath.empty() ? "None" : obj.scriptPath.c_str())) {
            const bool noScriptSelected = obj.scriptPath.empty();
            if (ImGui::Selectable("None", noScriptSelected)) {
                obj.scriptResourceId = 0;
                obj.scriptPath.clear();
            }

            for (const AssetRecord& asset : editorState.assetRegistry.getAssets()) {
                if (asset.type != AssetType::Script) {
                    continue;
                }

                const std::string label = asset.relativePath.empty() ? asset.name : asset.relativePath;
                const bool selected = (obj.scriptResourceId == asset.id);
                if (ImGui::Selectable(label.c_str(), selected)) {
                    obj.scriptResourceId = asset.id;
                    obj.scriptPath = asset.sourcePath;
                }
            }
            ImGui::EndCombo();
        }
        ImGui::Spacing();
        ImGui::TextUnformatted("Transform");
        ImGui::TextDisabled("Use Scene or Hierarchy selection to edit this object.");
        ImGui::TextDisabled("Position");
        ImGui::InputFloat2("##Position", obj.position);
        ImGui::TextDisabled("Scale");
        ImGui::InputFloat2("##Scale", obj.scale);
        ImGui::TextDisabled("Rotation");
        ImGui::InputFloat("##Rotation", &obj.rotation);
        if (!editorState.scriptStatus.empty()) {
            ImGui::Spacing();
            ImGui::TextWrapped("Script Status: %s", editorState.scriptStatus.c_str());
        }
    }
    else {
        ImGui::TextUnformatted("No object selected");
        ImGui::Separator();
        ImGui::TextWrapped("Select a GameObject in Hierarchy or Scene to edit its transform and texture binding here.");
        lastSelectedIndex = -1;
    }

    ImGui::PopItemWidth();
    ImGui::PopTextWrapPos();
    ImGui::End();
}
