#pragma once

enum class EditorMode {
    Edit,
    Play,
    Pause
};

struct EditorState {
    int selectedObjectIndex = -1;
    EditorMode mode = EditorMode::Edit;
};