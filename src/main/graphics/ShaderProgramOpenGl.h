#ifndef ENGINE_GRAPHICS_SHADERPROGRAMOPENGL_H_
#define ENGINE_GRAPHICS_SHADERPROGRAMOPENGL_H_

#include "ShaderProgram.h"

#include "GL/gl3w.h"

#include <vector>
#include <map>
#include <string>

class ShaderOpenGl;

class ShaderProgramOpenGl : public ShaderProgram {
public:
    explicit ShaderProgramOpenGl(std::string name);
    ~ShaderProgramOpenGl();
    bool addShader(Shader *shader);
    bool link();
    bool isLinked();
    void free();
    void bind();
    void unbind();
    GLuint getId();
    bool containsUniform(std::string uniformKey);
    static GLint getUniformLocation(const char* variable);
    static void useCurrentBind();
protected:
    bool generate();
    bool attach();
    bool detach();
    void determineUniforms();
    void assignUniforms();
    bool setUniformFunction1f(std::string uniformName, std::function<float()> function);
    bool setUniformFunction2fv(std::string uniformName, std::function<std::array<float, 2>()> function);
    bool setUniformFunction3fv(std::string uniformName, std::function<std::array<float, 3>()> function);
    bool setUniformFunction4fv(std::string uniformName, std::function<std::array<float, 4>()> function);
    bool setUniformFunction1i(std::string uniformName, std::function<int()> function);
    bool setUniformFunction(std::string uniformName, GLint uniformId, std::function<void()> function);
private:
    bool checkLinkStatus();
    bool linked;
    GLuint id;
    std::map<std::string, std::function<void()>> uniforms;
    static GLuint getCurrentBindId();
    static std::vector<ShaderProgramOpenGl*> bindStack;
    std::vector<ShaderOpenGl*> shaders;
    static ShaderProgramOpenGl* shaderProgramDefault;
    static ShaderProgramOpenGl* shaderProgramDefaultShadow;
};

#endif /*ENGINE_GRAPHICS_SHADERPROGRAMOPENGL_H_*/
