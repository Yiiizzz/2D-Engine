#include "ConsolePanel.h"
#include "../EditorActions.h"

#include "imgui.h"

void DrawConsolePanel(EditorState& editorState)
{
    ImGui::Begin("Console", &editorState.showConsole);

    if (ImGui::Button("Clear"))
    {
        editorState.logs.clear();
    }

    ImGui::SameLine();
    ImGui::Checkbox("Info", &editorState.consoleShowInfo);
    ImGui::SameLine();
    ImGui::Checkbox("Warnings", &editorState.consoleShowWarnings);
    ImGui::SameLine();
    ImGui::Checkbox("Errors", &editorState.consoleShowErrors);
    ImGui::SameLine();
    ImGui::Checkbox("Auto Scroll", &editorState.consoleAutoScroll);

    ImGui::Separator();
    ImGui::BeginChild("ConsoleLogScroll");

    bool shouldScroll = false;
    if (editorState.logs.empty())
    {
        ImGui::TextDisabled("No logs yet.");
    }
    else
    {
        for (auto it = editorState.logs.begin(); it != editorState.logs.end(); ++it)
        {
            if (it->level == EditorLogLevel::Info && !editorState.consoleShowInfo)
                continue;
            if (it->level == EditorLogLevel::Warning && !editorState.consoleShowWarnings)
                continue;
            if (it->level == EditorLogLevel::Error && !editorState.consoleShowErrors)
                continue;

            ImVec4 color = ImVec4(0.85f, 0.85f, 0.85f, 1.0f);
            if (it->level == EditorLogLevel::Warning)
                color = ImVec4(0.95f, 0.80f, 0.35f, 1.0f);
            else if (it->level == EditorLogLevel::Error)
                color = ImVec4(0.95f, 0.45f, 0.45f, 1.0f);

            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextWrapped("[%s] %s", GetEditorLogLevelLabel(it->level), it->message.c_str());
            ImGui::PopStyleColor();
            shouldScroll = true;
        }
    }

    if (editorState.consoleAutoScroll && shouldScroll)
    {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndChild();
    ImGui::End();
}
