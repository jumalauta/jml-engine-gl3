#include "TextureOpenGl.h"
#include "Graphics.h"
#include "logger/logger.h"
#include "Settings.h"

//TODO: Support for 1D - 3D textures

// possible cave-at: bind/unbind behavior with multiple texture units could be incoherrent, as there is only single stack
std::vector<TextureOpenGl*> TextureOpenGl::bindStack = {};

void Texture::setDefault(Texture* texture) {
    defaultTexture = texture;
    TextureOpenGl::setDefault();
}

void TextureOpenGl::setDefault() {
    if (bindStack.empty()) {
        // during initialization we'll need to bind the textures
        // otherwise texture unbind mechanism handles defaultTexture binding when needed
        for (unsigned int i = 0; i < Settings::demo.graphics.maxTextureUnits; i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            TextureOpenGl *tex = dynamic_cast<TextureOpenGl*>(defaultTexture);
            glBindTexture(tex->getTargetTypeOpenGl(), tex->getId());
        }
        glActiveTexture(GL_TEXTURE0);
    } else {
        // this probably should not happen?
        loggerWarning("Texture set as default, but texture bind stack is not empty");
    }
}

Texture* Texture::newInstance() {
    Texture* texture = new TextureOpenGl();
    if (texture == NULL) {
        loggerFatal("Could not allocate memory for texture");
    }

    return texture;
}

TextureOpenGl::TextureOpenGl() {
    id = 0;
    width = 0;
    height = 0;
    setDataType(Settings::demo.graphics.defaultTextureDataType);
    setTargetType(Settings::demo.graphics.defaultTextureTargetType);
    setType(Settings::demo.graphics.defaultTextureType);
    setFormat(Settings::demo.graphics.defaultTextureFormat);
    setWrap(Settings::demo.graphics.defaultTextureWrap);
    setFilter(Settings::demo.graphics.defaultTextureFilter);
}

TextureOpenGl::~TextureOpenGl() {
    free();
}

bool TextureOpenGl::generate() {
    PROFILER_BLOCK("TextureOpenGl::generate");

    free();

    glGenTextures(1, &id);
    if (id == 0) {
        loggerError("Could not generate texture ID! texture:0x%p", this);
        return false;
    }

    return true;
}

void TextureOpenGl::free() {
    PROFILER_BLOCK("TextureOpenGl::free");

    if (id != 0) {
        glDeleteTextures(1, &id);
        Graphics &graphics = Graphics::getInstance();
        if (graphics.handleErrors()) {
            loggerError("Could not free texture. texture:0x%p, id:%u", this, id);
        } else {
            loggerTrace("Freed texture. texture:0x%p", this);
        }

        id = 0;
    }
}

void TextureOpenGl::bind(unsigned int textureUnit) {
    PROFILER_BLOCK("TextureOpenGl::bind");

    bindStack.push_back(this);

    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(getTargetTypeOpenGl(), id);
    glActiveTexture(GL_TEXTURE0);
}

void TextureOpenGl::unbind(unsigned int textureUnit) {
    PROFILER_BLOCK("TextureOpenGl::unbind");

    bindStack.pop_back();

    GLuint parentId = 0;
    if (Texture::defaultTexture) {
        parentId = dynamic_cast<TextureOpenGl*>(defaultTexture)->getId();
    }
    if (! bindStack.empty()) {
        parentId = bindStack.back()->getId();
    }

    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(getTargetTypeOpenGl(), parentId);
    glActiveTexture(GL_TEXTURE0);
}

void TextureOpenGl::setType(TextureType type) {
    this->type = type;
}

const char *TextureOpenGl::getTypeName() {
    switch(type) {
        case TextureType::DIFFUSE:
            return "DIFFUSE";
        case TextureType::SPECULAR:
            return "SPECULAR";
        case TextureType::AMBIENT:
            return "AMBIENT";
        case TextureType::NORMAL:
            return "NORMAL";
        default:
            return "Unknown type";
    }
}

void TextureOpenGl::setFormat(TextureFormat format) {
    this->format = format;
}

GLenum TextureOpenGl::getFormatOpenGl() {
    switch(format) {
        default:
            loggerError("Unknown format property. format:%d, texture:0x%p", format, this);

        case TextureFormat::RGBA:
            return GL_RGBA;

        case TextureFormat::RGB:
            return GL_RGB;

        case TextureFormat::RED:
            return GL_RED;

        case TextureFormat::DEPTH_COMPONENT:
            return GL_DEPTH_COMPONENT;
    }
}

const char *TextureOpenGl::getFormatName() {
    switch(format) {
        case TextureFormat::DEPTH_COMPONENT:
            return "DEPTH_COMPONENT";
        case TextureFormat::RGBA:
            return "RGBA";
        case TextureFormat::RGB:
            return "RGB";
        case TextureFormat::RED:
            return "RED";
        default:
            return "Unknown format";
    }
}

void TextureOpenGl::setWrap(TextureWrap wrap) {
    this->wrap = wrap;
}

const char *TextureOpenGl::getWrapName() {
    switch(wrap) {
        case TextureWrap::REPEAT:
            return "REPEAT";
        case TextureWrap::MIRRORED_REPEAT:
            return "MIRRORED_REPEAT";
        case TextureWrap::CLAMP_TO_EDGE:
            return "CLAMP_TO_EDGE";
        case TextureWrap::CLAMP_TO_BORDER:
            return "CLAMP_TO_BORDER";
        default:
            return "Unknown wrap";
    }
}

void TextureOpenGl::setFilter(TextureFilter filter) {
    this->filter = filter;
}

const char *TextureOpenGl::getFilterName() {
    switch(filter) {
        case TextureFilter::NEAREST:
            return "NEAREST";
        case TextureFilter::LINEAR:
            return "LINEAR";
        case TextureFilter::MIPMAP:
            return "MIPMAP";
        default:
            return "Unknown wrap";
    }
}

void TextureOpenGl::setTargetType(TextureTargetType targetType) {
    this->targetType = targetType;
}

const char *TextureOpenGl::getTargetTypeName() {
    switch(targetType) {
        case TextureTargetType::TEXTURE_2D:
            return "TEXTURE_2D";
        case TextureTargetType::TEXTURE_1D_ARRAY:
            return "TEXTURE_1D_ARRAY";
        default:
            return "Unknown target type";
    }
}

GLenum TextureOpenGl::getTargetTypeOpenGl() {
    switch(targetType) {
        default:
            loggerError("Unknown texture target type. targetType:%d, texture:0x%p", targetType, this);

        case TextureTargetType::TEXTURE_2D:
            return GL_TEXTURE_2D;

        case TextureTargetType::TEXTURE_1D_ARRAY:
            return GL_TEXTURE_1D_ARRAY;
    }
}

void TextureOpenGl::setDataType(TextureDataType dataType) {
    this->dataType = dataType;
}

GLenum TextureOpenGl::getDataTypeOpenGl() {
    switch(dataType) {
        default:
            loggerError("Unknown texture data type. dataType:%d, texture:0x%p", dataType, this);

        case TextureDataType::UNSIGNED_BYTE:
            return GL_UNSIGNED_BYTE;

        case TextureDataType::FLOAT:
            return GL_FLOAT;
    }
}

void TextureOpenGl::applyWrapProperties() {
    switch(wrap) {
        case TextureWrap::REPEAT:
            glTexParameteri(getTargetTypeOpenGl(), GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(getTargetTypeOpenGl(), GL_TEXTURE_WRAP_T, GL_REPEAT);
            break;
        case TextureWrap::MIRRORED_REPEAT:
            glTexParameteri(getTargetTypeOpenGl(), GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
            glTexParameteri(getTargetTypeOpenGl(), GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
            break;
        case TextureWrap::CLAMP_TO_EDGE:
            glTexParameteri(getTargetTypeOpenGl(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(getTargetTypeOpenGl(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            break;
        case TextureWrap::CLAMP_TO_BORDER:
            glTexParameteri(getTargetTypeOpenGl(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(getTargetTypeOpenGl(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            break;
        default:
            loggerError("Unknown wrap property. wrap:%d, texture:0x%p", wrap, this);
            break;
    }
}

void TextureOpenGl::applyFilterProperties() {
    switch(filter) {
        case TextureFilter::NEAREST:
            glTexParameteri(getTargetTypeOpenGl(), GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(getTargetTypeOpenGl(), GL_TEXTURE_MIN_FILTER, GL_NEAREST);    
            break;
        case TextureFilter::LINEAR:
            glTexParameteri(getTargetTypeOpenGl(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(getTargetTypeOpenGl(), GL_TEXTURE_MIN_FILTER, GL_LINEAR);    
            break;
        case TextureFilter::MIPMAP:
            glTexParameteri(getTargetTypeOpenGl(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(getTargetTypeOpenGl(), GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            break;
        default:
            loggerError("Unknown filter property. filter:%d, texture:0x%p", filter, this);
            break;
    }
}

void TextureOpenGl::processFilterProperties() {
    if (filter == TextureFilter::MIPMAP) {
        glGenerateMipmap(getTargetTypeOpenGl());
    }
}

bool TextureOpenGl::create(int width, int height, const void *data) {
    PROFILER_BLOCK("TextureOpenGl::create");

    this->width = static_cast<GLsizei>(width);
    this->height = static_cast<GLsizei>(height);

    if (generate() == false) {
        return false;
    }

    bind();

    applyWrapProperties();

    applyFilterProperties();

    GLenum glFormat = getFormatOpenGl();
    glTexImage2D(getTargetTypeOpenGl(), 0, glFormat, this->width, this->height, 0, glFormat, getDataTypeOpenGl(), data);

    processFilterProperties();

    Graphics &graphics = Graphics::getInstance();
    if (graphics.handleErrors()) {
        loggerError("Could not create texture. texture:0x%p", this);
        return false;
    }

    loggerDebug("Created texture. id:%u dimensions:%dx%d, type:%s, format:%s, wrap:%s, filter:%s, texture:0x%p",
        id, width, height, getTypeName(), getFormatName(), getWrapName(), getFilterName(), this);

    unbind();

    if (graphics.handleErrors()) {
        loggerWarning("Errorssss!. texture:0x%p", this);
    }

    return true;
}

bool TextureOpenGl::update(const void *data) {
    if (id == 0) {
        loggerError("Texture not generated, cannot update. id:%u dimensions:%dx%d, format:%s, wrap:%s, filter:%s, targetType:%s, texture:0x%p, data:0x%p",
            id, width, height, getFormatName(), getWrapName(), getFilterName(), getTargetTypeName(), this, data);

        return false;
    }

    bind();
    glTexSubImage2D(getTargetTypeOpenGl(), 0, 0, 0, this->width, this->height, getFormatOpenGl(), getDataTypeOpenGl(), data);
    processFilterProperties();
    unbind();

    return true;
}

GLuint TextureOpenGl::getId() {
    return id;
}
