#ifndef ENGINE_GRAPHICS_GRAPHICS_OPENGL_H_
#define ENGINE_GRAPHICS_GRAPHICS_OPENGL_H_

#include "Graphics.h"
#include "GL/gl3w.h"

#include <vector>

struct OpenGlState {
    bool saved;
    GLint currentProgram; 
    GLint textureBinding2d; 
    GLint activeTexture; 
    GLint drawFramebufferBinding;
    GLint readFramebufferBinding;
    GLint renderbufferBinding;
    GLint arrayBufferBinding; 
    GLint elementArrayBufferBinding; 
    GLint vertexArrayBinding; 
    GLint blendSrc; 
    GLint blendDst; 
    GLint blendEquationRgb; 
    GLint blendEquationAlpha; 
    GLint viewport[4]; 
    GLboolean blend;
    GLboolean cullFace;
    GLboolean depthTest;
    GLboolean scissorTest;

    OpenGlState();

    void save();
    void load();
    void print();
};

class GraphicsOpenGl : public Graphics {
public:
    GraphicsOpenGl();
    ~GraphicsOpenGl();
    void pushState();
    void popState();
    bool init();
    bool exit();
    void setViewport();
    void setViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
    void setClearColor(Color color);
    void setColor(Color color);
    Color& getColor();
    void clear();
    bool handleErrors();
    bool takeScreenshot(Window &window);
    void setDepthTest(bool enable);
protected:
    bool setup();
private:
    bool checkError();
    static void setCapability(GLenum capability, bool enable);
    static std::vector<OpenGlState> stateStack;
    Color color;
    bool libraryLoaded;
    bool initialized;
};


#endif /*ENGINE_GRAPHICS_GRAPHICS_OPENGL_H_*/
