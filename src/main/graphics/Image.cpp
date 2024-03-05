#include "Image.h"

#include "Settings.h"

Image::Image(std::string filePath) : File(filePath) {
    width = 0;
    height = 0;
    texture = NULL;

    setModifyGracePeriod(Settings::gui.largeFileModifyGracePeriod);
}

int Image::getWidth() {
    return width;
}

void Image::setWidth(int width) {
    this->width = width;
}

int Image::getHeight() {
    return height;
}

void Image::setHeight(int height) {
    this->height = height;
}

Texture* Image::getTexture() {
    return texture;
}
