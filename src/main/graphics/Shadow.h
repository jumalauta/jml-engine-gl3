#ifndef ENGINE_GRAPHICS_SHADOW_H_
#define ENGINE_GRAPHICS_SHADOW_H_

#include <string>
#include "math/TransformationMatrixGlm.h"

class TransformationMatrix;
class Camera;
class Fbo;
class ShaderProgram;
class Light;

class Shadow {
public:
    Shadow();
    ~Shadow();
    bool init();
    const float* getMvp();

    Camera& getCamera();
    Fbo& getFbo();
    void setTextureUnit(unsigned int textureUnit);
    void setCameraFromLight(Light &light);
    void captureStart();
    void captureEnd();
    void textureBind();
    void textureUnbind();
private:
    glm::mat4 mvp;
    std::string name;
    Fbo *fbo;
    Camera *camera;
    TransformationMatrixGlm transformationMatrix;
    unsigned int textureUnit;
    ShaderProgram *defaultShadow;
};

#endif /*ENGINE_GRAPHICS_SHADOW_H_*/
