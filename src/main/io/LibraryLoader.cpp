#include "LibraryLoader.h"

#include "logger/logger.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

LibraryLoader::LibraryLoader(std::string libraryName) {
    this->libraryName = libraryName;
    handle = NULL;

}

LibraryLoader::~LibraryLoader() {
    free();
}

bool LibraryLoader::load() {
    if (handle) {
        free();
    }

#ifdef _WIN32
    handle = static_cast<void*>(LoadLibrary(getLibraryFullName().c_str()));
    if (!handle) {
        // DWORD WINAPI GetLastError(void);
        loggerWarning("Could not load dynamic library! library:0x%p/%s", handle, getLibraryFullName().c_str());

        return false;
    }
#else
    handle = dlopen(getLibraryFullName().c_str(), RTLD_NOW);

    if (!handle) {
        //errstr = dlerror();
        //if (errstr != NULL)
        //    printf ("A dynamic linking error occurred: (%s)\n", errstr);
        loggerWarning("Could not load dynamic library! library:0x%p/%s", handle, getLibraryFullName().c_str());

        return false;
    }
#endif

    loggerDebug("Dynamically loaded library. 0x%p/%s", handle, getLibraryFullName().c_str());

    return true;
}

bool LibraryLoader::free() {
    if (!handle) {
        return true;
    }

#ifdef _WIN32
    if (FreeLibrary(static_cast<HINSTANCE>(handle)) == 0) {
        // DWORD WINAPI GetLastError(void);
        loggerWarning("Could not free dynamic library! library:0x%p/%s", handle, getLibraryFullName().c_str());

        return false;
    }
#else
    if (dlclose(handle) != 0) {
        //errstr = dlerror();
        //if (errstr != NULL)
        //    printf ("A dynamic linking error occurred: (%s)\n", errstr);
        loggerWarning("Could not free dynamic library! library:0x%p/%s", handle, getLibraryFullName().c_str());

        return false;
    }
#endif

    handle = NULL;
    return true;
}

void *LibraryLoader::getProcAddress(std::string function) {
    if (handle == NULL || function.empty()) {
        loggerError("Could not dynamic load library function! library:0x%p/%s, function:'%s'", handle, getLibraryFullName().c_str(), function.c_str());
        return NULL;
    }

    void *functionPointer = NULL;
#ifdef _WIN32
    functionPointer = reinterpret_cast<void*>(GetProcAddress(static_cast<HINSTANCE>(handle), function.c_str()));
    if (functionPointer == NULL) {
        // DWORD WINAPI GetLastError(void);
        loggerError("Could not dynamic load library function! library:0x%p/%s, function:'%s'", handle, getLibraryFullName().c_str(), function.c_str());
    }
#else
    functionPointer = dlsym(handle, function.c_str());
    if (functionPointer == NULL) {
        //errstr = dlerror();
        //if (errstr != NULL)
        //    printf ("A dynamic linking error occurred: (%s)\n", errstr);
        loggerError("Could not dynamic load library function! library:0x%p/%s, function:'%s'", handle, getLibraryFullName().c_str(), function.c_str());
    }
#endif

    return functionPointer;
}

std::string LibraryLoader::getLibraryFullName() {
    return libraryName + getLibrarySuffix();
}

std::string LibraryLoader::getLibrarySuffix() {
#ifdef _WIN32
    return ".dll";
#else
    return ".so";
#endif
}
