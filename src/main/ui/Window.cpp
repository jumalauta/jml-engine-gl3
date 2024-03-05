#include "Window.h"

Window::Window() {
    title = "";
    extraInfo = "";
    width = 0;
    height = 0;
}

bool Window::setTitle(std::string title, std::string extraInfo) {
    bool changed = false;

    if (this->title != title || this->extraInfo != extraInfo) {
        changed = true;
    }

    this->title = title;
    this->extraInfo = extraInfo;

    return changed;
}

std::string Window::getTitle() {
    return title;
}

void Window::setDimensions(unsigned int width, unsigned int height) {
    this->width = width;
    this->height = height;
}
unsigned int Window::getWidth() {
    return width;
}
unsigned int Window::getHeight() {
    return height;
}
