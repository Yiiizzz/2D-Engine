#include "ScenePanel.h"
#include "imgui.h"
#include "../../../../backend/SceneSerializer.h"
#include <algorithm>
#include <cstdint>

static std::string sceneStatus = "No scene operation yet";

namespace {

void DrawActionButtonRow(SceneState& sceneState, EditorState& editorState, int index) {
    const float width = ImGui::GetContentRegionAvail().x;
    const int columns = width < 780.0f ? 2 : 4;
    const float spacing = ImGui::GetStyle().ItemSpacing.x;
    const float buttonWidth = (width - spacing * static_cast<float>(columns - 1)) / static_cast<float>(columns);

    auto drawButton = [&](const char* label, auto&& fn) {
        if (ImGui::Button(label, ImVec2(buttonWidth, 0.0f))) {
            fn();
        }
    };

    drawButton("Reset Position", [&]() {
        if (index >= 0 && index < static_cast<int>(sceneState.objects.size())) {
            sceneState.objects[index].position[0] = 0.0f;
            sceneState.objects[index].position[1] = 0.0f;
        }
    });

    if (columns > 1) ImGui::SameLine();
    drawButton("Reset Scale", [&]() {
        if (index >= 0 && index < static_cast<int>(sceneState.objects.size())) {
            sceneState.objects[index].scale[0] = 1.0f;
            sceneState.objects[index].scale[1] = 1.0f;
        }
    });

    if (columns > 2) {
        ImGui::SameLine();
    } else {
        ImGui::NewLine();
    }
    drawButton("Reset Rotation", [&]() {
        if (index >= 0 && index < static_cast<int>(sceneState.objects.size())) {
            sceneState.objects[index].rotation = 0.0f;
        }
    });

    if (columns > 3) {
        ImGui::SameLine();
    } else {
        ImGui::NewLine();
    }
    drawButton("Add Object", [&]() {
        GameObject newObject;
        newObject.id = static_cast<int>(sceneState.objects.size());
        newObject.name = "New Object " + std::to_string(newObject.id);
        newObject.position[0] = 0.0f;
        newObject.position[1] = 0.0f;
        newObject.scale[0] = 1.0f;
        newObject.scale[1] = 1.0f;
        newObject.rotation = 0.0f;
        newObject.textureResourceId = 0;
        newObject.texturePath = "pillar.png";
        newObject.scriptResourceId = 0;
        newObject.scriptPath.clear();
        sceneState.objects.push_back(newObject);
        editorState.selectedObjectIndex = static_cast<int>(sceneState.objects.size()) - 1;
    });

    if (columns > 4) {
        ImGui::SameLine();
    } else {
        ImGui::NewLine();
    }
    drawButton("Delete Selected", [&]() {
        if (index >= 0 && index < static_cast<int>(sceneState.objects.size())) {
            sceneState.objects.erase(sceneState.objects.begin() + index);

            if (sceneState.objects.empty()) {
                editorState.selectedObjectIndex = -1;
            }
            else if (index >= static_cast<int>(sceneState.objects.size())) {
                editorState.selectedObjectIndex = static_cast<int>(sceneState.objects.size()) - 1;
            }
        }
    });
}

}

void DrawScenePanel(SceneState& sceneState, EditorState& editorState, SDL_Texture* sceneTexture)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

    ImGui::Begin("Scene");

    int index = editorState.selectedObjectIndex;
    ImGui::TextUnformatted("Viewport");

    ImVec2 availableRegion = ImGui::GetContentRegionAvail();
    float viewportHeight = std::clamp(availableRegion.y * 0.34f, 140.0f, 420.0f);
    ImVec2 viewportSize(availableRegion.x, viewportHeight);

    editorState.sceneViewportWidth = viewportSize.x;
    editorState.sceneViewportHeight = viewportSize.y;

    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 255));
    ImGui::BeginChild("SceneViewport", viewportSize, true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    if (sceneTexture) {
        float textureWidth = 1.0f;
        float textureHeight = 1.0f;
        if (!SDL_GetTextureSize(sceneTexture, &textureWidth, &textureHeight)) {
            textureWidth = viewportSize.x;
            textureHeight = viewportSize.y;
        }

        float scale = std::min(viewportSize.x / textureWidth, viewportSize.y / textureHeight);
        scale = (scale > 0.0f) ? scale : 1.0f;

        ImVec2 imageSize(textureWidth * scale, textureHeight * scale);
        ImVec2 cursor = ImGui::GetCursorPos();
        float offsetX = (viewportSize.x - imageSize.x) * 0.5f;
        float offsetY = (viewportSize.y - imageSize.y) * 0.5f;
        ImGui::SetCursorPos(ImVec2(cursor.x + std::max(0.0f, offsetX), cursor.y + std::max(0.0f, offsetY)));
        ImGui::Image((ImTextureID)(intptr_t)sceneTexture, imageSize);
    }
    else {
        ImVec2 childPos = ImGui::GetCursorScreenPos();
        ImGui::GetWindowDrawList()->AddText(
            ImVec2(childPos.x + 12.0f, childPos.y + 12.0f),
            IM_COL32(220, 220, 220, 255),
            "Scene viewport is initializing..."
        );
    }

    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::Separator();
    DrawActionButtonRow(sceneState, editorState, index);
    ImGui::Spacing();
    const bool hasProject = !editorState.sceneFilePath.empty();

    if (!hasProject) {
        ImGui::BeginDisabled();
    }

    if (ImGui::Button("Save Scene")) {
        std::string sceneName = "UntitledScene";
        if (index >= 0 && index < static_cast<int>(sceneState.objects.size())) {
            sceneName = sceneState.objects[index].name;
        } else if (!editorState.projectName.empty()) {
            sceneName = editorState.projectName;
        }

        if (SaveSceneToFile(sceneState, sceneName, editorState.sceneFilePath)) {
            sceneStatus = "Scene saved successfully";
        }
        else {
            sceneStatus = "Failed to save scene";
        }
    }

    if (!hasProject) {
        ImGui::EndDisabled();
        ImGui::TextWrapped("Create or open a project before saving the scene.");
    }
    else {
        ImGui::TextWrapped("Scene File: %s", editorState.sceneFilePath.c_str());
    }
    ImGui::TextWrapped("Status: %s", sceneStatus.c_str());

    ImGui::End();

    ImGui::PopStyleVar(2);
}
