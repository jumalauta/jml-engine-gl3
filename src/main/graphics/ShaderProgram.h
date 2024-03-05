#ifndef ENGINE_GRAPHICS_SHADERPROGRAM_H_
#define ENGINE_GRAPHICS_SHADERPROGRAM_H_

#include "io/ManagedMemoryObject.h"
#include <string>
#include <functional>
#include <array>

class Shader;

class ShaderProgram : public ManagedMemoryObject {
public:
    static ShaderProgram *newInstance(std::string name);
    static void useCurrentBind();
    virtual ~ShaderProgram() {};
    virtual bool addShader(Shader *shader) = 0;
    virtual bool link() = 0;
    virtual bool isLinked() = 0;
    virtual void free() = 0;
    virtual void bind() = 0;
    virtual void unbind() = 0;
    virtual bool containsUniform(std::string uniformKey) = 0;
    const std::string &getName();
protected:
    explicit ShaderProgram(std::string name);
    virtual bool generate() = 0;
    virtual bool attach() = 0;
    virtual bool detach() = 0;
private:
    std::string name;
};

#endif /*ENGINE_GRAPHICS_SHADERPROGRAM_H_*/
