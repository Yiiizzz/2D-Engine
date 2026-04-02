#include "InputManager.h"
#include "../window/WindowManager.h"
#include <iostream>
#include <backends/imgui_impl_sdl3.h>

InputManager::InputManager() : quitRequested(false) {}

void InputManager::processEvents(WindowManager& windowManager) {
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
        }
    }
}

bool InputManager::shouldQuit() const {
    return quitRequested;
}