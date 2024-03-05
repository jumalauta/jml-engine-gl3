#ifndef ENGINE_GRAPHICS_FONT_H_
#define ENGINE_GRAPHICS_FONT_H_

#include "io/File.h"

#include <string>

class Font : public File {
public:
    virtual ~Font() {}
    virtual bool load(bool rollback=false) = 0;
    virtual bool isSupported() = 0;
    virtual void drawText(double x, double y, double z, double fontSize, std::string& text) = 0;

    static Font* newInstance(std::string filePath);
protected:
    explicit Font(std::string filePath);
};

#endif /*ENGINE_GRAPHICS_FONT_H_*/
