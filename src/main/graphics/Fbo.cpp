#include "Fbo.h"

Fbo::Fbo(std::string name) {
    this->name = name;
}

Fbo::~Fbo() {

}

const std::string &Fbo::getName() {
    return name;
}
