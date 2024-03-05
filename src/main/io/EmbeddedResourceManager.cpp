#include "EmbeddedResourceManager.h"
#include "EmbeddedResource.h"
#include "logger/logger.h"

#include "EmbeddedResources.h"

EmbeddedResourceManager& EmbeddedResourceManager::getInstance() {
    static EmbeddedResourceManager embeddedResourceManager;
    return embeddedResourceManager;
}

EmbeddedResourceManager::EmbeddedResourceManager() {
}

EmbeddedResourceManager::~EmbeddedResourceManager() {
    size_t size = resources.size();
    if (size == 0) {
        return;
    }

    loggerDebug("Cleaning %d embedded resource(s)", size);
    for (auto it : resources) {
        delete it.second;
    }

    resources.clear();
}

void EmbeddedResourceManager::addResource(std::string filePath, const unsigned char *data, size_t dataSize) {
    auto it = resources.find(filePath);
    if (it != resources.end()) {
        loggerWarning("Embedded resource '%s' exists. Replacing.", filePath.c_str());
        delete it->second;
    }

    EmbeddedResource *resource = new EmbeddedResource(filePath, data, dataSize);
    resources[filePath] = resource;
}

EmbeddedResource* EmbeddedResourceManager::getResource(std::string filePath, bool required) {
    auto it = resources.find(filePath);
    if (it == resources.end()) {
        if (required) {
            loggerFatal("Embedded resource not found: '%s'", filePath.c_str());
        }
        return NULL;
    }

    return it->second;
}
