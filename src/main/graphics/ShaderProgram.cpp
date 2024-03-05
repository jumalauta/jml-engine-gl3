#include "ShaderProgram.h"

ShaderProgram::ShaderProgram(std::string name) {
    this->name = name;
}

const std::string &ShaderProgram::getName() {
    return name;
}
