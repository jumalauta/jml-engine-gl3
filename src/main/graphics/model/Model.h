#ifndef ENGINE_GRAPHICS_MODEL_MODEL_H_
#define ENGINE_GRAPHICS_MODEL_MODEL_H_

#include <string>

#include "io/File.h"

class Material;
class Mesh;

class Model : public File {
public:
    static Model* newInstance(std::string filePath);

    virtual ~Model() {};
    virtual bool isSupported() = 0;

    virtual Material *getMaterial(unsigned int index) = 0;
    virtual Mesh *getMesh(unsigned int index) = 0;
    virtual Mesh *getMesh(std::string name) = 0;
    virtual void addMaterial(Material* material) = 0;
    virtual void addMesh(Mesh* mesh) = 0;
    virtual void draw() = 0;
    virtual bool load() = 0;
protected:
    explicit Model(std::string filePath);
};

#endif /*ENGINE_GRAPHICS_MODEL_MODEL_H_*/
