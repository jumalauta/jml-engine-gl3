#ifndef ENGINE_GRAPHICS_FBOOPENGL_H_
#define ENGINE_GRAPHICS_FBOOPENGL_H_

#include "Fbo.h"
#include "GL/gl3w.h"

#include <vector>

class Texture;

class FboOpenGl : public Fbo {
public:
    static void reset();
    FboOpenGl(std::string name);
    ~FboOpenGl();
    GLuint getId();
    GLuint getDepthBufferId();
    bool generate();
    void free();
    void bind();
    void unbind();
    void start();
    void end();
    void setDimensions(unsigned int width, unsigned int height);
    unsigned int getWidth();
    unsigned int getHeight();
    void textureBind();
    void textureUnbind();

    Texture *getColorTexture();
    Texture *getDepthTexture();
private:
    GLenum checkFramebufferStatus();

    GLuint id;
    Texture *color;

    bool storeDepth;
    bool storeColor;
    GLuint depthBuffer;
    Texture *depth;

    unsigned int width;
    unsigned int height;
    unsigned int depthTextureUnit;
    unsigned int colorTextureUnit;

    static std::vector<FboOpenGl*> bindStack;
};

#endif /*ENGINE_GRAPHICS_FBOOPENGL_H_*/
