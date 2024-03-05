#include "FboOpenGl.h"
#include "Graphics.h"
#include "TextureOpenGl.h"
#include "Settings.h"
#include "logger/logger.h"

std::vector<FboOpenGl*> FboOpenGl::bindStack = {};

Fbo* Fbo::newInstance(std::string name) {
    Fbo *fbo = new FboOpenGl(name);
    if (fbo == NULL) {
        loggerFatal("Could not allocate memory for FBO '%s'", name.c_str());
    }

    return fbo;
}

void Fbo::reset() {
    FboOpenGl::reset();
}

void FboOpenGl::reset() {
    loggerDebug("Reseting FBO bind stack");
    while (!bindStack.empty()) {
        bindStack.back()->unbind();
    }
}

FboOpenGl::FboOpenGl(std::string name) : Fbo(name) {
    id = 0;
    color = NULL;
    depth = NULL;
    storeColor = true;
    storeDepth = true;
    depthBuffer = 0;
    colorTextureUnit = 0;
    depthTextureUnit = 1;
}

FboOpenGl::~FboOpenGl() {
    free();
}

GLuint FboOpenGl::getId() {
    return id;
}

GLuint FboOpenGl::getDepthBufferId() {
    return depthBuffer;
}

GLenum FboOpenGl::checkFramebufferStatus() {
    GLenum e = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    const char *statusString = NULL;
    switch(e) {
        case GL_FRAMEBUFFER_COMPLETE:
            statusString = "GL_FRAMEBUFFER_COMPLETE";
            break;
        case GL_FRAMEBUFFER_UNDEFINED:
            statusString = "GL_FRAMEBUFFER_UNDEFINED";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            statusString = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            statusString = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            statusString = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            statusString = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            statusString = "GL_FRAMEBUFFER_UNSUPPORTED";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            statusString = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
            statusString = "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
            break;
        default:
            statusString = "Unknown error code";
            break;
    }

    if (e != GL_FRAMEBUFFER_COMPLETE) {
        loggerError("FBO framebuffer status not OK. name:'%s', error:%s (0x%X)", getName().c_str(), statusString, e);
    }

    return e;
}

bool FboOpenGl::generate() {
    PROFILER_BLOCK("FboOpenGl::generate");

    setDimensions(Settings::window.screenAreaWidth, Settings::window.screenAreaHeight);

    if (storeColor) {
        if (color == NULL) {
            color = Texture::newInstance();
            color->setFilter(Settings::demo.graphics.defaultFboTextureFilter);
            color->setWrap(Settings::demo.graphics.defaultFboTextureWrap);

            if (color->create(getWidth(), getHeight()) == false) {
                loggerError("Could not create FBO color texture. name:'%s'", getName().c_str());
                return false;
            }
        }
    }

    if (storeDepth) {
        if (depth == NULL) {
            depth = Texture::newInstance();
            depth->setFilter(Settings::demo.graphics.defaultFboTextureFilter);
            depth->setWrap(Settings::demo.graphics.defaultFboTextureWrap);
            depth->setFormat(TextureFormat::DEPTH_COMPONENT);

            if (depth->create(getWidth(), getHeight()) == false) {
                loggerError("Could not create FBO depth texture. name:'%s'", getName().c_str());
                return false;
            }
        }
    }
    
    glGenFramebuffers(1, &id);
    if (id == 0) {
        Graphics &graphics = Graphics::getInstance();
        graphics.handleErrors();
        loggerError("Could not create FBO framebuffer. name:'%s'", getName().c_str());
        return false;
    }

    bind();

    if (color) {
        //depth buffer is needed so that depth testing will work with 3D objects
        if (depthBuffer == 0) {
            glGenRenderbuffers(1, &depthBuffer);
            if (depthBuffer == 0) {
                Graphics &graphics = Graphics::getInstance();
                graphics.handleErrors();
                loggerError("Could not create FBO render buffer. name:'%s'", getName().c_str());
                unbind();
                return false;
            }

            glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, getWidth(), getHeight());
            glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
        }

        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dynamic_cast<TextureOpenGl*>(color)->getId(), 0);
    } else {
        // if no color texture, then remove color drawing for potential speed improvements
        glDrawBuffer(GL_NONE);
    }

    if (depth) {
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dynamic_cast<TextureOpenGl*>(depth)->getId(), 0);
    }

    GLenum e = checkFramebufferStatus();

    unbind();

    if (e != GL_FRAMEBUFFER_COMPLETE) {
        return false;
    }

    loggerInfo("Created FBO. name:'%s' id:%u dimensions:%ux%u, colorTexture:0x%p, depthTexture:0x%p",
        getName().c_str(), id, getWidth(), getHeight(), color, depth);

    return true;
}

void FboOpenGl::free() {
    PROFILER_BLOCK("FboOpenGl::free");

    if (color != NULL) {
        color->free();
        delete color;
        color = NULL;
    }

    if (depth != NULL) {
        depth->free();
        delete depth;
        depth = NULL;
    }

    if (depthBuffer != 0) {
        glDeleteRenderbuffers(1, &depthBuffer);
        depthBuffer = 0;
    }

    if (id != 0) {
        glDeleteFramebuffers(1, &id);
        id = 0;
        loggerDebug("Freed FBO. name:'%s'", getName().c_str());
    }

    Graphics &graphics = Graphics::getInstance();
    if (graphics.handleErrors()) {
        loggerError("Could not cleanly free FBO. name:'%s' id:%u, depthBuffer:%u, dimensions:%ux%u, colorTexture:0x%p, depthTexture:0x%p",
            getName().c_str(), id, depthBuffer, getWidth(), getHeight(), color, depth);
    }
}

void FboOpenGl::bind() {
    PROFILER_BLOCK("FboOpenGl::bind");

    bindStack.push_back(this);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, id);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
}

void FboOpenGl::unbind() {
    PROFILER_BLOCK("FboOpenGl::unbind");

    if (bindStack.empty()) {
        loggerDebug("Can't unbind FBO, stack empty. name:'%s' id:%u, depthBuffer:%u, dimensions:%ux%u, colorTexture:0x%p, depthTexture:0x%p",
            getName().c_str(), id, depthBuffer, getWidth(), getHeight(), color, depth);
        return;
    }

    textureBind();
    if (color) {
        color->processFilterProperties();
    }
    if (depth) {
        depth->processFilterProperties();
    }
    textureUnbind();

    bindStack.pop_back();

    GLuint parentId = 0;
    GLuint parentDepthBufferId = 0;
    if (! bindStack.empty()) {
        parentId = bindStack.back()->getId();
        parentDepthBufferId = bindStack.back()->getDepthBufferId();
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, parentId);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, parentId);
    glBindRenderbuffer(GL_RENDERBUFFER, parentDepthBufferId);
}

void FboOpenGl::setDimensions(unsigned int width, unsigned int height) {
    this->width = width;
    this->height = height;
}

unsigned int FboOpenGl::getWidth() {
    return width;
}

unsigned int FboOpenGl::getHeight() {
    return height;
}

void FboOpenGl::start() {
    PROFILER_BLOCK("FboOpenGl::start");

    bind();

    Graphics& graphics = Graphics::getInstance();
    graphics.setViewport(0, 0, getWidth(), getHeight());
    graphics.clear();
}

void FboOpenGl::end() {
    PROFILER_BLOCK("FboOpenGl::end");

    unbind();

    Graphics& graphics = Graphics::getInstance();
    graphics.setViewport();
    graphics.clear();
}

void FboOpenGl::textureBind() {
    PROFILER_BLOCK("FboOpenGl::textureBind");

    if (depth) {
        depth->bind(depthTextureUnit);
    }

    if (color) {
        color->bind(colorTextureUnit);
    }
}

void FboOpenGl::textureUnbind() {
    PROFILER_BLOCK("FboOpenGl::textureUnbind");

    if (depth) {
        depth->unbind(depthTextureUnit);
    }

    if (color) {
        color->unbind(colorTextureUnit);
    }
}

Texture *FboOpenGl::getColorTexture() {
    return color;
}

Texture *FboOpenGl::getDepthTexture() {
    return depth;
}
