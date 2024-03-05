#ifndef ENGINE_GRAPHICS_CAMERA_H_
#define ENGINE_GRAPHICS_CAMERA_H_

#include <string>
#include "datatypes.h"

class Camera {
public:
    explicit Camera();

    std::string toString() const;
    
    void setName(std::string name);
    const std::string& getName() const;
    void setAspectRatio(double aspectRatio);
    double getAspectRatio() const;
    void setClipPlaneNear(double clipPlaneNear);
    double getClipPlaneNear() const;
    void setClipPlaneFar(double clipPlaneFar);
    double getClipPlaneFar() const;
    void setHorizontalFov(double horizontalFov);
    double getHorizontalFov() const;
    void setPosition(double x, double y, double z);
    const Vector3& getPosition() const;
    void setDirection(double x, double y, double z);
    void setLookAt(double x, double y, double z);
    const Vector3& getLookAt() const;
    void setUp(double x, double y, double z);
    const Vector3& getUp() const;

private:
    std::string name;
    double aspectRatio;
    double clipPlaneNear;
    double clipPlaneFar;
    double horizontalFov;
    Vector3 position;
    Vector3 lookAt;
    Vector3 up;
};

#endif /*ENGINE_GRAPHICS_CAMERA_H_*/
