#ifndef ENGINE_IO_LIBRARYLOADER_H_
#define ENGINE_IO_LIBRARYLOADER_H_

#include <string>

class LibraryLoader {
public:
    LibraryLoader(std::string libraryName);
    ~LibraryLoader();
    bool load();
    bool free();

    void *getProcAddress(std::string function);

private:
    std::string getLibraryFullName();
    std::string getLibrarySuffix();
    std::string libraryName;
    void *handle;
};

#endif /*ENGINE_IO_LIBRARYLOADER_H_*/
