#pragma once

#include "../core/SceneState.h"

#include <filesystem>
#include <string>
#include <unordered_map>
#include <unordered_set>

class ScriptRuntime {
public:
    ScriptRuntime();
    ~ScriptRuntime();

    void reset();
    void execute(GameObject& object, const std::string& scriptPath, const std::string& projectRoot, float deltaTime, std::string& outStatus);

private:
    using ScriptCallback = void(*)(void*);

    struct ScriptContext {
        const char* name = nullptr;
        float deltaTime = 0.0f;
        float positionX = 0.0f;
        float positionY = 0.0f;
        float rotation = 0.0f;
        float scaleX = 1.0f;
        float scaleY = 1.0f;
    };

    struct ScriptModule {
        void* handle = nullptr;
        ScriptCallback onStart = nullptr;
        ScriptCallback onUpdate = nullptr;
        std::filesystem::file_time_type sourceWriteTime{};
        std::string binaryPath;
    };

    ScriptModule* loadModule(const std::string& scriptPath, const std::string& projectRoot, std::string& outStatus);
    void unloadAll();
    static void applyContext(GameObject& object, const ScriptContext& context);

    std::unordered_map<std::string, ScriptModule> modules_;
    std::unordered_set<std::string> startedInstances_;
};
