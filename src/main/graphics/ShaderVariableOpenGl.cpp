#include "ShaderVariableOpenGl.h"

#include "logger/logger.h"
#include "graphics/ShaderProgram.h"
#include "graphics/ShaderProgramOpenGl.h"
#include "math/TransformationMatrix.h"

ShaderVariable *ShaderVariable::newInstance(ShaderProgram *shaderProgram, std::string name) {
    return new ShaderVariableOpenGl(shaderProgram, name);
}

ShaderVariableOpenGl::ShaderVariableOpenGl(ShaderProgram *shaderProgram, std::string name) : ShaderVariable(shaderProgram, name) {
    uniformId = -1;
}

bool ShaderVariableOpenGl::init() {
    uniformId = glGetUniformLocation(dynamic_cast<ShaderProgramOpenGl*>(shaderProgram)->getId(), name.c_str());
    if (uniformId == -1) {
        loggerWarning("Could not determine uniform. name:'%s', program:'%s'", name.c_str(), shaderProgram->getName().c_str());
        return false;
    }

    return true;
}

void ShaderVariableOpenGl::set() {
    if (uniformId == -1) {
        loggerWarning("Could not determine uniform. name:'%s', program:'%s'", name.c_str(), shaderProgram->getName().c_str());
        return;
    }

    switch(type) {
        case VariableType::INT:
            glUniform1i(uniformId, * static_cast<GLint*>(variablePointer));
            return;
        case VariableType::INT2:
            glUniform2iv(uniformId, 1, static_cast<GLint*>(variablePointer));
            return;
        case VariableType::INT3:
            glUniform3iv(uniformId, 1, static_cast<GLint*>(variablePointer));
            return;
        case VariableType::INT4:
            glUniform4iv(uniformId, 1, static_cast<GLint*>(variablePointer));
            return;
        case VariableType::FLOAT:
            glUniform1f(uniformId, * static_cast<GLfloat*>(variablePointer));
            return;
        case VariableType::FLOAT2:
            glUniform2fv(uniformId, 1, static_cast<GLfloat*>(variablePointer));
            return;
        case VariableType::FLOAT3:
            glUniform3fv(uniformId, 1, static_cast<GLfloat*>(variablePointer));
            return;
        case VariableType::FLOAT4:
            glUniform4fv(uniformId, 1, static_cast<GLfloat*>(variablePointer));
            return;
        case VariableType::TRANSFORMATION_MATRIX:
            glUniformMatrix4fv(uniformId, 1, GL_FALSE, static_cast<TransformationMatrix*>(variablePointer)->getMvp());
            return;
        default:
            loggerWarning("Variable type %d unknown. name:'%s', program:'%s'", type, name.c_str(), shaderProgram->getName().c_str());
            return;
    }
}
