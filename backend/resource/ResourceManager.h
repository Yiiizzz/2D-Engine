#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct TextureResourceInfo {
    unsigned int rendererId = 0;
    std::string resolvedPath;
    int width = 0;
    int height = 0;
};

class ResourceManager {
public:
    ResourceManager();
    ~ResourceManager();

    void addSearchPath(const std::string& path);
    void setSearchPaths(const std::vector<std::string>& paths);
    void clearSearchPaths();
    const std::vector<std::string>& getSearchPaths() const;

    const TextureResourceInfo* loadTexture(const std::string& identifier);
    const TextureResourceInfo* getTexture(const std::string& identifier);
    const TextureResourceInfo* findTextureInfo(const std::string& identifier) const;
    bool hasTexture(const std::string& identifier) const;

    std::string resolveTexturePath(const std::string& identifier) const;
    std::vector<std::string> collectTextureCandidates(const std::string& identifier) const;

    bool releaseTexture(const std::string& identifier);
    void releaseAllTextures();
    void destroy();

    std::size_t getTextureCount() const;
    const std::string& getLastError() const;

private:
    struct TextureRecord {
        unsigned int rendererId = 0;
        std::string resolvedPath;
        int width = 0;
        int height = 0;
        std::unordered_set<std::string> aliases;
    };

    const TextureResourceInfo* loadTextureInternal(const std::string& identifier);
    void registerAlias(const std::string& alias, const std::string& resolvedPath);
    std::string lookupResolvedPath(const std::string& identifier) const;
    void destroyRecord(const std::string& resolvedPath);
    void setLastError(std::string message);
    void clearPathCache();

    std::vector<std::string> searchPaths_;
    std::unordered_map<std::string, TextureRecord> textures_;
    std::unordered_map<std::string, std::string> aliasToResolvedPath_;
    mutable std::unordered_map<std::string, std::string> resolvedPathCache_;
    mutable TextureResourceInfo tempTextureInfo_;
    std::string lastError_;
};
