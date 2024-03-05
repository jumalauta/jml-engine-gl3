#ifndef ENGINE_GRAPHICS_SHADERVARIABLE_H_
#define ENGINE_GRAPHICS_SHADERVARIABLE_H_

#include <string>

class ShaderProgram;

enum class VariableType {
    INT,
    INT2,
    INT3,
    INT4,
    FLOAT,
    FLOAT2,
    FLOAT3,
    FLOAT4,
    TRANSFORMATION_MATRIX
};

class ShaderVariable {
public:
    static ShaderVariable *newInstance(ShaderProgram *shaderProgram, std::string name);

    void setValueReference(VariableType type, void *variable);

    virtual bool init() = 0;
    virtual void set() = 0;

protected:
    ShaderVariable(ShaderProgram *shaderProgram, std::string name);
    ShaderProgram *shaderProgram;
    std::string name;
    VariableType type;
    void *variablePointer;
};

#endif /*ENGINE_GRAPHICS_SHADERVARIABLE_H_*/
