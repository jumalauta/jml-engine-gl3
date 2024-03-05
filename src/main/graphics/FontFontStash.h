#ifndef ENGINE_GRAPHICS_FONTFONTSTASH_H_
#define ENGINE_GRAPHICS_FONTFONTSTASH_H_

#include "Font.h"
#include "fontstash.h"

class FontFontStash : public Font {
public:
    explicit FontFontStash(std::string filePath);
    ~FontFontStash();

    bool load(bool rollback=false);
    bool isLoaded();
    bool isSupported();
    void drawText(double x, double y, double z, double fontSize, std::string& text);
private:
    bool isError(int fontStashId);

    FONScontext* fs;
    int fontId;
};

#endif /*ENGINE_GRAPHICS_FONTFONTSTASH_H_*/
