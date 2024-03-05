#ifndef ENGINE_IO_MEMORYMANAGER_H_
#define ENGINE_IO_MEMORYMANAGER_H_

#include <string>
#include <map>
#include <functional>

class File;
class ManagedMemoryObject;

template <typename T>
class MemoryManager {
public:
    static MemoryManager<T>& getInstance();
    ~MemoryManager();

    void clear();
    bool removeResource(const std::string& name);

    T* getResource(const std::string& name, bool required = false);
    const std::map<std::string, T*>& getResources();

    const char *getName() {
        return name;
    }

private:
    T* getGenericResource(const std::string& name, bool required);
    T* getFileResource(const std::string& name, bool required);
    MemoryManager();

    static std::function<T*(std::string)> newInstance;
    std::map<std::string, T*> resources;
    const char *name;
};

#endif /*ENGINE_IO_MEMORYMANAGER_H_*/
