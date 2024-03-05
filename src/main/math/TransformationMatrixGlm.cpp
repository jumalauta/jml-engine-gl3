#include "TransformationMatrixGlm.h"

#include "MathUtils.h"

#include "Settings.h"
#include "logger/logger.h"

#include "EnginePlayer.h"
#include "graphics/Graphics.h"
#include "graphics/Camera.h"
#include "graphics/LightManager.h"

#include "glm/gtc/matrix_inverse.hpp"

std::vector<TransformationMatrixGlm*> TransformationMatrixGlm::matrixStack = {};

void TransformationMatrixGlm::push() {
    PROFILER_BLOCK("TransformationMatrixGlm::push");
    matrixStack.push_back(new TransformationMatrixGlm(*this));
}

void TransformationMatrixGlm::pop() {
    PROFILER_BLOCK("TransformationMatrixGlm::pop");

    if (matrixStack.begin() == matrixStack.end()) {
        loggerFatal("Attempted to pop empty matrix stack");
        return;
    }

    delete matrixStack.back();
    matrixStack.pop_back();

    if (! matrixStack.empty()) {
        *this = *matrixStack.back();
    }
}

TransformationMatrix& TransformationMatrix::getInstance() {
    static TransformationMatrixGlm transformationMatrixGlm;
    return transformationMatrixGlm;
}

void TransformationMatrixGlm::operator=(const TransformationMatrixGlm& src) {
    mvp = src.mvp;
    fprojection = src.fprojection;
    fview = src.fview;
    fmodel = src.fmodel;

    projection = src.projection;
    view = src.view;
    model = src.model;
    normalMatrix = src.normalMatrix;
    setMode(src.mode);
}

TransformationMatrixGlm::TransformationMatrixGlm() {
    matrix = NULL;
    projection = glm::dmat4(1.0);
    view = glm::dmat4(1.0);
    model = glm::dmat4(1.0);
    setProjectionMode();
}

TransformationMatrixGlm::~TransformationMatrixGlm() {
    /*if (! matrixStack.empty()) {
        loggerWarning("Non-empty transformation matrix stack during destructor. size:%u", matrixStack.size());
    }*/
}

void TransformationMatrixGlm::setProjectionMode() {
    setMode(PROJECTION);
}

void TransformationMatrixGlm::setViewMode() {
    setMode(VIEW);
}

void TransformationMatrixGlm::setModelMode() {
    setMode(MODEL);
}

void TransformationMatrixGlm::setMode(MatrixMode mode) {
    this->mode = mode;
    switch(mode) {
        case PROJECTION:
            matrix = &projection;
            break;
        case VIEW:
            matrix = &view;
            break;
        case MODEL:
            matrix = &model;
            break;
        default:
            loggerError("Unknown matrix mode: %d", mode);
            break;
    }
}

void TransformationMatrixGlm::setMatrix4( const double* rowOrderMat4x4 ) {
    // row-order matrix (instead of column-order that OpenGL glLoadMatrixf would use)
    const double* m = rowOrderMat4x4;
    *matrix = glm::dmat4(m[ 0], m[ 1], m[ 2], m[ 3],
                         m[ 4], m[ 5], m[ 6], m[ 7],
                         m[ 8], m[ 9], m[10], m[11],
                         m[12], m[13], m[14], m[15]);
}

const double* TransformationMatrixGlm::getMatrix4() {
    return glm::value_ptr(*matrix);
}

void TransformationMatrixGlm::loadIdentity() {
    *matrix = glm::dmat4(1.0);
}

void TransformationMatrixGlm::translate(double x, double y, double z) {
    *matrix = glm::translate(*matrix, glm::dvec3(x, y, z));
}

void TransformationMatrixGlm::scale(double x, double y, double z) {
    *matrix = glm::scale(*matrix, glm::dvec3(x, y, z));
}

void TransformationMatrixGlm::rotateQuaternion(double w, double x, double y, double z) {
    glm::quat quaternion = glm::quat(w, x, y, z);
    glm::vec3 eulerDegrees = glm::eulerAngles(quaternion) * static_cast<float>(180.0f / M_PI);
    rotateX(-eulerDegrees.x);
    rotateY(-eulerDegrees.y);
    rotateZ(-eulerDegrees.z);
}

void TransformationMatrixGlm::rotateX(double degrees) {
    *matrix = glm::rotate(*matrix, glm::radians(degrees), glm::dvec3(-1.0, 0.0, 0.0));
}

void TransformationMatrixGlm::rotateY(double degrees) {
    *matrix = glm::rotate(*matrix, glm::radians(degrees), glm::dvec3(0.0, -1.0, 0.0));
}

void TransformationMatrixGlm::rotateZ(double degrees) {
    *matrix = glm::rotate(*matrix, glm::radians(degrees), glm::dvec3(0.0, 0.0, -1.0));
}

void TransformationMatrixGlm::perspective2d() {
    perspective2d(static_cast<double>(Settings::demo.graphics.canvasWidth), static_cast<double>(Settings::demo.graphics.canvasHeight));
}

void TransformationMatrixGlm::perspective2d(double width, double height) {
    PROFILER_BLOCK("TransformationMatrixGlm::perspective2d");

    Graphics &graphics = Graphics::getInstance();
    graphics.setDepthTest(false);

    LightManager& lightManager = LightManager::getInstance();
    lightManager.setLighting(false);

    setProjectionMode();
    *matrix = glm::ortho(0.0, width, 0.0, height);

    setViewMode();
    loadIdentity();

    setModelMode();
    loadIdentity();
}

void TransformationMatrixGlm::perspective3d() {
    PROFILER_BLOCK("TransformationMatrixGlm::perspective3d");

    Camera& camera = EnginePlayer::getInstance().getActiveCamera();

    Graphics &graphics = Graphics::getInstance();
    graphics.setDepthTest(true);

    LightManager& lightManager = LightManager::getInstance();
    lightManager.setLighting(true);

    setProjectionMode();
    *matrix = glm::perspective(camera.getHorizontalFov(), camera.getAspectRatio(), camera.getClipPlaneNear(), camera.getClipPlaneFar());

    setViewMode();

    const Vector3& pos = camera.getPosition();
    const Vector3& look = camera.getLookAt();
    const Vector3& up = camera.getUp();
    *matrix = glm::lookAt(
        glm::dvec3(pos.x, pos.y, pos.z),
        glm::dvec3(look.x, look.y, look.z),
        glm::dvec3(up.x, up.y, up.z)
    );
    //loggerTrace("Current camera: %s", camera.toString().c_str());

    setModelMode();
    loadIdentity();
}

glm::mat4 TransformationMatrixGlm::calculateMvp() {
    PROFILER_BLOCK("TransformationMatrixGlm::calculateMvp");

    mvp = projection * view * model;
    return mvp;
}

const float* TransformationMatrixGlm::getMvp() {
    calculateMvp(); // TODO: precalculate mvp only when data changes

    return glm::value_ptr(mvp);
}

void TransformationMatrixGlm::print() {
    const float* m = getMvp();

    loggerInfo("MVP:\n[\n" \
               "  %.2f\t%.2f\t%.2f\t%.2f\n" \
               "  %.2f\t%.2f\t%.2f\t%.2f\n" \
               "  %.2f\t%.2f\t%.2f\t%.2f\n" \
               "  %.2f\t%.2f\t%.2f\t%.2f\n" \
               "]",
               m[ 0], m[ 1], m[ 2], m[ 3],
               m[ 4], m[ 5], m[ 6], m[ 7],
               m[ 8], m[ 9], m[10], m[11],
               m[12], m[13], m[14], m[15]);

}

const float* TransformationMatrixGlm::getShadowMvp() {
    calculateMvp(); // TODO: precalculate mvp only when data changes

    return glm::value_ptr(mvp);

    /*//calculateMvp(); // TODO: precalculate mvp only when data changes
    glm::vec3 lightInvDir = glm::vec3(0.5f,2,2);

    // Compute the MVP matrix from the light's point of view
    glm::mat4 depthProjectionMatrix = projection;//glm::ortho<float>(-100,100,-100,100,0.1,1000.0);
    glm::mat4 depthViewMatrix = view;//glm::lookAt(lightInvDir, glm::vec3(0,0,0), glm::vec3(0,1,0));
    glm::mat4 depthModelMatrix = glm::mat4(1.0);
    mvp = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

    //projection = depthProjectionMatrix;
    mvp = projection * view * model;

    glm::mat4 biasMatrix(
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0
    );
    mvp = biasMatrix * mvp;

    return glm::value_ptr(mvp);*/
}

const float* TransformationMatrixGlm::getNormalMatrix() {
    //TODO: Precalculate normal matrix only when data changes
    normalMatrix = glm::inverseTranspose(glm::mat3(view * model));
    return glm::value_ptr(normalMatrix);
}

const float* TransformationMatrixGlm::getProjectionMatrix() {
    fprojection = glm::mat4(projection);
    return glm::value_ptr(fprojection);
}

const float* TransformationMatrixGlm::getModelMatrix() {
    fmodel = glm::mat4(model);
    return glm::value_ptr(fmodel);
}

const float* TransformationMatrixGlm::getViewMatrix() {
    fview = glm::mat4(view);
    return glm::value_ptr(fview);
}
