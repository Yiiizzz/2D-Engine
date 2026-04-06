#include "ScriptRuntime.h"

#include "../resource/ResourcePathUtils.h"

#include <cctype>
#include <fstream>
#include <sstream>
#include <system_error>

#ifdef _WIN32
#include <windows.h>
#include <cstdio>
#endif

namespace fs = std::filesystem;

namespace {

std::string normalizePath(const std::string& path) {
    return ResourcePathUtils::NormalizePathString(ResourcePathUtils::Utf8ToPath(path));
}

std::string sanitizeStem(const fs::path& path) {
    std::string stem = path.stem().string();
    for (char& ch : stem) {
        if (!(std::isalnum(static_cast<unsigned char>(ch)) || ch == '_')) {
            ch = '_';
        }
    }
    return stem.empty() ? "Script" : stem;
}

bool ensureDirectory(const fs::path& path) {
    std::error_code ec;
    fs::create_directories(path, ec);
    return !ec;
}

std::string makeInstanceKey(const GameObject& object, const std::string& scriptPath) {
    return std::to_string(object.id) + "|" + scriptPath;
}

bool writeScriptApiHeader(const fs::path& cacheDirectory) {
    const fs::path headerPath = cacheDirectory / "ScriptAPI.h";
    if (fs::exists(headerPath)) {
        return true;
    }

    std::ofstream file(headerPath);
    if (!file.is_open()) {
        return false;
    }

    file <<
        "#pragma once\n"
        "#ifdef _WIN32\n"
        "#define SCRIPT_EXPORT extern \"C\" __declspec(dllexport)\n"
        "#else\n"
        "#define SCRIPT_EXPORT extern \"C\"\n"
        "#endif\n\n"
        "struct ScriptContext {\n"
        "    const char* name;\n"
        "    float deltaTime;\n"
        "    float positionX;\n"
        "    float positionY;\n"
        "    float rotation;\n"
        "    float scaleX;\n"
        "    float scaleY;\n"
        "};\n\n"
        "SCRIPT_EXPORT void OnStart(ScriptContext* ctx);\n"
        "SCRIPT_EXPORT void OnUpdate(ScriptContext* ctx);\n";
    return true;
}

#ifdef _WIN32
std::string runCommand(const std::string& command) {
    std::string output;
    FILE* pipe = _popen(command.c_str(), "r");
    if (!pipe) {
        return "Failed to start compiler process.";
    }

    char buffer[512];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        output += buffer;
    }
    const int exitCode = _pclose(pipe);
    if (exitCode != 0 && output.empty()) {
        output = "Compiler exited with code " + std::to_string(exitCode);
    }
    return output;
}
#endif

}  // namespace

ScriptRuntime::ScriptRuntime() = default;

ScriptRuntime::~ScriptRuntime() {
    unloadAll();
}

void ScriptRuntime::reset() {
    startedInstances_.clear();
    unloadAll();
}

void ScriptRuntime::applyContext(GameObject& object, const ScriptContext& context) {
    object.position[0] = context.positionX;
    object.position[1] = context.positionY;
    object.rotation = context.rotation;
    object.scale[0] = context.scaleX;
    object.scale[1] = context.scaleY;
}

ScriptRuntime::ScriptModule* ScriptRuntime::loadModule(const std::string& scriptPath, const std::string& projectRoot, std::string& outStatus) {
#ifndef _WIN32
    outStatus = "Dynamic native scripts are currently implemented for Windows builds only.";
    return nullptr;
#else
    const std::string normalizedScriptPath = normalizePath(scriptPath);
    const fs::path sourcePath = ResourcePathUtils::Utf8ToPath(normalizedScriptPath);
    std::error_code ec;
    if (!fs::exists(sourcePath, ec)) {
        outStatus = "Script file not found: " + normalizedScriptPath;
        return nullptr;
    }

    const fs::path cacheDirectory = ResourcePathUtils::Utf8ToPath(projectRoot) / "Library" / "ScriptCache";
    if (!ensureDirectory(cacheDirectory) || !writeScriptApiHeader(cacheDirectory)) {
        outStatus = "Failed to prepare script cache directory.";
        return nullptr;
    }

    const auto sourceWriteTime = fs::last_write_time(sourcePath, ec);
    if (ec) {
        outStatus = "Failed to read script timestamp: " + normalizedScriptPath;
        return nullptr;
    }

    auto existing = modules_.find(normalizedScriptPath);
    if (existing != modules_.end() && existing->second.sourceWriteTime == sourceWriteTime) {
        return &existing->second;
    }

    if (existing != modules_.end()) {
        if (existing->second.handle) {
            FreeLibrary(static_cast<HMODULE>(existing->second.handle));
        }
        modules_.erase(existing);
    }

    const std::string stem = sanitizeStem(sourcePath);
    const auto tick = static_cast<long long>(sourceWriteTime.time_since_epoch().count());
    const fs::path dllPath = cacheDirectory / (stem + "_" + std::to_string(tick) + ".dll");
    const fs::path objPath = cacheDirectory / (stem + "_" + std::to_string(tick) + ".obj");

    std::ostringstream command;
    command
        << "cmd /c cl /nologo /std:c++20 /EHsc /LD "
        << "/I\"" << ResourcePathUtils::PathToUtf8String(cacheDirectory) << "\" "
        << "/I\"" << normalizePath(projectRoot) << "\" "
        << "\"" << normalizedScriptPath << "\" "
        << "/Fe:\"" << ResourcePathUtils::PathToUtf8String(dllPath) << "\" "
        << "/Fo:\"" << ResourcePathUtils::PathToUtf8String(objPath) << "\" 2>&1";

    const std::string compilerOutput = runCommand(command.str());
    if (!fs::exists(dllPath)) {
        outStatus = compilerOutput.empty() ? "Failed to compile script: " + normalizedScriptPath : compilerOutput;
        return nullptr;
    }

    HMODULE moduleHandle = LoadLibraryA(ResourcePathUtils::PathToUtf8String(dllPath).c_str());
    if (!moduleHandle) {
        outStatus = "Compiled script but failed to load module: " + ResourcePathUtils::PathToUtf8String(dllPath);
        return nullptr;
    }

    ScriptModule module;
    module.handle = moduleHandle;
    module.onStart = reinterpret_cast<ScriptCallback>(GetProcAddress(moduleHandle, "OnStart"));
    module.onUpdate = reinterpret_cast<ScriptCallback>(GetProcAddress(moduleHandle, "OnUpdate"));
    module.sourceWriteTime = sourceWriteTime;
    module.binaryPath = ResourcePathUtils::PathToUtf8String(dllPath);

    auto [it, inserted] = modules_.emplace(normalizedScriptPath, std::move(module));
    if (!inserted) {
        outStatus = "Failed to cache compiled script module.";
        return nullptr;
    }

    outStatus = compilerOutput;
    return &it->second;
#endif
}

void ScriptRuntime::unloadAll() {
#ifdef _WIN32
    for (auto& [path, module] : modules_) {
        if (module.handle) {
            FreeLibrary(static_cast<HMODULE>(module.handle));
            module.handle = nullptr;
        }
    }
#endif
    modules_.clear();
}

void ScriptRuntime::execute(GameObject& object, const std::string& scriptPath, const std::string& projectRoot, float deltaTime, std::string& outStatus) {
    if (scriptPath.empty() || projectRoot.empty()) {
        return;
    }

    ScriptModule* module = loadModule(scriptPath, projectRoot, outStatus);
    if (!module) {
        return;
    }

    ScriptContext context;
    context.name = object.name.c_str();
    context.deltaTime = deltaTime;
    context.positionX = object.position[0];
    context.positionY = object.position[1];
    context.rotation = object.rotation;
    context.scaleX = object.scale[0];
    context.scaleY = object.scale[1];

    const std::string instanceKey = makeInstanceKey(object, normalizePath(scriptPath));
    if (!startedInstances_.contains(instanceKey)) {
        if (module->onStart) {
            module->onStart(&context);
            applyContext(object, context);
        }
        startedInstances_.insert(instanceKey);
    }

    if (module->onUpdate) {
        context.name = object.name.c_str();
        context.deltaTime = deltaTime;
        context.positionX = object.position[0];
        context.positionY = object.position[1];
        context.rotation = object.rotation;
        context.scaleX = object.scale[0];
        context.scaleY = object.scale[1];
        module->onUpdate(&context);
        applyContext(object, context);
    }
}
