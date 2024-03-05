#include "FontFontStash.h"

#include "logger/logger.h"

#include <stdio.h>
#include <string.h>
#include <algorithm>

#include "GL/gl3w.h"


#define FONS_VERTEX_COUNT 2048
#define FONTS_STATIC
#define FONTSTASH_IMPLEMENTATION
#include "fontstash.h"

#define GLFONS_TEXTURE_SIZE 4096
#define GLFONS_VERTEX_ATTRIB 0
#define GLFONS_TCOORD_ATTRIB 1
#define GLFONS_COLOR_ATTRIB 3
#define GLFONTSTASH_IMPLEMENTATION
#include "gl3corefontstash.h" // OpenGL 3.3+ required

#include "graphics/ShaderProgram.h"
#include "graphics/ShaderProgramOpenGl.h"

Font* Font::newInstance(std::string filePath) {
    Font *font = new FontFontStash(filePath);
    if (font == NULL) {
        loggerFatal("Could not allocate memory for font");
    }

    return font;
}

FontFontStash::FontFontStash(std::string filePath) : Font(filePath) {
    fs = NULL;
    fontId = FONS_INVALID;
}

FontFontStash::~FontFontStash() {
    if (fs != NULL) {
        loggerDebug("Deleting font. file:'%s', font:0x%p", getFilePath().c_str(), fs);
        glfonsDelete(fs);
    }
}

bool FontFontStash::isSupported() {
    std::string fileExtension = getFileExtension();
    std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::tolower);
    if (fileExtension == "ttf") {
        return true;
    }

    return false;
}

bool FontFontStash::isError(int fontStashId) {
    switch(fontStashId) {
        case FONS_INVALID:
            return true;

        default:
            return false;
    }
}

static void fontStashHandleError(void* uptr, int error, int val) {
    const char *fontPath = "";
    FontFontStash* fontPtr = static_cast<FontFontStash*>(uptr);
    if (fontPtr != NULL) {
        fontPath = fontPtr->getFilePath().c_str();
    }

    switch(error) {
        case FONS_ATLAS_FULL:
            // Font atlas is full.
            loggerWarning("Font atlas full. error:FONS_ATLAS_FULL(%d), value:%d, path:'%s'", error, val, fontPath);
            break;
        case FONS_SCRATCH_FULL:
            // Scratch memory used to render glyphs is full, requested size reported in 'val', you may need to bump up FONS_SCRATCH_BUF_SIZE.
            loggerWarning("Font scratch full. error:FONS_SCRATCH_FULL(%d), value:%d, path:'%s'", error, val, fontPath);
            break;
        case FONS_STATES_OVERFLOW:
            // Calls to fonsPushState has created too large stack, if you need deep state stack bump up FONS_MAX_STATES.
            loggerWarning("Font states overflow. error:FONS_STATES_OVERFLOW(%d), value:%d, path:'%s'", error, val, fontPath);
            break;
        case FONS_STATES_UNDERFLOW:
            // Trying to pop too many states fonsPopState().
            loggerWarning("Font states underflow. error:FONS_STATES_UNDERFLOW(%d), value:%d, path:'%s'", error, val, fontPath);
            break;
        default:
            // Trying to pop too many states fonsPopState().
            loggerWarning("Unknown font error. error:%d, value:%d, path:'%s'", error, val, fontPath);
            break;
    }
}

bool FontFontStash::load(bool rollback) {
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

    fs = glfonsCreate(GLFONS_TEXTURE_SIZE, GLFONS_TEXTURE_SIZE, FONS_ZERO_BOTTOMLEFT);
    if (fs == NULL) {
        loggerError("Could not initialize FontStash font");
        return false;
    }

    fs->handleError = fontStashHandleError;
    fs->errorUptr = static_cast<void*>(this);

    fontId = fonsAddFontMem(fs, getFilePath().c_str(), getData(), static_cast<int>(length()), 0);
    if (isError(fontId)) {
        return false;
    }

    loggerInfo("Loaded font. file:'%s', fontId:%d, this:0x%p", getFilePath().c_str(), fontId, this);
    return true;
}

bool FontFontStash::isLoaded() {
    if (getData() != NULL && fontId != FONS_INVALID) {
        return true;
    }

    return false;
}


void FontFontStash::drawText(double x, double y, double z, double fontSize, std::string& text) {
    if (fontId == FONS_INVALID || fs == NULL) {
        loggerWarning("Font has not been initialized! font:0x%p, fontInternalId:%d", fs, fontId);
        return;
    }

    ShaderProgram::useCurrentBind();
/*
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
*/
    fonsClearState(fs);
    fonsSetFont(fs, fontId);
    fonsSetSize(fs, fontSize);
    fonsSetColor(fs, glfonsRGBA(255,255,255,255));
    fonsSetAlign(fs, FONS_ALIGN_CENTER | FONS_ALIGN_MIDDLE);
    fonsSetBlur(fs, 2.0f);
    fonsDrawText(fs, x, y, text.c_str(), NULL);
}
