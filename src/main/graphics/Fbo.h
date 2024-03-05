#ifndef ENGINE_GRAPHICS_FBO_H_
#define ENGINE_GRAPHICS_FBO_H_

#include "io/ManagedMemoryObject.h"
#include <string>

class Texture;

/**
 * Frame Buffer Object (FBO) information
 */
class Fbo : public ManagedMemoryObject {
public:
    static Fbo* newInstance(std::string name);
    static void reset();
    virtual ~Fbo();
    virtual bool generate() = 0;
    virtual void free() = 0;
    virtual void bind() = 0;
    virtual void unbind() = 0;
    virtual void start() = 0;
    virtual void end() = 0;
    virtual void setDimensions(unsigned int width, unsigned int height) = 0;
    virtual unsigned int getWidth() = 0;
    virtual unsigned int getHeight() = 0;
    virtual void textureBind() = 0;
    virtual void textureUnbind() = 0;

    virtual Texture *getColorTexture() = 0;
    virtual Texture *getDepthTexture() = 0;

    const std::string &getName();
protected:
    Fbo(std::string name);
private:
    std::string name;
};

#endif /*ENGINE_GRAPHICS_FBO_H_*/
