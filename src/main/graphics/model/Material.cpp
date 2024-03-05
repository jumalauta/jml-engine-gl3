#include "Material.h"

#include "Settings.h"
#include "logger/logger.h"
#include "graphics/Texture.h"
#include "graphics/ShaderProgram.h"

#include "EnginePlayer.h"

#include <cstddef>
#include <sstream>


Material* Material::currentMaterial = NULL;

Material* Material::getCurrentMaterial() {
    return currentMaterial;
}

Material::Material() {
    shaderProgram = NULL;
    setShininess(10.0);
    name = "N/A";
}

Material::~Material() {
}

std::string Material::toString() const {
    std::stringstream ss;
    ss << "name: " << name;
    ss << ", ambient: r:" << ambient.r << ", g:" << ambient.g << ", b:" << ambient.b << ", a:" << ambient.a;
    ss << ", diffuse: r:" << diffuse.r << ", g:" << diffuse.g << ", b:" << diffuse.b << ", a:" << diffuse.a;
    ss << ", specular: r:" << specular.r << ", g:" << specular.g << ", b:" << specular.b << ", a:" << specular.a;
    ss << ", shininess: " << shininess;
    for (auto it : textureUnits) {
        Texture *texture = it.second;
        if (texture != NULL) {
            ss << ", texture" << it.first << ":" << static_cast<void*>(texture);
        }
    }

    return ss.str();
}

void Material::bind() {
    currentMaterial = this;

    for (auto it : textureUnits) {
        Texture *texture = it.second;
        if (texture != NULL) {
            texture->bind(it.first);
        }
    }

    if (shaderProgram) {
        shaderProgram->bind();
    } else {
        ShaderProgram::useCurrentBind();
    }
}

void Material::unbind() {
    currentMaterial = NULL;

    if (shaderProgram) {
        shaderProgram->unbind();
    }

    for (auto it : textureUnits) {
        Texture *texture = it.second;
        if (texture != NULL) {
            texture->unbind(it.first);
        }
    }
}

void Material::setTexture(Texture *texture, unsigned int unit) {
    if (unit >= Settings::demo.graphics.maxTextureUnits) {
        loggerError("Cannot set texture to unit %u, maxTextureUnits:%u", unit, Settings::demo.graphics.maxTextureUnits);
        return;
    }

    textureUnits[unit] = texture;
}

Texture* Material::getTexture(unsigned int unit) {
    auto textureUnit = textureUnits.find(unit);
    if(textureUnit != textureUnits.end()) {
        return textureUnit->second;
    }

    return NULL;
}

void Material::setShaderProgram(ShaderProgram* shaderProgram) {
    this->shaderProgram = shaderProgram;
}

ShaderProgram* Material::getShaderProgram() {
    return shaderProgram;
}

void Material::setName(std::string name) {
    this->name = name;
}

const std::string& Material::getName() {
    return name;
}

void Material::setAmbient(double r, double g, double b, double a) {
    ambient.r = r;
    ambient.g = g;
    ambient.b = b;
    ambient.a = a;
}

const Color& Material::getAmbient() const {
    return ambient;
}

void Material::setDiffuse(double r, double g, double b, double a) {
    diffuse.r = r;
    diffuse.g = g;
    diffuse.b = b;
    diffuse.a = a;
}

const Color& Material::getDiffuse() const {
    return diffuse;
}

void Material::setSpecular(double r, double g, double b, double a) {
    specular.r = r;
    specular.g = g;
    specular.b = b;
    specular.a = a;
}

const Color& Material::getSpecular() const {
    return specular;
}

void Material::setShininess(double shininess) {
    this->shininess = shininess;
}

double Material::getShininess() {
    return shininess;
}
