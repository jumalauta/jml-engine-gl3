#include "ManagedMemoryObject.h"

ManagedMemoryObject::ManagedMemoryObject() {
    error = false;
}

ManagedMemoryObject::~ManagedMemoryObject() {
}

void ManagedMemoryObject::setError(bool error) {
    this->error = error;
    
    if (!error) {
        setErrorMessage("");
    }
}

bool ManagedMemoryObject::getError() const {
    return error;
}

void ManagedMemoryObject::setErrorMessage(std::string errorMessage) {
    this->errorMessage = errorMessage;
}

const std::string& ManagedMemoryObject::getErrorMessage() const {
    return errorMessage;
}
