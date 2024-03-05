#ifndef ENGINE_MATH_TRANSFORMATIONMATRIX_H_
#define ENGINE_MATH_TRANSFORMATIONMATRIX_H_

enum MatrixMode {
    PROJECTION,
    VIEW,
    MODEL
};

class TransformationMatrix {
public:
    static TransformationMatrix& getInstance();
    virtual void setProjectionMode() = 0;
    virtual void setViewMode() = 0;
    virtual void setModelMode() = 0;
    virtual void loadIdentity() = 0;
    virtual void setMatrix4(const double* rowOrderMat4x4) = 0;
    virtual const double* getMatrix4() = 0;
    virtual void translate(double x, double y, double z) = 0;
    virtual void scale(double x, double y, double z) = 0;
    virtual void rotateQuaternion(double w, double x, double y, double z) = 0;
    virtual void rotateX(double degrees) = 0;
    virtual void rotateY(double degrees) = 0;
    virtual void rotateZ(double degrees) = 0;
    virtual void perspective2d() = 0;
    virtual void perspective2d(double width, double height) = 0;
    virtual void perspective3d() = 0;

    virtual void push() = 0;
    virtual void pop() = 0;

    virtual void print() = 0;

    // float as OpenGL uniforms don't support double natively
    virtual const float* getMvp() = 0;
    virtual const float* getShadowMvp() = 0;
    virtual const float* getNormalMatrix() = 0;
    virtual const float* getProjectionMatrix() = 0;
    virtual const float* getModelMatrix() = 0;
    virtual const float* getViewMatrix() = 0;

    virtual ~TransformationMatrix() {};
protected:
    TransformationMatrix() {};
};

#endif /*ENGINE_MATH_TRANSFORMATIONMATRIX_H_*/
