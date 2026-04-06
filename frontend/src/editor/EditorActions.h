#pragma once

#include "../EditorState.h"
#include "../../../backend/core/SceneState.h"

#include <cctype>
#include <filesystem>

inline const char* GetEditorLogLevelLabel(EditorLogLevel level)
{
    switch (level)
    {
    case EditorLogLevel::Info:
        return "INFO";
    case EditorLogLevel::Warning:
        return "WARN";
    case EditorLogLevel::Error:
        return "ERROR";
    default:
        return "LOG";
    }
}

inline void AddEditorLog(EditorState& editorState, EditorLogLevel level, const std::string& message)
{
    editorState.logs.push_back({ level, message });
}

inline bool ContainsInsensitive(const std::string& value, const std::string& search)
{
    if (search.empty())
        return true;

    auto lower = [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); };
    std::string lowerValue;
    std::string lowerSearch;
    lowerValue.reserve(value.size());
    lowerSearch.reserve(search.size());

    for (char ch : value)
        lowerValue.push_back(lower(static_cast<unsigned char>(ch)));
    for (char ch : search)
        lowerSearch.push_back(lower(static_cast<unsigned char>(ch)));

    return lowerValue.find(lowerSearch) != std::string::npos;
}

inline bool HasSelectedObject(const SceneState& sceneState, const EditorState& editorState)
{
    return editorState.selectedObjectIndex >= 0
        && editorState.selectedObjectIndex < static_cast<int>(sceneState.objects.size());
}

inline GameObject* GetSelectedObject(SceneState& sceneState, EditorState& editorState)
{
    if (!HasSelectedObject(sceneState, editorState))
        return nullptr;
    return &sceneState.objects[editorState.selectedObjectIndex];
}

inline int GetNextObjectId(const SceneState& sceneState)
{
    int nextId = 0;
    for (const auto& object : sceneState.objects)
    {
        if (object.id + 1 > nextId)
            nextId = object.id + 1;
    }
    return nextId;
}

inline std::string MakeUniqueObjectName(const SceneState& sceneState, const std::string& baseName)
{
    std::string candidate = baseName.empty() ? "GameObject" : baseName;
    int suffix = 1;

    auto nameExists = [&](const std::string& name) {
        for (const auto& object : sceneState.objects)
        {
            if (object.name == name)
                return true;
        }
        return false;
    };

    while (nameExists(candidate))
    {
        candidate = (baseName.empty() ? "GameObject" : baseName) + " " + std::to_string(suffix++);
    }

    return candidate;
}

inline void FocusAssetPath(EditorState& editorState, const std::string& assetPath)
{
    editorState.focusAssetPath = assetPath;
    if (!assetPath.empty()) {
        editorState.showProject = true;
    }
}

inline bool CreateEmptyObject(SceneState& sceneState, EditorState& editorState, const std::string& baseName, const std::string& source)
{
    GameObject object;
    object.id = GetNextObjectId(sceneState);
    object.name = MakeUniqueObjectName(sceneState, baseName);
    object.position[0] = 0.0f;
    object.position[1] = 0.0f;
    object.scale[0] = 1.0f;
    object.scale[1] = 1.0f;
    object.rotation = 0.0f;
    object.textureResourceId = 0;
    object.texturePath.clear();
    object.scriptResourceId = 0;
    object.scriptPath.clear();

    sceneState.objects.push_back(object);
    editorState.selectedObjectIndex = static_cast<int>(sceneState.objects.size()) - 1;
    AddEditorLog(editorState, EditorLogLevel::Info, "Created object '" + object.name + "' (" + source + ")");
    return true;
}

inline bool DuplicateSelectedObject(SceneState& sceneState, EditorState& editorState, const std::string& source)
{
    GameObject* selected = GetSelectedObject(sceneState, editorState);
    if (!selected)
    {
        AddEditorLog(editorState, EditorLogLevel::Warning, "Duplicate skipped: no object selected.");
        return false;
    }

    GameObject copy = *selected;
    copy.id = GetNextObjectId(sceneState);
    copy.name = MakeUniqueObjectName(sceneState, selected->name + " Copy");
    copy.position[0] += 20.0f;
    copy.position[1] += 20.0f;

    sceneState.objects.push_back(copy);
    editorState.selectedObjectIndex = static_cast<int>(sceneState.objects.size()) - 1;
    AddEditorLog(editorState, EditorLogLevel::Info, "Duplicated object '" + copy.name + "' (" + source + ")");
    return true;
}

inline void ResetObjectPosition(GameObject& object)
{
    object.position[0] = 0.0f;
    object.position[1] = 0.0f;
}

inline void ResetObjectScale(GameObject& object)
{
    object.scale[0] = 1.0f;
    object.scale[1] = 1.0f;
}

inline void ResetObjectRotation(GameObject& object)
{
    object.rotation = 0.0f;
}

inline bool CreateObjectFromAsset(SceneState& sceneState, EditorState& editorState, const AssetRecord& asset, float x, float y, const std::string& source)
{
    if (asset.type != AssetType::Texture) {
        AddEditorLog(editorState, EditorLogLevel::Warning, "Only texture assets can create sprite objects.");
        return false;
    }

    std::string baseName = std::filesystem::path(asset.name).stem().string();
    if (baseName.empty()) {
        baseName = std::filesystem::path(asset.sourcePath).stem().string();
    }

    GameObject object;
    object.id = GetNextObjectId(sceneState);
    object.name = MakeUniqueObjectName(sceneState, baseName.empty() ? "Sprite" : baseName);
    object.position[0] = x;
    object.position[1] = y;
    object.scale[0] = 1.0f;
    object.scale[1] = 1.0f;
    object.rotation = 0.0f;
    object.textureResourceId = asset.id;
    object.texturePath = !asset.relativePath.empty() ? asset.relativePath : asset.sourcePath;
    object.scriptResourceId = 0;
    object.scriptPath.clear();

    sceneState.objects.push_back(object);
    editorState.selectedObjectIndex = static_cast<int>(sceneState.objects.size()) - 1;
    AddEditorLog(editorState, EditorLogLevel::Info, "Created object '" + object.name + "' from asset '" + asset.name + "' (" + source + ")");
    return true;
}

inline bool CreateObjectFromAsset(SceneState& sceneState, EditorState& editorState, const std::string& assetPath, float x, float y, const std::string& source)
{
    const AssetRecord* asset = editorState.assetRegistry.findByPath(assetPath);
    if (!asset)
    {
        AddEditorLog(editorState, EditorLogLevel::Error, "Cannot create object. Asset not found: " + assetPath);
        return false;
    }

    return CreateObjectFromAsset(sceneState, editorState, *asset, x, y, source);
}

inline bool DeleteSelectedObject(SceneState& sceneState, EditorState& editorState)
{
    if (!HasSelectedObject(sceneState, editorState))
    {
        AddEditorLog(editorState, EditorLogLevel::Warning, "Delete skipped: no object selected.");
        return false;
    }

    const std::string deletedName = sceneState.objects[editorState.selectedObjectIndex].name;
    sceneState.objects.erase(sceneState.objects.begin() + editorState.selectedObjectIndex);
    editorState.isDraggingSceneObject = false;
    editorState.draggingObjectIndex = -1;
    editorState.sceneDragOffsetX = 0.0f;
    editorState.sceneDragOffsetY = 0.0f;

    if (sceneState.objects.empty())
        editorState.selectedObjectIndex = -1;
    else if (editorState.selectedObjectIndex >= static_cast<int>(sceneState.objects.size()))
        editorState.selectedObjectIndex = static_cast<int>(sceneState.objects.size()) - 1;

    AddEditorLog(editorState, EditorLogLevel::Info, "Deleted object: " + deletedName);
    return true;
}
