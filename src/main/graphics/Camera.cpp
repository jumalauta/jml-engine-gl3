#include "Camera.h"

#include "Settings.h"

#include "glm/glm.hpp"

#include <sstream>

Camera::Camera() {
    name = "Default";
    aspectRatio = Settings::demo.graphics.aspectRatio;
    clipPlaneNear = 0.1;
    clipPlaneFar = 1000.0;
    horizontalFov = glm::radians(45.0);

    setPosition(0.0, 0.0, 2.0);
    setLookAt(0.0, 0.0, 0.0);
    setUp(0.0, 1.0, 0.0);
}

std::string Camera::toString() const {
    std::stringstream ss;
    ss << "name: " << name;
    ss << ", position: x:" << position.x << ", y:" << position.y << ", z:" << position.z;
    ss << ", lookAt: x:" << lookAt.x << ", y:" << lookAt.y << ", z:" << lookAt.z;
    ss << ", up: x:" << up.x << ", y:" << up.y << ", z:" << up.z;
    ss << ", aspectRatio: " << aspectRatio;
    ss << ", clipPlane: " << clipPlaneNear << " - " << clipPlaneFar;
    return ss.str();
}

void Camera::setName(std::string name) {
    this->name = name;
}
const std::string& Camera::getName() const {
    return name;
}

void Camera::setAspectRatio(double aspectRatio) {
    this->aspectRatio = aspectRatio;
}
double Camera::getAspectRatio() const {
    return aspectRatio;
}


void Camera::setClipPlaneNear(double clipPlaneNear) {
    this->clipPlaneNear = clipPlaneNear;
}
double Camera::getClipPlaneNear() const {
    return clipPlaneNear;
}
void Camera::setClipPlaneFar(double clipPlaneFar) {
    this->clipPlaneFar = clipPlaneFar;
}
double Camera::getClipPlaneFar() const {
    return clipPlaneFar;
}

void Camera::setHorizontalFov(double horizontalFov) {
    this->horizontalFov = horizontalFov;
}
double Camera::getHorizontalFov() const {
    return horizontalFov;
}

void Camera::setPosition(double x, double y, double z) {
    position.x = x;
    position.y = y;
    position.z = z;
}

const Vector3& Camera::getPosition() const {
    return position;
}

void Camera::setDirection(double x, double y, double z) {
    // TODO: should normalize maybe baby?
    lookAt.x = position.x + x;
    lookAt.y = position.y + y;
    lookAt.z = position.z + z;
}

void Camera::setLookAt(double x, double y, double z) {
    lookAt.x = x;
    lookAt.y = y;
    lookAt.z = z;
}
const Vector3& Camera::getLookAt() const {
    return lookAt;
}

void Camera::setUp(double x, double y, double z) {
    up.x = x;
    up.y = y;
    up.z = z;
}
const Vector3& Camera::getUp() const {
    return up;
}
