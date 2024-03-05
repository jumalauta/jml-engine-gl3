#ifndef ENGINE_IO_EMBEDDEDRESOURCE_H_
#define ENGINE_IO_EMBEDDEDRESOURCE_H_

#include <string>
#include <ctype.h>

class EmbeddedResource {
public:
    explicit EmbeddedResource(std::string filePath, const unsigned char *data, size_t dataSize);
    const std::string& getFilePath();
    const unsigned char *getData();
    size_t length();
private:
    std::string filePath;
    const unsigned char *data;
    size_t dataSize;
};

#endif /*ENGINE_IO_EMBEDDEDRESOURCE_H_*/
