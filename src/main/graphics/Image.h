#ifndef ENGINE_GRAPHICS_IMAGE_H_
#define ENGINE_GRAPHICS_IMAGE_H_

#include "io/File.h"

#include <string>

class Texture;

class Image : public File {
public:
    virtual ~Image() {}
    virtual bool load(bool rollback=false) = 0;
    virtual bool isSupported() = 0;
    static Image* newInstance(std::string filePath);
    static bool write(Image &image, int width, int height, int channels, const void *rawData);
    int getWidth();
    void setWidth(int width);
    int getHeight();
    void setHeight(int height);
    Texture *getTexture();
protected:
    explicit Image(std::string filePath);
    int width;
    int height;
    Texture *texture;
};

#endif /*ENGINE_GRAPHICS_IMAGE_H_*/
