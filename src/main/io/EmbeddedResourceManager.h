#ifndef ENGINE_IO_EMBEDDEDRESOURCEMANAGER_H_
#define ENGINE_IO_EMBEDDEDRESOURCEMANAGER_H_

#include <string>
#include <map>

class EmbeddedResource;

class EmbeddedResourceManager {
public:
    static EmbeddedResourceManager& getInstance();

    ~EmbeddedResourceManager();
    void createEmbeddedResources();
    void addResource(std::string filePath, const unsigned char *data, size_t dataSize);
    EmbeddedResource* getResource(std::string filePath, bool required = false);
private:
    EmbeddedResourceManager();
    std::map<std::string, EmbeddedResource*> resources;
};

#endif /*ENGINE_IO_EMBEDDEDRESOURCEMANAGER_H_*/
