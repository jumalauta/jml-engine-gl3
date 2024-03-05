#include "ShaderVariable.h"

ShaderVariable::ShaderVariable(ShaderProgram *shaderProgram, std::string name) {
    this->shaderProgram = shaderProgram;
    this->name = name;
}

void ShaderVariable::setValueReference(VariableType type, void *variable) {
    this->type = type;
    variablePointer = variable;
}
