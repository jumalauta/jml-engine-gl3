#ifndef ENGINE_GRAPHICS_MODEL_MODELASSIMP_H_
#define ENGINE_GRAPHICS_MODEL_MODELASSIMP_H_

#include <vector>

#include "Model.h"

#include <assimp/Importer.hpp>

struct aiScene;
struct aiMaterial;
struct aiMesh;
struct aiNode;
struct aiAnimation;
struct aiCamera;
struct aiLight;

class ModelAssimp : public Model {
public:
    explicit ModelAssimp(std::string filePath);
    ~ModelAssimp();
    bool isSupported();
    bool isLoaded();

    Material *getMaterial(unsigned int index);
    Mesh *getMesh(unsigned int index);
    Mesh *getMesh(std::string name);
    void addMaterial(Material* material);
    void addMesh(Mesh* mesh);
    void draw();
    bool load();
    void clear();
protected:
    std::vector<Material*> materials;
    std::vector<Mesh*> meshes;
private:
    void drawNode(const aiScene* scene, const aiNode *node);

    bool handleMaterial(const aiMaterial* material);
    bool handleMesh(const aiScene* scene, const aiMesh* mesh);
    bool handleCamera(const aiCamera* camera);
    bool handleLight(const aiLight* light);

    Assimp::Importer importer;
};

#endif /*ENGINE_GRAPHICS_MODEL_MODELASSIMP_H_*/
