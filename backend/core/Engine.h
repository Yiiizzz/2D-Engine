#pragma once
#include "WindowManager.h"
#include "Renderer2D.h"
#include "InputManager.h"
#include "ResourceManager.h"
#include "GameLoop.h"
#include "SceneState.h"
#include "EditorState.h"

class Engine {
private:
    WindowManager windowManager;
    Renderer2D renderer2D;
    InputManager inputManager;
    ResourceManager resourceManager;
    SceneState sceneState;
    EditorState editorState;
	GameLoop gameLoop;
    bool running;

public:
    Engine();
    bool init();
    void run();
    void shutdown();
};