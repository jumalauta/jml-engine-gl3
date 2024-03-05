#ifndef ENGINE_GRAPHICS_TEXTURE_H_
#define ENGINE_GRAPHICS_TEXTURE_H_

#include "datatypes.h"

#include <cstddef>

class Texture {
public:
    static void setDefault(Texture* texture);
    static Texture* newInstance();
    virtual ~Texture() {}
    virtual bool generate() = 0;
    virtual void free() = 0;
    virtual void bind(unsigned int textureUnit = 0) = 0;
    virtual void unbind(unsigned int textureUnit = 0) = 0;
    virtual void setType(TextureType type) = 0;
    virtual void setFormat(TextureFormat format) = 0;
    virtual void applyWrapProperties() = 0;
    virtual void applyFilterProperties() = 0;
    virtual void processFilterProperties() = 0;
    virtual bool create(int width, int height, const void *data = NULL) = 0;
    virtual bool update(const void *data) = 0;
    virtual void setWrap(TextureWrap wrap) = 0;
    virtual void setFilter(TextureFilter filter) = 0;
    virtual void setTargetType(TextureTargetType targetType) = 0;
    virtual void setDataType(TextureDataType dataType) = 0;
protected:
    static Texture* defaultTexture;
    Texture() {};
};

#endif /*ENGINE_GRAPHICS_TEXTURE_H_*/
