#ifndef ENGINE_GRAPHICS_TEXTUREOPENGL_H_
#define ENGINE_GRAPHICS_TEXTUREOPENGL_H_

#include "Texture.h"

#include "GL/gl3w.h"

#include <vector>

class TextureOpenGl : public Texture {
public:
    static void setDefault();
    TextureOpenGl();
    ~TextureOpenGl();
    bool generate();
    void free();
    void bind(unsigned int textureUnit = 0);
    void unbind(unsigned int textureUnit = 0);
    void setType(TextureType type);
    void setFormat(TextureFormat format);
    void setWrap(TextureWrap wrap);
    void setFilter(TextureFilter filter);
    void setTargetType(TextureTargetType targetType);
    void setDataType(TextureDataType dataType);
    void applyWrapProperties();
    void applyFilterProperties();
    void processFilterProperties();
    bool create(int width, int height, const void *data = NULL);
    bool update(const void *data);
    GLuint getId();
    GLenum getTargetTypeOpenGl();
    GLenum getFormatOpenGl();
    GLenum getDataTypeOpenGl();
protected:
    const char *getTypeName();
    const char *getFormatName();
    const char *getWrapName();
    const char *getFilterName();
    const char *getTargetTypeName();

    GLuint id;
    GLsizei width;
    GLsizei height;
    TextureDataType dataType;
    TextureType type;
    TextureFormat format;
    TextureWrap wrap;
    TextureFilter filter;
    TextureTargetType targetType;

    static std::vector<TextureOpenGl*> bindStack;
};

#endif /*ENGINE_GRAPHICS_TEXTUREOPENGL_H_*/
