#ifndef ENGINE_GRAPHICS_SHADERVARIABLEOPENGL_H_
#define ENGINE_GRAPHICS_SHADERVARIABLEOPENGL_H_

#include "ShaderVariable.h"

#include "GL/gl3w.h"

class ShaderVariableOpenGl : public ShaderVariable {
public:
    ShaderVariableOpenGl(ShaderProgram *shaderProgram, std::string name);

    bool init();
    void set();
private:
    GLint uniformId;
};

#endif /*ENGINE_GRAPHICS_SHADERVARIABLEOPENGL_H_*/
