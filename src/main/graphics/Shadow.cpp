#include "graphics/Shadow.h"

#include "Settings.h"
#include "logger/logger.h"

#include "io/MemoryManager.h"
#include "graphics/Camera.h"
#include "graphics/Texture.h"
#include "graphics/ShaderProgram.h"
#include "math/TransformationMatrixGlm.h"
#include "graphics/Fbo.h"
#include "graphics/Light.h"

Shadow::Shadow() {
    textureUnit = 0;
    fbo = NULL;
    camera = NULL;
}

Shadow::~Shadow() {
    if (camera) {
        delete camera;
    }
}

const float* Shadow::getMvp() {
    return glm::value_ptr(mvp);
}

Camera& Shadow::getCamera() {
    return *camera;
}

Fbo& Shadow::getFbo() {
    return *fbo;
}

void Shadow::setTextureUnit(unsigned int textureUnit) {
    this->textureUnit = textureUnit;
}

bool Shadow::init() {
    name = std::string("shadow") + std::to_string(textureUnit);
    camera = new Camera();
    camera->setName(name);

    MemoryManager<Fbo>& fboMemory = MemoryManager<Fbo>::getInstance();
    fbo = fboMemory.getResource(name, true);
    if (!fbo->generate()) {
        loggerFatal("Failed initializing %s", name.c_str());
        return false;
    }

    MemoryManager<ShaderProgram>& shaderProgramMemory = MemoryManager<ShaderProgram>::getInstance();
    defaultShadow = shaderProgramMemory.getResource(Settings::demo.graphics.shaderProgramDefaultShadow, false);
    if (!defaultShadow) {
        loggerDebug("No shadow shader program found! searchedName:'%s'", Settings::demo.graphics.shaderProgramDefaultShadow.c_str());
    }

    return true;
}

void Shadow::setCameraFromLight(Light &light) {
    if (!light.getGenerateShadowMap()) {
        loggerWarning("Light not defined to generate shadows. %s", light.toString().c_str());
        return;
    }

    if (light.getType() == LightType::SPOT) {
        const Vector3& position = light.getPosition();
        camera->setPosition(position.x, position.y, position.z);
        const Vector3& direction = light.getDirection();
        camera->setDirection(direction.x, direction.y, direction.z);
    }
    /*else if (light.getType() == LightType::POINT) {
        const Vector3& position = light.getPosition();
        camera->setPosition(position.x, position.y, position.z);

        Vector3 direction(defaultCamera->getLookAt());
        direction = direction - position;
        direction = direction.normalize();
        camera->setDirection(direction.x, direction.y, direction.z);
    } else {
        // FIXME: directional light shadow?
        //const Vector3& direction = light.getDirection();
        //camera->setPosition(direction.x, direction.y, direction.z);
        continue;
    }*/
    else {
        loggerWarning("Light type not supported. %s", light.toString().c_str());
    }
}

void Shadow::captureStart() {
    fbo->start();

    TransformationMatrixGlm& transformationMatrix = dynamic_cast<TransformationMatrixGlm&>(TransformationMatrix::getInstance());
    transformationMatrix.perspective3d();
    transformationMatrix.calculateMvp();
    mvp = transformationMatrix.mvp;

    if (defaultShadow) {
        defaultShadow->bind();
    }

    loggerWarning("Capturing shadow!");
}

void Shadow::captureEnd() {
    TransformationMatrixGlm& transformationMatrix = dynamic_cast<TransformationMatrixGlm&>(TransformationMatrix::getInstance());
    mvp = transformationMatrix.mvp;

    if (defaultShadow) {
        defaultShadow->unbind();
    }

    fbo->end();
}

void Shadow::textureBind() {
    fbo->getDepthTexture()->bind(textureUnit);
}

void Shadow::textureUnbind() {
    fbo->getDepthTexture()->unbind(textureUnit);
}
