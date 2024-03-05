#ifndef ENGINE_IO_FILEREFRESHMANAGER_H_
#define ENGINE_IO_FILEREFRESHMANAGER_H_

#include <vector>
#include <mutex>

class File;

class FileRefreshManager {
public:
    static FileRefreshManager& getInstance();
    FileRefreshManager();
    bool start();
    void markFileForRefresh(File& file);
    void reloadModified();
    bool isModified();
    void forceReload();
    bool exit();
private:
    void markModified();
    size_t markFilesForRefresh();
    bool reload;
    bool running;
    bool stopping;
    std::vector<File*> reloadFiles;
    std::mutex mutex;
};

#endif /*ENGINE_IO_FILEREFRESHMANAGER_H_*/
