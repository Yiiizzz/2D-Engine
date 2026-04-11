#include "ResourceManager.h"

#include "ResourcePathUtils.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <glad/glad.h>

#include <algorithm>
#include <filesystem>
#include <utility>

namespace fs = std::filesystem;

ResourceManager::ResourceManager() {
    addSearchPath(".");
    addSearchPath("asset");
    addSearchPath("asset/image");
}

ResourceManager::~ResourceManager() {
    releaseAllTextures();
}

void ResourceManager::addSearchPath(const std::string& path) {
    if (path.empty()) {
        return;
    }

    const std::string normalized = ResourcePathUtils::NormalizePathString(path);
    if (std::find(searchPaths_.begin(), searchPaths_.end(), normalized) != searchPaths_.end()) {
        return;
    }

    searchPaths_.push_back(normalized);
    clearPathCache();
}

void ResourceManager::setSearchPaths(const std::vector<std::string>& paths) {
    searchPaths_.clear();
    for (const std::string& path : paths) {
        addSearchPath(path);
    }
    clearPathCache();
}

void ResourceManager::clearSearchPaths() {
    searchPaths_.clear();
    clearPathCache();
}

const std::vector<std::string>& ResourceManager::getSearchPaths() const {
    return searchPaths_;
}

const TextureResourceInfo* ResourceManager::loadTexture(const std::string& identifier) {
    return loadTextureInternal(identifier);
}

const TextureResourceInfo* ResourceManager::getTexture(const std::string& identifier) {
    return loadTextureInternal(identifier);
}

const TextureResourceInfo* ResourceManager::findTextureInfo(const std::string& identifier) const {
    const std::string resolvedPath = lookupResolvedPath(identifier);
    if (resolvedPath.empty()) {
        return nullptr;
    }

    const auto it = textures_.find(resolvedPath);
    if (it == textures_.end()) {
        return nullptr;
    }

    tempTextureInfo_.rendererId = it->second.rendererId;
    tempTextureInfo_.resolvedPath = it->second.resolvedPath;
    tempTextureInfo_.width = it->second.width;
    tempTextureInfo_.height = it->second.height;
    return &tempTextureInfo_;
}

bool ResourceManager::hasTexture(const std::string& identifier) const {
    return findTextureInfo(identifier) != nullptr;
}

std::string ResourceManager::resolveTexturePath(const std::string& identifier) const {
    if (identifier.empty()) {
        return {};
    }

    const auto cacheIt = resolvedPathCache_.find(identifier);
    if (cacheIt != resolvedPathCache_.end()) {
        return cacheIt->second;
    }

    const std::string resolved = ResourcePathUtils::ResolvePath(identifier, searchPaths_);
    resolvedPathCache_[identifier] = resolved;
    return resolved;
}

std::vector<std::string> ResourceManager::collectTextureCandidates(const std::string& identifier) const {
    std::vector<std::string> candidates;
    if (identifier.empty()) {
        return candidates;
    }

    candidates.push_back(identifier);

    const std::vector<fs::path> roots = ResourcePathUtils::BuildSearchRoots(searchPaths_);
    for (const fs::path& root : roots) {
        const std::string candidate = ResourcePathUtils::NormalizePathString(root / identifier);
        if (std::find(candidates.begin(), candidates.end(), candidate) == candidates.end()) {
            candidates.push_back(candidate);
        }
    }

    return candidates;
}

bool ResourceManager::releaseTexture(const std::string& identifier) {
    const std::string resolvedPath = lookupResolvedPath(identifier);
    if (resolvedPath.empty()) {
        return false;
    }

    destroyRecord(resolvedPath);
    return true;
}

void ResourceManager::releaseAllTextures() {
    for (auto& [_, record] : textures_) {
        if (record.rendererId != 0) {
            glDeleteTextures(1, &record.rendererId);
            record.rendererId = 0;
        }
    }

    textures_.clear();
    aliasToResolvedPath_.clear();
}

void ResourceManager::destroy() {
    releaseAllTextures();
}

std::size_t ResourceManager::getTextureCount() const {
    return textures_.size();
}

const std::string& ResourceManager::getLastError() const {
    return lastError_;
}

const TextureResourceInfo* ResourceManager::loadTextureInternal(const std::string& identifier) {
    if (identifier.empty()) {
        setLastError("Texture identifier is empty.");
        return nullptr;
    }

    if (const TextureResourceInfo* cached = findTextureInfo(identifier); cached != nullptr) {
        lastError_.clear();
        return cached;
    }

    const std::string resolvedPath = resolveTexturePath(identifier);
    if (resolvedPath.empty()) {
        setLastError(std::string("Unable to resolve texture path: ") + identifier);
        return nullptr;
    }

    SDL_Surface* surface = IMG_Load(resolvedPath.c_str());
    if (surface == nullptr) {
        setLastError(std::string("IMG_Load failed for '") + resolvedPath + "': " + SDL_GetError());
        return nullptr;
    }

    SDL_Surface* rgbaSurface = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
    SDL_DestroySurface(surface);
    if (rgbaSurface == nullptr) {
        setLastError(std::string("SDL_ConvertSurface failed for '") + resolvedPath + "': " + SDL_GetError());
        return nullptr;
    }

    const int width = rgbaSurface->w;
    const int height = rgbaSurface->h;

    unsigned int texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgbaSurface->pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
    SDL_DestroySurface(rgbaSurface);

    TextureRecord record;
    record.rendererId = texture;
    record.resolvedPath = resolvedPath;
    record.width = width;
    record.height = height;

    textures_[resolvedPath] = std::move(record);
    registerAlias(identifier, resolvedPath);
    registerAlias(resolvedPath, resolvedPath);
    lastError_.clear();
    return findTextureInfo(identifier);
}

void ResourceManager::registerAlias(const std::string& alias, const std::string& resolvedPath) {
    aliasToResolvedPath_[alias] = resolvedPath;

    auto it = textures_.find(resolvedPath);
    if (it != textures_.end()) {
        it->second.aliases.insert(alias);
    }
}

std::string ResourceManager::lookupResolvedPath(const std::string& identifier) const {
    const auto aliasIt = aliasToResolvedPath_.find(identifier);
    if (aliasIt != aliasToResolvedPath_.end()) {
        return aliasIt->second;
    }

    const std::string resolved = resolveTexturePath(identifier);
    if (resolved.empty()) {
        return {};
    }

    return textures_.contains(resolved) ? resolved : std::string{};
}

void ResourceManager::destroyRecord(const std::string& resolvedPath) {
    const auto it = textures_.find(resolvedPath);
    if (it == textures_.end()) {
        return;
    }

    for (const std::string& alias : it->second.aliases) {
        aliasToResolvedPath_.erase(alias);
    }

    if (it->second.rendererId != 0) {
        glDeleteTextures(1, &it->second.rendererId);
    }

    textures_.erase(it);
}

void ResourceManager::setLastError(std::string message) {
    lastError_ = std::move(message);
}

void ResourceManager::clearPathCache() {
    resolvedPathCache_.clear();
}
