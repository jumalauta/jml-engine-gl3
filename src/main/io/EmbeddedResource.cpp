#include "EmbeddedResource.h"

EmbeddedResource::EmbeddedResource(std::string filePath, const unsigned char *data, size_t dataSize) {
    this->filePath = filePath;
    this->data = data;
    this->dataSize = dataSize;
}

const std::string& EmbeddedResource::getFilePath() {
    return filePath;
}

const unsigned char *EmbeddedResource::getData() {
    return data;
}

size_t EmbeddedResource::length() {
    return dataSize;
}
