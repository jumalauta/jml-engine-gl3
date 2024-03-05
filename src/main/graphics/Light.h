#ifndef ENGINE_GRAPHICS_LIGHT_H_
#define ENGINE_GRAPHICS_LIGHT_H_

#include <string>
#include "datatypes.h"

enum class LightType {
    //NOTE: Hard-coded values in GLSL
    DIRECTIONAL=1,
    POINT=2,
    SPOT=3
};

class Light {
public:
    explicit Light();

    std::string toString() const;

    void setName(std::string name);
    const std::string& getName() const;

    void setType(LightType type);
    const LightType& getType() const;

    void setPosition(double x, double y, double z);
    const Vector3& getPosition() const;

    void setDirection(double x, double y, double z);
    const Vector3& getDirection() const;

    void setAmbient(double r, double g, double b, double a);
    const Color& getAmbient() const;

    void setDiffuse(double r, double g, double b, double a);
    const Color& getDiffuse() const;

    void setSpecular(double r, double g, double b, double a);
    const Color& getSpecular() const;

    void setGenerateShadowMap(bool generateShadowMap);
    bool getGenerateShadowMap() const;
private:
    std::string name;

    LightType type;

    bool generateShadowMap;

    Vector3 direction;
    Vector3 position;

    Color ambient;
    Color diffuse;
    Color specular;
};

#endif /*ENGINE_GRAPHICS_LIGHT_H_*/
