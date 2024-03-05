#include "GraphicsOpenGl.h"

#include <stdio.h>

#include <string>

#include "GL/gl3w.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "logger/logger.h"
#include "graphics/Image.h"
#include "ui/Window.h"
#include "time/SystemTime.h"

#include "Settings.h"

OpenGlState::OpenGlState() {
    saved = false;
    currentProgram = 0;
    textureBinding2d = 0;
    activeTexture = 0;
    drawFramebufferBinding = 0;
    readFramebufferBinding = 0;
    renderbufferBinding = 0;
    arrayBufferBinding = 0;
    elementArrayBufferBinding = 0;
    vertexArrayBinding = 0;
    blendSrc = 0;
    blendDst = 0;
    blendEquationRgb = 0;
    blendEquationAlpha = 0;
    viewport[0] = viewport[1] = viewport[2] = viewport[3] = 0;
    blend = 0;
    cullFace = 0;
    depthTest = 0;
    scissorTest = 0;
}

void OpenGlState::save() {
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &textureBinding2d);
    glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &drawFramebufferBinding);
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &readFramebufferBinding);
    glGetIntegerv(GL_RENDERBUFFER_BINDING, &renderbufferBinding);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &arrayBufferBinding);
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &elementArrayBufferBinding);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &vertexArrayBinding);
    glGetIntegerv(GL_BLEND_SRC, &blendSrc);
    glGetIntegerv(GL_BLEND_DST, &blendDst);
    glGetIntegerv(GL_BLEND_EQUATION_RGB, &blendEquationRgb);
    glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &blendEquationAlpha);
    glGetIntegerv(GL_VIEWPORT, viewport);
    blend = glIsEnabled(GL_BLEND);
    cullFace = glIsEnabled(GL_CULL_FACE);
    depthTest = glIsEnabled(GL_DEPTH_TEST);
    scissorTest = glIsEnabled(GL_SCISSOR_TEST);
    saved = true;
}

void OpenGlState::load() {
    if (!saved) {
        loggerWarning("State has not been saved yet, can't load");
        return;
    }

    glUseProgram(currentProgram);
    glActiveTexture(activeTexture);
    glBindTexture(GL_TEXTURE_2D, textureBinding2d);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFramebufferBinding);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, readFramebufferBinding);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbufferBinding);
    glBindVertexArray(vertexArrayBinding);
    glBindBuffer(GL_ARRAY_BUFFER, arrayBufferBinding);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArrayBufferBinding);
    glBlendEquationSeparate(blendEquationRgb, blendEquationAlpha);
    glBlendFunc(blendSrc, blendDst);
    if (blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
    if (cullFace) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
    if (depthTest) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
    if (scissorTest) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
    glViewport(viewport[0], viewport[1], (GLsizei)viewport[2], (GLsizei)viewport[3]);
}

void OpenGlState::print() {
    if (!saved) {
        loggerWarning("State has not been saved yet, can't print");
        return;
    }

    loggerDebug("currentProgram: %d", currentProgram);
    loggerDebug("textureBinding2d: %d", textureBinding2d);
    loggerDebug("activeTexture: %d", activeTexture);
    loggerDebug("drawFramebufferBinding: %d", drawFramebufferBinding);
    loggerDebug("readFramebufferBinding: %d", readFramebufferBinding);
    loggerDebug("renderbufferBinding: %d", renderbufferBinding);
    loggerDebug("arrayBufferBinding: %d", arrayBufferBinding);
    loggerDebug("elementArrayBufferBinding: %d", elementArrayBufferBinding);
    loggerDebug("vertexArrayBinding: %d", vertexArrayBinding);
    loggerDebug("blendSrc: %d", blendSrc);
    loggerDebug("blendDst: %d", blendDst);
    loggerDebug("blendEquationRgb: %d", blendEquationRgb);
    loggerDebug("blendEquationAlpha: %d", blendEquationAlpha);
    loggerDebug("viewport: x:%d, y:%d, w:%d, h:%d", viewport[0], viewport[1], viewport[2], viewport[3]);
    loggerDebug("blend: %d", blend);
    loggerDebug("cullFace: %d", cullFace);
    loggerDebug("depthTest: %d", depthTest);
    loggerDebug("scissorTest: %d", scissorTest);
}

std::vector<OpenGlState> GraphicsOpenGl::stateStack = {};

void GraphicsOpenGl::pushState() {
    OpenGlState state;
    state.save();
    stateStack.push_back(state);
}

void GraphicsOpenGl::popState() {
    if (stateStack.empty()) {
        loggerWarning("Attempting to pop empty stack");
        return;
    }

    stateStack.back().load();
    stateStack.pop_back();
}


Graphics& Graphics::getInstance() {
    static GraphicsOpenGl graphics = GraphicsOpenGl();
    return graphics;
}

GraphicsOpenGl::GraphicsOpenGl() {
    libraryLoaded = false;
    initialized = false;
}

GraphicsOpenGl::~GraphicsOpenGl() {
    if (!stateStack.empty()) {
        loggerWarning("Non-empty state stack during destructor. size:%u", stateStack.size());
    }
}

static const char* getOpenGlSource(GLenum source) {
    switch(source) {
        case GL_DEBUG_SOURCE_API:
            return "API";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            return "WINDOW_SYSTEM";
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            return "SHADER_COMPILER";
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            return "THIRD_PARTY";
        case GL_DEBUG_SOURCE_APPLICATION:
            return "APPLICATION";
        case GL_DEBUG_SOURCE_OTHER:
            return "OTHER";
        default:
            return "OTHER!"; 
    }
}

static const char* getOpenGlType(GLenum type) {
    switch(type) {
        case GL_DEBUG_TYPE_ERROR:
            return "ERROR";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            return "DEPRECATED_BEHAVIOR";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            return "UNDEFINED_BEHAVIOR";
        case GL_DEBUG_TYPE_PORTABILITY:
            return "PORTABILITY";
        case GL_DEBUG_TYPE_PERFORMANCE:
            return "PERFORMANCE";
        case GL_DEBUG_TYPE_OTHER:
            return "OTHER";
        case GL_DEBUG_TYPE_MARKER:
            return "MARKER";
        case GL_DEBUG_TYPE_PUSH_GROUP:
            return "GROUP";
        case GL_DEBUG_TYPE_POP_GROUP:
            return "GROUP";
        default:
            return "OTHER!"; 
    }
}

static const char* getOpenGlSeverity(GLenum severity) {
    switch(severity) {
        case GL_DEBUG_SEVERITY_LOW:
            return "LOW";
        case GL_DEBUG_SEVERITY_MEDIUM:
            return "MEDIUM";
        case GL_DEBUG_SEVERITY_HIGH:
            return "HIGH";
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            return "NOTIFICATION";
        default:
            return "OTHER!"; 
    }
}

static void openGlDebugLogProcess(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    loggerDebug("OpenGL LOG %s/%s/%s (%u): %s", getOpenGlSource(source), getOpenGlType(type), getOpenGlSeverity(severity), id, message);
}

bool GraphicsOpenGl::init() {
    if (!libraryLoaded) {
        int ret = gl3wInit();
        if (ret < 0) {
            if (ret == GL3W_ERROR_OPENGL_VERSION && glGetString) {
                loggerWarning("Invalid OpenGL version: %s", glGetString(GL_VERSION));
            }

            loggerFatal("Failed to initialize OpenGL: %d", ret);
            return false;
        }

        loggerDebug("OpenGL: %s, GLSL: %s, Renderer: %s, Vendor: %s",
            glGetString(GL_VERSION),
            glGetString(GL_SHADING_LANGUAGE_VERSION),
            glGetString(GL_RENDERER),
            glGetString(GL_VENDOR));

        if (Settings::logger.logLevel <= LEVEL_TRACE) {
            // in theory this is supported only in OpenGL 4.3 and upwards
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
            glDebugMessageCallback(openGlDebugLogProcess, NULL);

            //full-blown debug in OpenGL
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
        }

        GLint glMaxTextureSize = 0;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glMaxTextureSize);
        loggerTrace("OpenGL variables: GL_MAX_TEXTURE_SIZE:%d", glMaxTextureSize);

        int glMajorVersion = Settings::demo.graphics.requestedMajorVersion;
        int glMinorVersion = Settings::demo.graphics.requestedMinorVersion;

        if (!gl3wIsSupported(glMajorVersion, glMinorVersion)) {
            loggerError("OpenGL %d.%d not supported", glMajorVersion, glMinorVersion);
            return false;
        }

        libraryLoaded = true;
    }

    setup();

    initialized = true;

    return true;
}

bool GraphicsOpenGl::setup() {
    setCapability(GL_SCISSOR_TEST, true);
    setCapability(GL_BLEND, true);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    setDepthTest(true);
    glDepthFunc(GL_LEQUAL);
    // TODO: quality options should be separated, let's in the meanwhile assume that we want the nicest quality
    setCapability(GL_LINE_SMOOTH, true);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST); // or GL_FASTEST
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    glFrontFace(GL_CCW); // explicitly defined, used at least by TexturedQuad

    return true;
}

bool GraphicsOpenGl::exit() {
    initialized = false;
    // NOP
    return true;
}

void GraphicsOpenGl::setViewport() {
    setViewport(Settings::window.canvasPositionX,
                Settings::window.canvasPositionY,
                Settings::window.screenAreaWidth,
                Settings::window.screenAreaHeight);
}

void GraphicsOpenGl::setViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
    glViewport(x, y, width, height);

    glScissor(x, y, width, height);
}

void GraphicsOpenGl::setClearColor(Color color) {
    if (libraryLoaded) {
        glClearColor(color.r, color.g, color.b, color.a);
    }
}

void GraphicsOpenGl::setColor(Color color) {
    this->color = color;
}

Color& GraphicsOpenGl::getColor() {
    return color;
}

void GraphicsOpenGl::clear() {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
}

bool GraphicsOpenGl::handleErrors() {
    bool errorOccurred = false;
    if (checkError()) {
        errorOccurred = true;
    }

    return errorOccurred;
}

bool GraphicsOpenGl::checkError() {
    if (!initialized) {
        loggerTrace("OpenGL errors not checked, graphics not initialized");
        return false;
    }

    GLenum errorCode = glGetError();
    if (errorCode != GL_NO_ERROR) {
        std::string errorString;

        switch(errorCode) {
            case GL_NO_ERROR:
                errorString = "GL_NO_ERROR";
                break;
            case GL_INVALID_ENUM:
                errorString = "GL_INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                errorString = "GL_INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                errorString = "GL_INVALID_OPERATION";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                errorString = "GL_INVALID_FRAMEBUFFER_OPERATION";
                break;
            case GL_OUT_OF_MEMORY:
                errorString = "GL_OUT_OF_MEMORY";
                break;
            case GL_STACK_UNDERFLOW:
                errorString = "GL_STACK_UNDERFLOW";
                break;
            case GL_STACK_OVERFLOW:
                errorString = "GL_STACK_OVERFLOW";
                break;
            default:
                errorString = std::to_string(errorCode);
                break;
        }

        loggerError("OpenGL Error: %s", errorString.c_str());
        static bool showVersionInfo = true;
        if (showVersionInfo) {
            loggerError("OpenGL: %s, GLSL: %s, Renderer: %s, Vendor: %s",
                glGetString(GL_VERSION),
                glGetString(GL_SHADING_LANGUAGE_VERSION),
                glGetString(GL_RENDERER),
                glGetString(GL_VENDOR));

            showVersionInfo = false;
        }

        return true;
    }

    return false;
}

bool GraphicsOpenGl::takeScreenshot(Window &window) {
    std::string file = std::string("screenshot_") + std::to_string(SystemTime::getTimeInMillis()) + std::string(".png");
    Image *image = Image::newInstance(file);

    const int width = window.getWidth();
    const int height = window.getHeight();

    const GLenum format = GL_RGB;
    const int channels = 3;

    unsigned char *data = new unsigned char[(width * height + width) * channels];

    glReadPixels(0, 0, width, height, format, GL_UNSIGNED_BYTE, static_cast<GLvoid*>(data));

    bool success = Image::write(*image, width, height, channels, static_cast<const void *>(data));

    delete [] data;

    delete image;

    return success;
}

void GraphicsOpenGl::setDepthTest(bool enable) {
    setCapability(GL_DEPTH_TEST, enable);
}

void GraphicsOpenGl::setCapability(GLenum capability, bool enable) {
    if (enable) {
        glEnable(capability);
    } else {
        glDisable(capability);
    }
}
