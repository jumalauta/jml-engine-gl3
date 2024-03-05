#ifndef ENGINE_MATH_TRANSFORMATIONMATRIXGLM_H_
#define ENGINE_MATH_TRANSFORMATIONMATRIXGLM_H_

#include "TransformationMatrix.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"

#include <vector>

class TransformationMatrixGlm : public TransformationMatrix {
public:
    void operator=(const TransformationMatrixGlm& src);
    TransformationMatrixGlm();
    ~TransformationMatrixGlm();
    void setProjectionMode();
    void setViewMode();
    void setModelMode();
    void setMatrix4(const double* rowOrderMat4x4);
    const double* getMatrix4();
    void loadIdentity();
    void translate(double x, double y, double z);
    void scale(double x, double y, double z);
    void rotateQuaternion(double w, double x, double y, double z);
    void rotateX(double degrees);
    void rotateY(double degrees);
    void rotateZ(double degrees);
    void perspective2d();
    void perspective2d(double width, double height);
    void perspective3d();

    void push();
    void pop();

    // float as OpenGL uniforms don't support double natively
    const float* getMvp();
    const float* getShadowMvp();
    const float* getNormalMatrix();
    const float* getProjectionMatrix();
    const float* getModelMatrix();
    const float* getViewMatrix();

    void print();
    glm::mat4 calculateMvp();
    glm::mat4 mvp;
private:
    void setMode(MatrixMode mode);

    glm::dmat4 *matrix;
    glm::dmat4 projection;
    glm::dmat4 view;
    glm::dmat4 model;

    // float precisions are disliked
    glm::mat4 fprojection;
    glm::mat4 fview;
    glm::mat4 fmodel;
    glm::mat3 normalMatrix;

    MatrixMode mode;

    static std::vector<TransformationMatrixGlm*> matrixStack;
};

#endif /*ENGINE_MATH_TRANSFORMATIONMATRIXGLM_H_*/
