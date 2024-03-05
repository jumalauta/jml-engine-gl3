#ifndef ENGINE_GRAPHICS_MODEL_MATERIAL_H_
#define ENGINE_GRAPHICS_MODEL_MATERIAL_H_

#include <map>
#include <string>
#include "graphics/datatypes.h"

class Texture;
class ShaderProgram;

class Material {
public:
    Material();
    ~Material();

    static Material* getCurrentMaterial();

    std::string toString() const;

    void setTexture(Texture *texture, unsigned int unit = 0);
    Texture* getTexture(unsigned int unit = 0);
    void bind();
    void unbind();
    void setShaderProgram(ShaderProgram* shaderProgram);
    ShaderProgram* getShaderProgram();

    void setName(std::string name);
    const std::string& getName();

    void setAmbient(double r, double g, double b, double a = 1.0);
    const Color& getAmbient() const;

    void setDiffuse(double r, double g, double b, double a = 1.0);
    const Color& getDiffuse() const;

    void setSpecular(double r, double g, double b, double a = 1.0);
    const Color& getSpecular() const;

    void setShininess(double shininess);
    double getShininess();
private:
    static Material* currentMaterial;

    std::string name;
    ShaderProgram* shaderProgram;
    std::map<unsigned int, Texture*> textureUnits;

    Color ambient;
    Color diffuse;
    Color specular;

    double shininess;
};

#endif /*ENGINE_GRAPHICS_MODEL_MATERIAL_H_*/
