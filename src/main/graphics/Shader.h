#ifndef ENGINE_GRAPHICS_SHADER_H_
#define ENGINE_GRAPHICS_SHADER_H_

#include "io/File.h"

#include <string>

class ShaderProgram;

class Shader : public File {
public:
    virtual ~Shader() {}
    virtual bool load(bool rollback=false) = 0;
    virtual bool isSupported() = 0;
    virtual void free() = 0;
    static Shader* newInstance(std::string filePath);
    virtual bool addShaderProgram(ShaderProgram *shaderProgram) = 0;
    virtual bool removeShaderProgram(ShaderProgram *shaderProgram) = 0;

protected:
    Shader();
    explicit Shader(std::string filePath);
    virtual bool generate() = 0;
};

#endif /*ENGINE_GRAPHICS_SHADER_H_*/
