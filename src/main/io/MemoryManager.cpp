#include "MemoryManager.h"
#include "logger/logger.h"
#include "graphics/Graphics.h"
#include "graphics/Image.h"
#include "graphics/model/Model.h"
#include "graphics/video/VideoFile.h"
#include "graphics/Shader.h"
#include "graphics/ShaderProgram.h"
#include "graphics/Fbo.h"
#include "graphics/Font.h"
#include "audio/AudioFile.h"
#include "script/Script.h"
#include "io/File.h"

#include <type_traits>

template <typename T>
std::function<T*(std::string)> MemoryManager<T>::newInstance;

template <typename T>
MemoryManager<T>::MemoryManager() {
    name = NULL;
}

template <typename T>
MemoryManager<T>::~MemoryManager() {
    clear();
}

template <typename T>
void MemoryManager<T>::clear() {
    if (resources.empty()) {
        return;
    }

    size_t size = resources.size();
    loggerDebug("Cleaning %d %s resource(s)", size, name);

    for (auto it : resources) {
        if (it.second != NULL) {
            delete it.second;
        } else {
            loggerWarning("Expected non-NULL pointer when removing resource. manager:'%s', resource:'%s'", name, it.first.c_str());
        }
    }

    if (Graphics::getInstance().handleErrors()) {
        loggerError("Could not cleanly clean %d %s resource(s).", size, name);
    }

    resources.clear();
}

template <typename T>
const std::map<std::string, T*>& MemoryManager<T>::getResources() {
    return resources;
}

template <typename T>
bool MemoryManager<T>::removeResource(const std::string& name) {
    auto it = resources.find(File::getProjectPath() + name);
    if (it == resources.end()) {
        it = resources.find(name);
    }

    if (it == resources.end()) {
        return false;
    }

    if (it->second != NULL) {
        delete it->second;
    } else {
        loggerWarning("Expected non-NULL pointer when removing resource. manager:'%s', resource:'%s'", getName(), name.c_str());
    }

    resources.erase(it);

    return true;
}


template <typename T>
T* MemoryManager<T>::getGenericResource(const std::string& name, bool required) {
    //FIXME: Project path determination needs to be fixed for files
    auto it = resources.find(File::getProjectPath() + name);
    if (it == resources.end()) {
        it = resources.find(name);
    }

    if (it != resources.end()) {
        if (it->second != NULL) {
            return dynamic_cast<T*>(it->second);
        } else {
            loggerWarning("Memory manager has NULL generic resource: '%s'. Attempting to override.", name.c_str());
        }
    }

    if (!newInstance) {
        loggerFatal("newInstance not defined in the MemoryManager. name:'%s'", name.c_str());
        return NULL;
    }

    auto *generic = newInstance(name);
    if (generic == NULL) {
        if (required) {
            loggerFatal("Resource not found: '%s'", name.c_str());
        }

        return NULL;
    }

    resources[generic->getName()] = generic;
    return dynamic_cast<T*>(generic);
}

template <typename T>
T* MemoryManager<T>::getFileResource(const std::string& name, bool required) {
    auto *resource = getGenericResource(name, required);

    if (resource) {
        File *file = dynamic_cast<File*>(resource);
        if (!file->exists()) {
            removeResource(name);

            if (required) {
                loggerFatal("Resource does not exist: '%s'", name.c_str());
            }

            return NULL;
        }
    } else if (required) {
        loggerFatal("Resource not found: '%s'", name.c_str());
    }

    return resource;
}

#define CREATE_MEMORY_MANAGER_TEMPLATE_METHODS(type, resourceType) \
template <> \
MemoryManager<type>& MemoryManager<type>::getInstance() { \
    static MemoryManager<type> memoryManager; \
    memoryManager.name = #type; \
    newInstance = &type::newInstance; \
 \
    return memoryManager; \
} \
template <> \
type* MemoryManager<type>::getResource(const std::string& name, bool required) { \
    return get##resourceType##Resource(name, required); \
} \
template class MemoryManager<type>;

CREATE_MEMORY_MANAGER_TEMPLATE_METHODS(Image, File)

CREATE_MEMORY_MANAGER_TEMPLATE_METHODS(AudioFile, File)

CREATE_MEMORY_MANAGER_TEMPLATE_METHODS(VideoFile, File)

CREATE_MEMORY_MANAGER_TEMPLATE_METHODS(Model, File)

CREATE_MEMORY_MANAGER_TEMPLATE_METHODS(Shader, File)

CREATE_MEMORY_MANAGER_TEMPLATE_METHODS(ShaderProgram, Generic)

CREATE_MEMORY_MANAGER_TEMPLATE_METHODS(Fbo, Generic)

CREATE_MEMORY_MANAGER_TEMPLATE_METHODS(Font, File)

CREATE_MEMORY_MANAGER_TEMPLATE_METHODS(Script, File)
