#ifndef ENGINE_GRAPHICS_MODEL_MESH_H_
#define ENGINE_GRAPHICS_MODEL_MESH_H_

#include <vector>
#include <string>
#include "GL/gl3w.h"
#include "graphics/datatypes.h"

#include "Material.h"


 enum class FaceType {
    POINTS,
    LINE_STRIP,
    LINE_LOOP,
    LINES,
    TRIANGLE_STRIP,
    TRIANGLES
};

class Mesh {
public:
    Mesh();
    ~Mesh();

    void print();

    void setName(std::string name);
    const std::string& getName();

    void addVertex(float x, float y, float z = 0.0f);
    void addNormal(float x, float y, float z = 0.0f);
    void addTexCoord(float x, float y);
    void addColor(float r, float g, float b, float a = 1.0f);
    void addIndex(unsigned int index);
    void setMaterial(Material *material, bool handleMaterialMemory = false);
    Material* getMaterial();
    void setFaceDrawType(FaceType faceDrawType);
    void clear();
    void free();
    bool generate();
    bool isGenerated();
    void draw(double begin, double end);
    void draw();

    void setRotate(double x, double y, double z);
    void setScale(double x, double y, double z);
    void setTranslate(double x, double y, double z);

    void begin(FaceType faceDrawType);
    void end();
private:
    GLenum getDrawElementsMode();

    std::string name;

    //TODO: Maybe abstract these to faces?
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texCoords;
    std::vector<float> colors;
    std::vector<unsigned int> indices;
    Material* material;
    bool handleMaterialMemory;
    GLuint vertexArray;
    GLuint vertexBuffer;
    GLuint texCoordBuffer;
    GLuint normalBuffer;
    GLuint colorBuffer;
    GLuint indexBuffer;
    FaceType faceDrawType;

    Vector3 scale;
    Vector3 translate;
    Vector3 rotate;
};

#endif /*ENGINE_GRAPHICS_MODEL_MESH_H_*/
