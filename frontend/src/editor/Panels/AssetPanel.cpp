#include "AssetPanel.h"
#include "imgui.h"

void DrawAssetPanel()
{
    ImGui::Begin("Project");
    ImGui::Text("Assets:");
    ImGui::BulletText("Texture.png");
    ImGui::BulletText("Sprite.png");
    ImGui::BulletText("Background.jpg");
    ImGui::End();
}