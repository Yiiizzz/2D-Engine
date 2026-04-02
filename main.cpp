#include "backend/core/Engine.h"
#include "frontend/src/editor/EditorUI.h"

int main() {
    Engine engine;
    if (!engine.init()) {
        return 1;
    }

    engine.run();
    engine.shutdown();
    return 0;
}