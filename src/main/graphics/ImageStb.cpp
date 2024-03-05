#include "ImageStb.h"

#include "graphics/Texture.h"
#include "logger/logger.h"

#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

#include <algorithm>

Image* Image::newInstance(std::string filePath) {
    Image *image = new ImageStb(filePath);
    if (image == NULL) {
        loggerFatal("Could not allocate memory for image file:'%s'", filePath.c_str());
        return NULL;
    }

    if (! image->isSupported()) {
        delete image;
        return NULL;
    }

    return image;
}

bool Image::write(Image &image, int width, int height, int channels, const void *rawData) {
    if (image.exists() && !image.isFile()) {
        loggerError("Could not write image. Not a file. file:'%s'", image.getFilePath().c_str());
        return false;
    }

    if (!image.isSupported()) {
        loggerError("Could not write image. File type not supported. file:'%s'", image.getFilePath().c_str());
        return false;
    }

    //stbi_set_flip_vertically_on_load missing from stb_write, do manually
    //TODO: PR to stb_image_write.h

    size_t allocatedBytes = width * height * channels;
    unsigned char *flippedRawData = new unsigned char[allocatedBytes];
    if (flippedRawData == NULL) {
        loggerFatal("Could not allocate memory for image writing");
        return false;
    }

    // flip vertically
    size_t lineSize = width * channels;
    for(size_t pos = 0; pos < allocatedBytes; pos += lineSize) {
        memcpy(flippedRawData + pos, reinterpret_cast<const unsigned char*>(rawData) + (allocatedBytes - lineSize - pos), lineSize);
    }

    const int STRIDE_IN_BYTES = 0;
    if (!stbi_write_png(image.getFilePath().c_str(), width, height, channels, static_cast<const void*>(flippedRawData), STRIDE_IN_BYTES)) {
        loggerError("Could not write image. file:'%s', width:%d, height:%d, channels:%d, rawData:%p",
            image.getFilePath().c_str(), width, height, channels, rawData);
        return false;
    }

    delete [] flippedRawData;

    image.setWidth(width);
    image.setHeight(height);

    if (!image.exists()) {
        //impossible error? file was written but does not exist.
        loggerError("Could not write image. file:'%s', width:%d, height:%d, channels:%d, rawData:%p",
            image.getFilePath().c_str(), width, height, channels, rawData);
        return false;
    }

    return true;
}

ImageStb::ImageStb(std::string filePath) : Image(filePath) {
}

ImageStb::~ImageStb() {
    if (texture != NULL) {
        loggerTrace("Deconstructing image and texture. file:'%s', texture:0x%p", getFilePath().c_str(), texture);
        delete texture;
    }
}

bool ImageStb::isSupported() {
    std::string fileExtension = getFileExtension();
    std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::tolower);
    if (fileExtension == "png") {
        return true;
    }

    return false;
}

bool ImageStb::load(bool rollback) {
    loadLastModified = lastModified();

    if (!isFile()) {
        loggerError("Not a file. file:'%s'", getFilePath().c_str());
        return false;
    }

    if (!isSupported()) {
        loggerError("File type not supported. file:'%s'", getFilePath().c_str());
        return false;
    }

    if (!loadRaw()) {
        loggerError("Could not load file. file:'%s'", getFilePath().c_str());
        return false;
    }

    //flip image to meet OpenGL's expectations
    stbi_set_flip_vertically_on_load(1);

    int width = 0;
    int height = 0;
    int channels = 0;
    const int FORCE_RGBA = 4;
    unsigned char *data = stbi_load_from_memory(getData(), static_cast<int>(length()), &width, &height, &channels, FORCE_RGBA);
    setWidth(width);
    setHeight(height);

    if (texture == NULL) {
        texture = Texture::newInstance();
    }

    if (texture->create(width, height, data) == false) {
        loggerError("Could not load image, error creating texture. file:'%s' width:%d, height:%d, texture:0x%p",
            getFilePath().c_str(), width, height, texture);

        stbi_image_free(data);
        return false;        
    }

    if (getFileScope() == FileScope::CONSTANT) {
        loggerDebug("Loaded image. file:'%s' width:%d, height:%d, texture:0x%p",
            getFilePath().c_str(), width, height, texture);
    } else {
        loggerInfo("Loaded image. file:'%s' width:%d, height:%d, texture:0x%p",
            getFilePath().c_str(), width, height, texture);
    }

    stbi_image_free(data);

    return true;
}
