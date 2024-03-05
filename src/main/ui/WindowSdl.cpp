#include "WindowSdl.h"

#include <stdio.h>

#include "Settings.h"
#include "logger/logger.h"
#include "graphics/Graphics.h"

#include "GL/gl3w.h"

static int windows = 0;
Window* Window::newInstance() {
    return new WindowSdl();
}

void Window::showMessageBox(int level, const char *title, const char *message) {
    if (!Settings::logger.showMessageBox) {
        return;
    }

    Uint32 flags = SDL_MESSAGEBOX_INFORMATION;
    switch(level) {
        case LEVEL_ERROR:
        case LEVEL_FATAL:
            flags = SDL_MESSAGEBOX_ERROR;
            break;
        case LEVEL_WARNING:
            flags = SDL_MESSAGEBOX_WARNING;
            break;
        default:
            flags = SDL_MESSAGEBOX_INFORMATION;
            break;
    }

    if (SDL_ShowSimpleMessageBox(flags, title, message, NULL) < 0) {
        loggerWarning("Could not show message box, error: %s", SDL_GetError());
    }
}

SDL_GLContext WindowSdl::graphicsContext = NULL;

WindowSdl::WindowSdl() {
    window = NULL;
    fullscreen = false;
    focus = false;
}

bool WindowSdl::init() {
    return true;
}

bool WindowSdl::open() {
    windows++;

    setOpenGlAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    setOpenGlAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, Settings::demo.graphics.requestedMajorVersion);
    setOpenGlAttribute(SDL_GL_CONTEXT_MINOR_VERSION, Settings::demo.graphics.requestedMinorVersion);
    //setOpenGlAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, windows > 1 ? 1 : 0);

    // Settings
    setOpenGlAttribute(SDL_GL_RED_SIZE, 8);
    setOpenGlAttribute(SDL_GL_GREEN_SIZE, 8);
    setOpenGlAttribute(SDL_GL_BLUE_SIZE, 8);
    setOpenGlAttribute(SDL_GL_ALPHA_SIZE, 8);
    setOpenGlAttribute(SDL_GL_DEPTH_SIZE, 32);
    setOpenGlAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    setOpenGlAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    setOpenGlAttribute(SDL_GL_ACCELERATED_VISUAL, 1); 

    Uint32 windowFlags = SDL_WINDOW_ALLOW_HIGHDPI|SDL_WINDOW_OPENGL;
    if (Settings::gui.tool) {
        windowFlags |= SDL_WINDOW_RESIZABLE;
    }

    window = SDL_CreateWindow(getTitle().c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        getWidth(), getHeight(),
        windowFlags);

    if (window == NULL) {
        loggerFatal("Window could not be created. error: %s", SDL_GetError());
        return false;
    }

    setFullscreen(fullscreen);

    setCursor(! fullscreen);

    if (!bindGraphicsContext()) {
        return false;
    }

    if (! Graphics::getInstance().init()) {
        loggerFatal("Failed to initialize graphics");
        return false;
    }

    return true;
}

bool WindowSdl::bindGraphicsContext() {
    if (window == NULL) {
        loggerError("Window not created");
        return false;
    }

    if (graphicsContext == NULL) {
        graphicsContext = SDL_GL_CreateContext(window);
        if (graphicsContext == NULL) {
            loggerFatal("Window OpenGL context could not be created. error: %s", SDL_GetError());
            return false;
        }

        if (Settings::window.verticalSync) {
            setVerticalSync(Settings::window.verticalSync);
        }
    }

    int ret = SDL_GL_MakeCurrent(window, graphicsContext);
    if (ret < 0) {
        loggerFatal("Could not make OpenGL context current, error: %s", SDL_GetError());
        return false;
    }

    return true;
}

void* WindowSdl::getGraphicsContext() {
    return static_cast<void*>(graphicsContext);
}

void WindowSdl::setCursor(bool cursor) {
    int toggleCursor = SDL_DISABLE;
    if (cursor) {
        toggleCursor = SDL_ENABLE;
    }

    if (Settings::gui.editor) {
        // In editor mode cursor is always enabled, as it's needed for editing...
        toggleCursor = SDL_ENABLE;
    }

    SDL_ShowCursor(toggleCursor);
}

bool WindowSdl::setFullscreen(bool fullscreen) {
    this->fullscreen = fullscreen;

    if (window == NULL) {
        return true;
    }

    Uint32 fullscreenFlags = 0; // disabled

    if (fullscreen) {
        fullscreenFlags = SDL_WINDOW_FULLSCREEN_DESKTOP;
    }

    int ret = SDL_SetWindowFullscreen(window, fullscreenFlags);
    if (ret < 0) {
        loggerWarning("Could not setup fullscreen=%s, error: %s", fullscreen ? "true" : "false", SDL_GetError());

        if (fullscreen) {
            // Fallback to "real" fullscreen
            fullscreenFlags = SDL_WINDOW_FULLSCREEN;
            ret = SDL_SetWindowFullscreen(window, fullscreenFlags);
            if (ret < 0) {
                loggerError("Could not setup fallback fullscreen=%s, error: %s", fullscreen ? "true" : "false", SDL_GetError());
                return false;
            }
        } else {
            return false;
        }
    }

    // amend window dimensions in case DPI alters assumpted dimensions
    resize();

    return true;
}

bool WindowSdl::getFullscreen() {
    return fullscreen;
}

bool WindowSdl::setTitle(std::string title, std::string extraInfo) {
    bool changed = Window::setTitle(title, extraInfo);
    if (changed && window) {
        SDL_SetWindowTitle(window, std::string(getTitle() + extraInfo).c_str());
    }

    return changed;
}

void WindowSdl::swapBuffers() {
    //On Mac OS X make sure you bind 0 to the draw framebuffer before swapping the window, otherwise nothing will happen.
    //ref: http://renderingpipeline.com/2012/05/nsopenglcontext-flushbuffer-might-not-do-what-you-think/
    //
    //In theory, the engine should internally work so that it tries to bind everything to engine's default values after use
    //but this behaviour may, for example, leak from user defined stuff
    //

    GLint drawFramebufferBinding = 0;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &drawFramebufferBinding);
    if (drawFramebufferBinding != 0) {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }

    glFinish();
    SDL_GL_SwapWindow(window);

    if (drawFramebufferBinding != 0) {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFramebufferBinding);
    }
}

bool WindowSdl::close() {
    windows--;

    if (graphicsContext && windows <= 0) {
        SDL_GL_DeleteContext(graphicsContext);
        graphicsContext = NULL;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = NULL;
    }
    return true;
}

bool WindowSdl::exit() {
    return close();
}

void WindowSdl::setOpenGlAttribute(SDL_GLattr attr, int value) {
    int ret = SDL_GL_SetAttribute(attr, value);
    if (ret < 0) {
        loggerWarning("Could set SDL OpenGL attribute. attribute:%d, value:%d, error(%d): %s", attr, value, ret, SDL_GetError());
    }
}

bool WindowSdl::setVerticalSync(bool vsync) {
    const int DISABLE_VSYNC = 0;
    const int ENABLE_VSYNC_WITH_LATE_SWAP_TEARING = -1;
    const int ENABLE_VSYNC = 1;
    
    int ret;
    if (vsync) {
        //Late swap tearing works the same as vsync, but if you've already missed the vertical retrace for a given frame,
        //it swaps buffers immediately, which might be less jarring for the user during occasional framerate drops.
        ret = SDL_GL_SetSwapInterval(ENABLE_VSYNC_WITH_LATE_SWAP_TEARING);
        if (ret == -1) {
            loggerDebug("Could not enable VSYNC with late swap tearing, attempting to fallback to normal VSYNC. error: %s", SDL_GetError());

            //fall-back to normal vsync
            ret = SDL_GL_SetSwapInterval(ENABLE_VSYNC);
        }
    } else {
        ret = SDL_GL_SetSwapInterval(DISABLE_VSYNC);
    }

    if (ret == -1) {
        loggerWarning("Could not '%s' VSYNC. error: %s", vsync ? "enable" : "disable", SDL_GetError());
        return false;
    }

    return true;
}

void WindowSdl::resize() {
    int width, height;
    // This may differ from SDL_GetWindowSize() if we're rendering to a high-DPI drawable
    SDL_GL_GetDrawableSize(window, &width, &height);
    if (Settings::window.width != static_cast<unsigned int>(width) || Settings::window.height != static_cast<unsigned int>(height)) {
        loggerInfo("Window resized to %dx%d!", width, height);
    }

    Settings::window.setWindowDimensions(width, height);
}

void WindowSdl::setFocus(bool focus) {
    this->focus = focus;
}

bool WindowSdl::getFocus() {
    return focus;
}

void WindowSdl::show(bool show) {
    if (window == NULL) {
        loggerError("Window not created");
        return;
    }

    if (show) {
        SDL_ShowWindow(window);
        SDL_RaiseWindow(window);
        setFocus(true);
    } else {
        setFocus(false);
        SDL_HideWindow(window);
    }
}
