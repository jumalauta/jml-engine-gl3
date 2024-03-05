#include "Light.h"

#include "logger/logger.h"

#include <sstream>

Light::Light() {
    setType(LightType::DIRECTIONAL);
    generateShadowMap = false;
}

std::string Light::toString() const {
    std::stringstream ss;
    ss << "name: " << name;
    ss << ", type: ";
    switch (type) {
        case LightType::DIRECTIONAL:
            ss << "DIRECTIONAL";
            break;
        case LightType::POINT:
            ss << "POINT";
            break;
        case LightType::SPOT:
            ss << "SPOT";
            break;
        default:
            ss << "UNKNOWN";
            break;
    }
    ss << ", shadows: " << generateShadowMap;
    ss << ", position: x:" << position.x << ", y:" << position.y << ", z:" << position.z;
    ss << ", direction: x:" << direction.x << ", y:" << direction.y << ", z:" << direction.z;
    ss << ", ambient: r:" << ambient.r << ", g:" << ambient.g << ", b:" << ambient.b << ", a:" << ambient.a;
    ss << ", diffuse: r:" << diffuse.r << ", g:" << diffuse.g << ", b:" << diffuse.b << ", a:" << diffuse.a;
    ss << ", specular: r:" << specular.r << ", g:" << specular.g << ", b:" << specular.b << ", a:" << specular.a;
    return ss.str();
}

void Light::setName(std::string name) {
    this->name = name;
}

const std::string& Light::getName() const {
    return name;
}

void Light::setType(LightType type) {
    this->type = type;
}

const LightType& Light::getType() const {
    return type;
}

void Light::setPosition(double x, double y, double z) {
    position.x = x;
    position.y = y;
    position.z = z;
}

const Vector3& Light::getPosition() const {
    return position;
}

void Light::setDirection(double x, double y, double z) {
    direction.x = x;
    direction.y = y;
    direction.z = z;
}

const Vector3& Light::getDirection() const {
    return direction;
}

void Light::setAmbient(double r, double g, double b, double a) {
    ambient.r = r;
    ambient.g = g;
    ambient.b = b;
    ambient.a = a;
}

const Color& Light::getAmbient() const {
    return ambient;
}

void Light::setDiffuse(double r, double g, double b, double a) {
    diffuse.r = r;
    diffuse.g = g;
    diffuse.b = b;
    diffuse.a = a;
}

const Color& Light::getDiffuse() const {
    return diffuse;
}

void Light::setSpecular(double r, double g, double b, double a) {
    specular.r = r;
    specular.g = g;
    specular.b = b;
    specular.a = a;
}

const Color& Light::getSpecular() const {
    return specular;
}

void Light::setGenerateShadowMap(bool generateShadowMap) {
    if (generateShadowMap && type != LightType::SPOT) {
        loggerWarning("Shadow maps can't be generated from this type of light! %s", toString().c_str());
        return;
    }
    this->generateShadowMap = generateShadowMap;
}

bool Light::getGenerateShadowMap() const {
    return generateShadowMap;
}
