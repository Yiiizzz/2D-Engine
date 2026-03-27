#include "Engine.h"
#include <iostream>
#include "editor/EditorUI.h"
#include <SDL3/SDL.h>
#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>
Engine::Engine() : running(false) {}

extern void SetupEditorStyle();

bool Engine::init() {
    if (!windowManager.init("SDL3 Window Control Demo", 800, 600)) {
        return false;
    }

    if (!renderer2D.init(windowManager.getWindow())) {
        return false;
    }

    if (!resourceManager.loadTexture("test.png", renderer2D.getRenderer())) {
        return false;
    }

    // ✅ 初始化 ImGui
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    SetupEditorStyle();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui_ImplSDL3_InitForSDLRenderer(windowManager.getWindow(), renderer2D.getRenderer());
    ImGui_ImplSDLRenderer3_Init(renderer2D.getRenderer());

    // 初始化场景数据
    sceneState.objects.push_back({ 0, "Player", {100.0f, 100.0f}, {1.0f, 1.0f} });
    sceneState.objects.push_back({ 1, "Enemy", {300.0f, 200.0f}, {1.0f, 1.0f} });
    editorState.selectedObjectIndex = 0;

    running = true;
    return true;
}

void Engine::run() {
    SDL_Event event;
    while (running) {
        // 1. 输入
        inputManager.processEvents(windowManager);
        if (inputManager.shouldQuit()) {
            running = false;
        }

        // 2. 逻辑
        gameLoop.update(sceneState, editorState);

        // 3. 开始 ImGui 帧
        ImGui_ImplSDL3_NewFrame();
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui::NewFrame();

        // 4. 编辑器 UI
        DrawEditorUI(sceneState, editorState);

        // 5. 渲染场景
        renderer2D.clear();
        renderer2D.renderScene(sceneState, resourceManager);

        // 6. 渲染 ImGui
        ImGui::Render();
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer2D.getRenderer());

        // 7. 提交画面
        renderer2D.present();
    }
}

void Engine::shutdown() {
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    resourceManager.destroy();
    renderer2D.destroy();
    windowManager.destroy();
}