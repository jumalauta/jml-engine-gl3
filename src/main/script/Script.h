#ifndef ENGINE_SCRIPT_SCRIPT_H_
#define ENGINE_SCRIPT_SCRIPT_H_

#include "io/File.h"

#include <string>

class Script : public File {
public:
    virtual ~Script() {}
    virtual bool load(bool rollback=false) = 0;
    virtual bool isSupported() = 0;
    virtual void free() = 0;
    virtual void setInitClassCall(std::string eval) = 0;
    virtual void setInitCall(std::string eval) = 0;
    virtual void setExitCall(std::string eval) = 0;
    virtual void setExitClassCall(std::string eval) = 0;
    static Script* newInstance(std::string filePath);
    virtual bool evalString(std::string eval) = 0;

protected:
    explicit Script(std::string filePath);
};

#endif /*ENGINE_SCRIPT_SCRIPT_H_*/
