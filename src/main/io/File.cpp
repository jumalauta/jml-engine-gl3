#include "File.h"

#include "logger/logger.h"
#include "time/Date.h"
#include "time/SystemTime.h"
#include "io/EmbeddedResourceManager.h"
#include "io/EmbeddedResource.h"
#include "Settings.h"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
// we're aiming for 64bit platforms, but in case of 32bit platforms need to consider -D_FILE_OFFSET_BITS=64
// in case there will be > 2 GB files
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <math.h>

#include <algorithm> 
#include <cstdio>
#include <cstdlib>

std::string File::projectPath = "data/";

void File::setProjectPath(std::string projectPath) {
    File::projectPath = "";

    File path = File(projectPath);
    if (!path.exists()) {
        loggerTrace("Project path '%s' not existing. Using current directory.", projectPath.c_str());
        return;
    }

    if (!path.isDirectory()) {
        loggerError("Project path is not a directory: '%s'", projectPath.c_str());
        return;
    }

    if (projectPath.back() != '/') {
        projectPath += '/';
    }

    File::projectPath = projectPath;

    loggerInfo("Project path: %s", File::projectPath.c_str());
}

const std::string& File::getProjectPath() {
    return projectPath;
}

File::File(std::string filePath) {
    this->dataList.clear();
    this->embeddedResource = NULL;
    this->scope = FileScope::VARIABLE;
    fileSizeChanged = Date(0);
    lastKnownLength = 0;
    diff = false;

    setModifyGracePeriod(Settings::gui.fileModifyGracePeriod);

    this->filePath = File::projectPath + filePath;
    if (!exists()) {
        this->filePath = filePath;

        if (!exists()) {
            EmbeddedResourceManager& embeddedResourceManager = EmbeddedResourceManager::getInstance();
            EmbeddedResource *embeddedResource = embeddedResourceManager.getResource(this->filePath, false);
            if (embeddedResource != NULL) {
                this->embeddedResource = embeddedResource;
                this->scope = FileScope::CONSTANT;
            }
        }
    }

    loadLastModified = Date(0);
}

File::~File() {
    //loggerTrace("File::~File %s", filePath.c_str());
    freeData();
}

const std::string& File::getFilePath() {
    return filePath;
}

const std::string& File::getName() {
    return filePath;
}

std::string File::getFileExtension() {
    std::size_t fileExtensionIndex = filePath.find_last_of(".");
    if (fileExtensionIndex == std::string::npos) {
        return std::string("");
    }

    return filePath.substr(fileExtensionIndex + 1);
}

bool File::isSupported() {
    //File supports everything
    //Inherited implementation may pose restrictions
    return true;
}

/*
Small note: use stat() so that it'll follow symbolic links through.

from man:
- stat() stats the file pointed to by path and fills in buf.
- lstat() is identical to stat(), except that if path is a symbolic link, then the link itself is stat-ed, not the file that it refers to. 
*/

bool File::isDirectory() {
    if (embeddedResource != NULL) {
        return false;
    }

    struct stat s;
    if (stat(filePath.c_str(), &s) == 0) {
        if (s.st_mode & S_IFDIR) {
            return true;
        } else {
            return false;
        }
    }

    //does not exist => not a directory
    return false;
}

bool File::isFile() {
    if (embeddedResource != NULL) {
        return true;
    }

    struct stat s;
    if (stat(filePath.c_str(), &s) == 0) {
        if (s.st_mode & S_IFDIR) {
            //is a directory => not a file
            return false;
        } else {
            //is not a directory => it is a file
            return true;
        }
    }

    //does not exist => not a directory
    return false;
}

bool File::exists() {
    if (embeddedResource != NULL) {
        return true;
    }

    struct stat s;
    if (stat(filePath.c_str(), &s) == 0) {
        return true;
    }

    return false;
}

void File::setModifyGracePeriod(int modifyGracePeriod) {
    this->modifyGracePeriod = modifyGracePeriod;
}

bool File::modified() {
    if (isLoaded()) {
        uint64_t lastModifiedTime = lastModified().getTime();
        if (lastModifiedTime > loadLastModified.getTime()
                && lastModifiedTime < Date(Date().getTime() - modifyGracePeriod).getTime()) {
            int waits = 0;
            do {
                lastModifiedTime = lastModified().getTime();
                SystemTime::sleepInMillis(modifyGracePeriod);
                waits++;
            } while (lastModifiedTime != lastModified().getTime());

            loggerInfo("File is modified! '%s', lastmodified:%u, now:%u, waits:%d", filePath.c_str(), lastModifiedTime, Date().getTime(), waits);
            return true;
        }
    }

    return false;
}

Date File::lastModified() {
    if (embeddedResource != NULL) {
        return Date(0);
    }

    struct stat s;
    if (stat(filePath.c_str(), &s) == 0) {
        length(); // check if size has changed

        // mtime = when content was modified, so presumably always latest
        // ctime = file creation (windows); file permissions etc changes in nix
        time_t modified = std::max(s.st_mtime, s.st_ctime);
        Date modifiedDate = Date(static_cast<uint64_t>(modified * 1000));
        if (modifiedDate.getTime() < fileSizeChanged.getTime()) {
            // identified file size change times are overriding OS file times
            modifiedDate = fileSizeChanged;
        }

        return modifiedDate;
    }

    return Date(0);
}

// FIXME: getData() should not use this to determine length as file content may change on-the-fly
size_t File::length() {
    if (embeddedResource != NULL) {
        return embeddedResource->length();
    }

    // stat.st_size is not realtime (at least on Windows), so don't use that
    // open + seek should give realtime size information, so we can determine
    // if there's write operations on-going to file (and not refresh when write in-progress)
    int f = open(filePath.c_str(), O_RDONLY);
    size_t fileSize = lseek(f, 0, SEEK_END);
    close(f);

    if (fileSize != lastKnownLength) {
        // st_mtime / st_ctime ain't reliable enough to determine if there're file modifications on-going (audio / image conversion)
        // we'll check modifications based on changes in file size and alternatively st_mtime / st_ctime
        fileSizeChanged = Date();
        lastKnownLength = fileSize;
    }

    return fileSize;
}

unsigned char *File::getData() {
    if (dataList.empty()) {
        return NULL;
    }

    return dataList.back();
}

bool File::load(bool rollback) {
    if (rollback) {
        loggerWarning("Rollback not supported. file:'%s'", filePath.c_str());
        return false;
    }

    bool loaded = loadRaw();
    if (loaded) {
        loadLastModified = lastModified();
    }

    return loaded;
}

bool File::isLoaded() {
    if (!dataList.empty()) {
        return true;
    }

    return false;
}

void File::freeData() {
    if (!dataList.empty()) {
        for(unsigned char *rawData : dataList) {
            delete [] rawData;
        }

        dataList.clear();
    }
}

void File::addData(unsigned char *data) {
    if (data == NULL) {
        loggerWarning("Attempting to add NULL data for file '%s'.", filePath.c_str());
        return;
    }

    dataList.push_back(data);

    if (dataList.size() - 1 > static_cast<unsigned int>(Settings::gui.fileHistorySize)) {
        delete [] dataList.front();
        dataList.erase(dataList.begin());
    }
}

void File::compareFiles() {
    if (!Settings::gui.diff) {
        // diff not supported
        return;
    }

    if (!diff) {
        // diff not supported with this particular file
        return;
    }

    if (dataList.size() < 2) {
        // Not enough data for a diff
        return;
    }

    char *temporaryName = tempnam("", "engine_diff_");
    std::FILE* f = std::fopen(temporaryName, "wb");
    if (!f) {
        loggerWarning("Could not open temporary file for writing. temporaryFile:'%s', file:'%s'", temporaryName, getFilePath().c_str());
        std::free(temporaryName);
        return;
    }
    unsigned char * oldData = dataList[dataList.size() - 2];
    std::fputs(reinterpret_cast<char*>(oldData), f);
    std::fclose(f);

    std::string command = Settings::gui.diffCommand;

    const std::string oldFileWord = "<oldFile>";
    const std::string newFileWord = "<newFile>";

    if (command.find(oldFileWord) != std::string::npos) {
        command.replace(command.find(oldFileWord), oldFileWord.length(), std::string(temporaryName));
    }

    if (command.find(newFileWord) != std::string::npos) {
        command.replace(command.find(newFileWord), newFileWord.length(), getFilePath());
    }

    std::FILE* diffProcess = popen(command.c_str(), "r");
    if (!diffProcess) {
        loggerWarning("Could not run diff. command:'%s', file:'%s'", command.c_str(), getFilePath().c_str());
        std::remove(temporaryName);
        std::free(temporaryName);
        return;
    }
    
    std::array<char, 512> buffer;
    std::string diffOutput;
    while (fgets(buffer.data(), buffer.size(), diffProcess) != NULL) {
        diffOutput += buffer.data();
    }

    pclose(diffProcess);
    std::remove(temporaryName); 
    std::free(temporaryName);

    loggerInfo("File diff. file:'%s', diff results:\n%s", getFilePath().c_str(), diffOutput.c_str());
}

size_t File::dataListSize() {
    return dataList.size();
}

bool File::loadRaw(int fromHistory) {
    size_t fileSize = 0;
    unsigned char *data = NULL;

    if (fromHistory > 0) {
        if (dataListSize() <= static_cast<unsigned int>(fromHistory)) {
            loggerDebug("Attempted to load non-existing file '%s' from history!", filePath.c_str());
            return false;
        }

        unsigned char * oldData = dataList[dataListSize() - (fromHistory+1)];
        fileSize = strlen(reinterpret_cast<char*>(oldData));

        data = new unsigned char[fileSize + 1];
        std::copy_n(oldData, fileSize, data);
        //always add null termination to the read data
        data[fileSize] = '\0';
    }

    if (data == NULL) {
        if (embeddedResource != NULL) {
            fileSize = length();
            data = new unsigned char[fileSize + 1];
            if (data == NULL) {
                loggerFatal("Could not allocate memory for file '%s'.", filePath.c_str());
                return false;
            }

            std::copy_n(embeddedResource->getData(), fileSize, data);
            //always add null termination to the read data
            data[fileSize] = '\0';
        } else {
            FILE *fp = fopen(filePath.c_str(),"rb");
            if (fp == NULL) {
                if (!exists()) {
                    loggerWarning("File '%s' does not exist.", filePath.c_str());
                } else {
                    loggerError("Could not open file '%s' for reading.", filePath.c_str());
                }

                return false;
            }

            fileSize = length();
            if (fileSize <= 0) {
                loggerWarning("Can't read '%s' to data. File is empty.", filePath.c_str());
                fclose(fp);
                return false;
            }

            data = new unsigned char[fileSize + 1];
            if (data == NULL) {
                loggerFatal("Could not allocate memory for file '%s'.", filePath.c_str());
                fclose(fp);
                return false;
            }
            
            size_t readBytes = fread(data, sizeof(unsigned char), fileSize, fp);
            if (readBytes != fileSize) {
                loggerError("Could not read file '%s'. readBytes:%d, fileSize:%d", filePath.c_str(), readBytes, fileSize);
                fclose(fp);
                delete [] data;
                data = NULL;
                return false;
            }
            fclose(fp);

            //always add null termination to the read data
            data[readBytes] = '\0';
        }
    }

    addData(data);

    return true;
}

void File::setFileScope(FileScope scope) {
    this->scope = scope;
}

FileScope File::getFileScope() {
    return this->scope;
}

std::vector<File> File::list()
{
    std::vector<File> filePaths;
    if (!this->isDirectory()) {
        return filePaths;
    }

    DIR *dir = opendir(this->getFilePath().c_str());
    if (!dir) {
        return filePaths;
    }

    struct dirent *direntIterator;
    while ((direntIterator = readdir(dir)) != NULL) {
        if (!strcmp(direntIterator->d_name, ".") || !strcmp(direntIterator->d_name, "..")) {
            continue;
        }

        std::string currentPath = this->getFilePath() + "/";
        File file = File(currentPath + direntIterator->d_name);
        if (file.exists()) {
            filePaths.push_back(file);
        }
    }

    closedir(dir);

    return filePaths;
}
