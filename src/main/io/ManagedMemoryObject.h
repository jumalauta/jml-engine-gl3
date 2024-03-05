#ifndef ENGINE_IO_MANAGEDMEMORYOBJECT_H_
#define ENGINE_IO_MANAGEDMEMORYOBJECT_H_

#include <string>

class ManagedMemoryObject {
public:
    ManagedMemoryObject();
    virtual ~ManagedMemoryObject();
    virtual const std::string &getName() = 0;

    virtual void setError(bool error);
    virtual bool getError() const;
    virtual void setErrorMessage(std::string errorMessage);
    virtual const std::string& getErrorMessage() const;

protected:
    bool error;
    std::string errorMessage;
};

#endif /*ENGINE_IO_MANAGEDMEMORYOBJECT_H_*/
