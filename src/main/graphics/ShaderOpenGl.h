#ifndef ENGINE_GRAPHICS_SHADEROPENGL_H_
#define ENGINE_GRAPHICS_SHADEROPENGL_H_

#include "Shader.h"

#include "GL/gl3w.h"

#include <vector>

class ShaderProgramOpenGl;

class ShaderOpenGl : public Shader {
public:
    explicit ShaderOpenGl(std::string filePath);
    ~ShaderOpenGl();
    bool load(bool rollback=false);
    bool isLoaded();
    bool isSupported();
    void free();
    bool addShaderProgram(ShaderProgram *shaderProgram);
    bool removeShaderProgram(ShaderProgram *shaderProgram);
    GLuint getId();
    GLenum determineShaderType();
protected:
    bool isShadertoyShader();
    void makeShadertoyBootstrap();
    bool generate();
    bool validate();
private:
    bool checkCompileStatus();
    GLuint id;
    std::vector<ShaderProgramOpenGl*> shaderPrograms;
};

#endif /*ENGINE_GRAPHICS_SHADEROPENGL_H_*/
