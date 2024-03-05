#ifndef ENGINE_IO_FILE_H_
#define ENGINE_IO_FILE_H_

#include "time/Date.h"
#include "io/ManagedMemoryObject.h"

#include <string>
#include <vector>
#include <ctype.h>

class EmbeddedResource;

enum FileScope {
    CONSTANT,
    VARIABLE
};

class File : public ManagedMemoryObject {
public:
    explicit File(std::string filePath);
    virtual ~File();
    const std::string& getFilePath();
    virtual const std::string& getName();
    std::string getFileExtension();
    Date lastModified();
    virtual bool isSupported();
    bool exists();
    void setModifyGracePeriod(int modifyGracePeriod);
    bool modified();
    bool isFile();
    bool isDirectory();
    virtual bool isLoaded();
    size_t length();
    virtual bool load(bool rollback = false);
    unsigned char *getData();
    void setFileScope(FileScope scope);
    FileScope getFileScope();
    std::vector<File> list();

    static void setProjectPath(std::string projectPath);
    static const std::string& getProjectPath();

    virtual bool loadRaw(int fromHistory = 0);
    virtual size_t dataListSize();
protected:
    virtual void addData(unsigned char *data);
    virtual void compareFiles();

    Date loadLastModified;
    Date fileSizeChanged;
    size_t lastKnownLength;
    std::vector<unsigned char *> dataList;
    bool diff;

private:
    void freeData();

    static std::string projectPath;
    std::string filePath;
    int modifyGracePeriod;
    FileScope scope;
    EmbeddedResource *embeddedResource;
};

#endif /*ENGINE_IO_FILE_H_*/
