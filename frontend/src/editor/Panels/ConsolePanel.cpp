#include"ConsolePanel.h"
#include "imgui.h"

void DrawConsolePanel()
{
    ImGui::Begin("Console");
    ImGui::Text("Log Output:");
    ImGui::Text("[INFO] Engine initialized...");
    ImGui::Text("[DEBUG] Object loaded");
    ImGui::End();
}