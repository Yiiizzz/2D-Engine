#pragma once

#include "../core/Ref.h"
#include "GraphicsAPI.h"

class GraphicsContext {
public:
    virtual ~GraphicsContext() = default;

    virtual void Init() = 0;
    virtual void SwapBuffers() = 0;

    static Scope<GraphicsContext> Create(void* windowHandle, GraphicsAPI api);
};
