#include "InputManager.h"
#include "../core/SceneState.h"
#include "../window/WindowManager.h"
#include "../../frontend/src/EditorState.h"
#include <backends/imgui_impl_sdl3.h>
#include <imgui.h>
#include <algorithm>
#include <cmath>
#include <iostream>

namespace {

constexpr float kSceneObjectBaseSize = 64.0f;

bool PointInRect(float x, float y, float left, float top, float width, float height) {
    return x >= left && y >= top && x <= left + width && y <= top + height;
}

bool MapScreenToScenePoint(
    float screenX,
    float screenY,
    const EditorState& editorState,
    float& sceneX,
    float& sceneY,
    bool clampToViewport = false) {
    if (editorState.sceneViewportScreenWidth <= 0.0f || editorState.sceneViewportScreenHeight <= 0.0f ||
        editorState.sceneViewportWidth <= 0.0f || editorState.sceneViewportHeight <= 0.0f) {
        return false;
    }

    float localX = screenX - editorState.sceneViewportScreenX;
    float localY = screenY - editorState.sceneViewportScreenY;
    if (clampToViewport) {
        localX = std::clamp(localX, 0.0f, editorState.sceneViewportScreenWidth);
        localY = std::clamp(localY, 0.0f, editorState.sceneViewportScreenHeight);
    } else if (!PointInRect(
        screenX,
        screenY,
        editorState.sceneViewportScreenX,
        editorState.sceneViewportScreenY,
        editorState.sceneViewportScreenWidth,
        editorState.sceneViewportScreenHeight)) {
        return false;
    }

    const float zoom = editorState.sceneViewZoom > 0.0f ? editorState.sceneViewZoom : 1.0f;
    sceneX = (localX - editorState.sceneViewOffsetX) / zoom;
    sceneY = (localY - editorState.sceneViewOffsetY) / zoom;
    return true;
}

int FindTopmostObjectAt(const SceneState& sceneState, float sceneX, float sceneY) {
    for (int index = static_cast<int>(sceneState.objects.size()) - 1; index >= 0; --index) {
        const GameObject& object = sceneState.objects[index];
        const float width = kSceneObjectBaseSize * std::max(object.scale[0], 0.0f);
        const float height = kSceneObjectBaseSize * std::max(object.scale[1], 0.0f);
        if (PointInRect(sceneX, sceneY, object.position[0], object.position[1], width, height)) {
            return index;
        }
    }

    return -1;
}

void StopSceneDrag(EditorState& editorState) {
    editorState.isDraggingSceneObject = false;
    editorState.draggingObjectIndex = -1;
    editorState.sceneDragOffsetX = 0.0f;
    editorState.sceneDragOffsetY = 0.0f;
}

bool IsInsideSceneViewport(float screenX, float screenY, const EditorState& editorState) {
    return PointInRect(
        screenX,
        screenY,
        editorState.sceneViewportScreenX,
        editorState.sceneViewportScreenY,
        editorState.sceneViewportScreenWidth,
        editorState.sceneViewportScreenHeight);
}

void RefreshSceneViewOffset(EditorState& editorState) {
    const float viewportWidth = std::max(editorState.sceneViewportWidth, 1.0f);
    const float viewportHeight = std::max(editorState.sceneViewportHeight, 1.0f);
    editorState.sceneViewOffsetX = viewportWidth * 0.5f - editorState.sceneViewCenterX * editorState.sceneViewZoom;
    editorState.sceneViewOffsetY = viewportHeight * 0.5f - editorState.sceneViewCenterY * editorState.sceneViewZoom;
}

}

InputManager::InputManager() : quitRequested(false) {}

void InputManager::processEvents(WindowManager& windowManager, SceneState& sceneState, EditorState& editorState) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL3_ProcessEvent(&event);

        switch (event.type) {
        case SDL_EVENT_QUIT:
            quitRequested = true;
            break;

        case SDL_EVENT_KEY_DOWN:
            if (event.key.key == SDLK_ESCAPE) {
                quitRequested = true;
            }
            else if (event.key.key == SDLK_F) {
                windowManager.toggleFullscreen();
                std::cout << "Toggled fullscreen mode" << std::endl;
            }
            else if (event.key.key == SDLK_1) {
                windowManager.resize(800, 600);
                std::cout << "Window resized to 800x600" << std::endl;
            }
            else if (event.key.key == SDLK_2) {
                windowManager.resize(1024, 768);
                std::cout << "Window resized to 1024x768" << std::endl;
            }
            else if (event.key.key == SDLK_3) {
                windowManager.resize(1280, 720);
                std::cout << "Window resized to 1280x720" << std::endl;
            }
            break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            if (event.button.button == SDL_BUTTON_LEFT) {
                float sceneX = 0.0f;
                float sceneY = 0.0f;
                const bool insideSceneViewport = MapScreenToScenePoint(
                    event.button.x,
                    event.button.y,
                    editorState,
                    sceneX,
                    sceneY);

                if (insideSceneViewport) {
                    const int hitObjectIndex = FindTopmostObjectAt(sceneState, sceneX, sceneY);
                    editorState.selectedObjectIndex = hitObjectIndex;

                    if (hitObjectIndex >= 0 && hitObjectIndex < static_cast<int>(sceneState.objects.size())) {
                        const GameObject& object = sceneState.objects[hitObjectIndex];
                        editorState.isDraggingSceneObject = true;
                        editorState.draggingObjectIndex = hitObjectIndex;
                        editorState.sceneDragOffsetX = sceneX - object.position[0];
                        editorState.sceneDragOffsetY = sceneY - object.position[1];
                    } else {
                        StopSceneDrag(editorState);
                    }
                } else if (!ImGui::GetIO().WantCaptureMouse) {
                    editorState.selectedObjectIndex = -1;
                    StopSceneDrag(editorState);
                }
            } else if (event.button.button == SDL_BUTTON_MIDDLE) {
                if (IsInsideSceneViewport(event.button.x, event.button.y, editorState)) {
                    editorState.isPanningSceneView = true;
                    editorState.sceneViewPanLastScreenX = event.button.x;
                    editorState.sceneViewPanLastScreenY = event.button.y;
                }
            }
            break;

        case SDL_EVENT_MOUSE_MOTION:
            if (editorState.isPanningSceneView) {
                const float deltaX = event.motion.x - editorState.sceneViewPanLastScreenX;
                const float deltaY = event.motion.y - editorState.sceneViewPanLastScreenY;
                const float zoom = editorState.sceneViewZoom > 0.0f ? editorState.sceneViewZoom : 1.0f;
                editorState.sceneViewCenterX -= deltaX / zoom;
                editorState.sceneViewCenterY -= deltaY / zoom;
                RefreshSceneViewOffset(editorState);
                editorState.sceneViewPanLastScreenX = event.motion.x;
                editorState.sceneViewPanLastScreenY = event.motion.y;
            } else if (editorState.isDraggingSceneObject &&
                editorState.draggingObjectIndex >= 0 &&
                editorState.draggingObjectIndex < static_cast<int>(sceneState.objects.size())) {
                float sceneX = 0.0f;
                float sceneY = 0.0f;
                if (MapScreenToScenePoint(event.motion.x, event.motion.y, editorState, sceneX, sceneY, true)) {
                    GameObject& object = sceneState.objects[editorState.draggingObjectIndex];
                    object.position[0] = sceneX - editorState.sceneDragOffsetX;
                    object.position[1] = sceneY - editorState.sceneDragOffsetY;
                }
            }
            break;

        case SDL_EVENT_MOUSE_BUTTON_UP:
            if (event.button.button == SDL_BUTTON_LEFT) {
                StopSceneDrag(editorState);
            } else if (event.button.button == SDL_BUTTON_MIDDLE) {
                editorState.isPanningSceneView = false;
            }
            break;

        case SDL_EVENT_MOUSE_WHEEL: {
            const ImVec2 mousePos = ImGui::GetIO().MousePos;
            if (!IsInsideSceneViewport(mousePos.x, mousePos.y, editorState) || event.wheel.y == 0.0f) {
                break;
            }

            float worldX = 0.0f;
            float worldY = 0.0f;
            if (!MapScreenToScenePoint(mousePos.x, mousePos.y, editorState, worldX, worldY)) {
                break;
            }

            const float oldZoom = editorState.sceneViewZoom > 0.0f ? editorState.sceneViewZoom : 1.0f;
            const float zoomFactor = std::pow(1.1f, event.wheel.y);
            editorState.sceneViewZoom = std::clamp(oldZoom * zoomFactor, 0.2f, 8.0f);

            const float localX = mousePos.x - editorState.sceneViewportScreenX;
            const float localY = mousePos.y - editorState.sceneViewportScreenY;
            const float viewportCenterX = editorState.sceneViewportScreenWidth * 0.5f;
            const float viewportCenterY = editorState.sceneViewportScreenHeight * 0.5f;
            editorState.sceneViewCenterX = worldX - (localX - viewportCenterX) / editorState.sceneViewZoom;
            editorState.sceneViewCenterY = worldY - (localY - viewportCenterY) / editorState.sceneViewZoom;
            RefreshSceneViewOffset(editorState);
            break;
        }
        }
    }

    if (editorState.draggingObjectIndex >= static_cast<int>(sceneState.objects.size())) {
        StopSceneDrag(editorState);
    }

    if (editorState.draggingObjectIndex < 0) {
        editorState.isDraggingSceneObject = false;
    }
}

bool InputManager::shouldQuit() const {
    return quitRequested;
}
