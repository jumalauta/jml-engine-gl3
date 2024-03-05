#include "Mesh.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "graphics/TextureOpenGl.h"

#include "logger/logger.h"


// TODO: Determine this shit dynamically
#define VERTEX_ATTRIB 0
#define UV_ATTRIB 1
#define NORMAL_ATTRIB 2
#define COLOR_ATTRIB 3

#include "graphics/Graphics.h"
#include "graphics/Image.h"
#include "graphics/Texture.h"
#include "graphics/Shader.h"
#include "io/MemoryManager.h"
#include "graphics/ShaderProgram.h"
#include "graphics/ShaderProgramOpenGl.h"
#include "math/TransformationMatrix.h"
#include "time/Timer.h"

#include "EnginePlayer.h"

//FIXME: No direct referencing to OpenGl
#include "GL/gl3w.h"

Mesh::Mesh() {
    vertexArray = 0;
    vertexBuffer = 0;
    texCoordBuffer = 0;
    normalBuffer = 0;
    colorBuffer = 0;
    indexBuffer = 0;
    faceDrawType = FaceType::TRIANGLES;
    // FIXME: Material should construct as a default
    material = NULL;
    handleMaterialMemory = false;
    name = "UntitledMesh";
    clear();

    setRotate(0.0, 0.0, 0.0);
    setTranslate(0.0, 0.0, 0.0);
    setScale(1.0, 1.0, 1.0);
}

Mesh::~Mesh() {
    if (vertexArray != 0) {
        free();
    }

    clear();
    if (handleMaterialMemory) {
        delete material;
        handleMaterialMemory = false;
    }
    material = NULL;
}

void Mesh::setName(std::string name) {
    this->name = name;
}

const std::string& Mesh::getName() {
    return name;
}

void Mesh::print() {
    loggerInfo("Mesh(%s, 0x%p, type:%d) - VAO(%u), Faces: %u, Vertices(%u): %u, normals(%u): %u, UVs(%u): %u, colors(%u): %u, indices(%u): %u",
        name.c_str(),
        this,
        faceDrawType,
        vertexArray,
        vertices.size() / 3 / 3,
        vertexBuffer, vertices.size(),
        normalBuffer, normals.size(),
        texCoordBuffer, texCoords.size(),
        colorBuffer, colors.size(),
        indexBuffer, indices.size());
}

void Mesh::addVertex(float x, float y, float z) {
    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(z);
}

void Mesh::addColor(float r, float g, float b, float a) {
    colors.push_back(r);
    colors.push_back(g);
    colors.push_back(b);
    colors.push_back(a);
}

void Mesh::addNormal(float x, float y, float z) {
    normals.push_back(x);
    normals.push_back(y);
    normals.push_back(z);
}

void Mesh::addTexCoord(float x, float y) {
    texCoords.push_back(x);
    texCoords.push_back(y);
}

void Mesh::addIndex(unsigned int index) {
    /*loggerTrace("index %u: x:%.2f y:%.2f z:%2.f - vt: u:%.2f v:%.2f - vn: x:%.2f y:%.2f z:%2.f",
        index,
        vertices[index*3],vertices[index*3 + 1],vertices[index*3 + 2],
        texCoords[index*2],texCoords[index*2 + 1],
        normals[index*3],normals[index*3 + 1],normals[index*3 + 2]);*/

    indices.push_back(index);
}

void Mesh::setMaterial(Material *material, bool handleMaterialMemory) {
    this->material = material;
    this->handleMaterialMemory = handleMaterialMemory;
}

Material* Mesh::getMaterial() {
    return material;
}

void Mesh::setFaceDrawType(FaceType faceDrawType) {
    this->faceDrawType = faceDrawType;
}

bool Mesh::generate() {
    PROFILER_BLOCK("Mesh::generate");

    free();

    if (vertices.empty()) {
        loggerWarning("Mesh has no vertices, can't generate. ptr:0x%p", this);
        return false;
    }

    if (vertexArray == 0) {
        glGenVertexArrays(1, &vertexArray);
        if (vertexArray == 0) {
            loggerWarning("Could not generate vertex array for mesh.");
            return false;
        }
    }
    glBindVertexArray(vertexArray);

    if (! vertices.empty()) {
        if (vertexBuffer == 0) {
            glGenBuffers(1, &vertexBuffer);
            if (vertexBuffer == 0) {
                loggerWarning("Could not generate vertex buffer for mesh. vertices:%d", vertices.size());
                return false;
            }
        }
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glEnableVertexAttribArray(VERTEX_ATTRIB);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_DYNAMIC_DRAW);
        glVertexAttribPointer(VERTEX_ATTRIB, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    }

    if (! texCoords.empty()) {
        if (texCoordBuffer == 0) {
            glGenBuffers(1, &texCoordBuffer);
            if (texCoordBuffer == 0) {
                loggerWarning("Could not generate normal buffer for mesh. texCoords:%d", texCoords.size());
                return false;
            }
        }
        glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
        glEnableVertexAttribArray(UV_ATTRIB);

        //glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
        glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(float), &texCoords[0], GL_DYNAMIC_DRAW);
        //glBindBuffer(GL_ARRAY_BUFFER, 0);
        glVertexAttribPointer(UV_ATTRIB, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    }

    if (! normals.empty()) {
        if (normalBuffer == 0) {
            glGenBuffers(1, &normalBuffer);
            if (normalBuffer == 0) {
                loggerWarning("Could not generate normal buffer for mesh. normals:%d", normals.size());
                return false;
            }
        }
        glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
        glEnableVertexAttribArray(NORMAL_ATTRIB);

        //glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), &normals[0], GL_DYNAMIC_DRAW);
        //glBindBuffer(GL_ARRAY_BUFFER, 0);
        glVertexAttribPointer(NORMAL_ATTRIB, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    }

    if (! colors.empty()) {
        if (colorBuffer == 0) {
            glGenBuffers(1, &colorBuffer);
            if (colorBuffer == 0) {
                loggerWarning("Could not generate color buffer for mesh. colors:%d", colors.size());
                return false;
            }
        }
        glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
        glEnableVertexAttribArray(COLOR_ATTRIB);
        glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), &colors[0], GL_DYNAMIC_DRAW);
        //glBindBuffer(GL_ARRAY_BUFFER, 0);
        glVertexAttribPointer(COLOR_ATTRIB, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    }

    if (! indices.empty()) {
        if (indexBuffer == 0) {
            glGenBuffers(1, &indexBuffer);
            if (indexBuffer == 0) {
                loggerWarning("Could not generate index buffer for mesh. indices:%d", indices.size());
                return false;
            }
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_DYNAMIC_DRAW);
    }

    glBindVertexArray(0);

    loggerTrace("Mesh generated. ptr:0x%p, vertexArray:%d, indexBuffer:%d, colorBuffer:%d, normalBuffer:%d, texCoordBuffer:%d, vertexBuffer:%d",
        this, vertexArray, indexBuffer, colorBuffer, normalBuffer, texCoordBuffer, vertexBuffer);

    Graphics &graphics = Graphics::getInstance();
    if (graphics.handleErrors()) {
        loggerError("Could not generate mesh. mesh:0x%p", this);
        return false;
    }

    return true;
}

void Mesh::free() {
    PROFILER_BLOCK("Mesh::free");

    glBindVertexArray(vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    if (vertexBuffer != 0) {
        glDeleteBuffers(1, &vertexBuffer);
        vertexBuffer = 0;
    }
    if (texCoordBuffer != 0) {
        glDeleteBuffers(1, &texCoordBuffer);
        texCoordBuffer = 0;
    }
    if (normalBuffer != 0) {
        glDeleteBuffers(1, &normalBuffer);
        normalBuffer = 0;
    }
    if (colorBuffer != 0) {
        glDeleteBuffers(1, &colorBuffer);
        colorBuffer = 0;
    }
    if (indexBuffer != 0) {
        glDeleteBuffers(1, &indexBuffer);
        indexBuffer = 0;
    }

    if (vertexArray != 0) {
        glBindVertexArray(0);
        glDeleteVertexArrays(1, &vertexArray);
        vertexArray = 0;
    }
}

void Mesh::clear() {
    vertices.clear();
    normals.clear();
    texCoords.clear();
    colors.clear();
    indices.clear();
}

bool Mesh::isGenerated() {
    if (vertexArray == 0) {
        return false;
    }

    return true;
}

void Mesh::draw() {
    draw(0.0, 1.0);
}

void Mesh::draw(double begin, double end) {
    PROFILER_BLOCK("Mesh::draw");
    if (!isGenerated()) {
        loggerError("Mesh not generated before draw attempt!");
        return;
    }

    if (begin != 0.0) {
        //FIXME
        loggerError("Setting of begin not supported at the moment... begin:%.2f, end:%.2f", begin, end);
        return;
    }

    TransformationMatrix& transformationMatrix = TransformationMatrix::getInstance();
    transformationMatrix.translate(translate.x, translate.y, translate.z);
    transformationMatrix.scale(scale.x, scale.y, scale.z);
    transformationMatrix.rotateX(rotate.x);
    transformationMatrix.rotateY(rotate.y);
    transformationMatrix.rotateZ(rotate.z);

    if (material) {
        material->bind();
    } else {
        ShaderProgram::useCurrentBind();
    }

    glBindVertexArray(vertexArray);

/*
    glEnableVertexAttribArray(VERTEX_ATTRIB);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(VERTEX_ATTRIB, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    bool renderTextureCoordinates = ! texCoords.empty();
    if (renderTextureCoordinates) {
        glEnableVertexAttribArray(UV_ATTRIB);
        glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
        glVertexAttribPointer(UV_ATTRIB, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    }

    bool renderNormals = ! normals.empty();
    if (renderNormals) {
        glEnableVertexAttribArray(NORMAL_ATTRIB);
        glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
        glVertexAttribPointer(NORMAL_ATTRIB, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    }

    bool renderColors = ! colors.empty();
    if (renderColors) {
        glEnableVertexAttribArray(COLOR_ATTRIB);
        glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
        glVertexAttribPointer(COLOR_ATTRIB, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    }
*/
    // FIXME: standard shader program uniform handling needed here...
    GLint enableVertexColorId = ShaderProgramOpenGl::getUniformLocation("enableVertexColor");
    if (enableVertexColorId != -1) {
        glUniform1i(enableVertexColorId, colors.empty() ? 0 : 1);
    }

    if (! indices.empty()) {
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glDrawElements(getDrawElementsMode(), indices.size() * end, GL_UNSIGNED_INT, 0);
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    } else {
        glDrawArrays(getDrawElementsMode(), 0, vertices.size() / 3 * end);
    }

    /*glDisableVertexAttribArray(VERTEX_ATTRIB);
    if (renderTextureCoordinates) {
        glDisableVertexAttribArray(UV_ATTRIB);
    }
    if (renderNormals) {
        glDisableVertexAttribArray(NORMAL_ATTRIB);
    }
    if (renderColors) {
        glDisableVertexAttribArray(COLOR_ATTRIB);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);*/
    glBindVertexArray(0);

    if (material) {
        material->unbind();
    } else {
        ShaderProgram::useCurrentBind();
    }
}

void Mesh::setRotate(double x, double y, double z) {
    rotate.x = x;
    rotate.y = y;
    rotate.z = z;
}

void Mesh::setScale(double x, double y, double z) {
    scale.x = x;
    scale.y = y;
    scale.z = z;
}

void Mesh::setTranslate(double x, double y, double z) {
    translate.x = x;
    translate.y = y;
    translate.z = z;
}

void Mesh::begin(FaceType faceDrawType) {
    PROFILER_BLOCK("Mesh::begin");

    setFaceDrawType(faceDrawType);
    clear();
}

void Mesh::end() {
    PROFILER_BLOCK("Mesh::end");

    generate();
    draw();
    clear();

    // As this mesh could be highly dynamic on single frame we need to ensure that commands are processed
    // ot we might end up in glitchy stuff
    glFinish();
}

GLenum Mesh::getDrawElementsMode() {
    switch(faceDrawType) {
        case FaceType::POINTS:
            return GL_POINTS;
        case FaceType::LINE_STRIP:
            return GL_LINE_STRIP;
        case FaceType::LINE_LOOP:
            return GL_LINE_LOOP;
        case FaceType::LINES:
            return GL_LINES;
        case FaceType::TRIANGLE_STRIP:
            return GL_TRIANGLE_STRIP;
        case FaceType::TRIANGLES:
            return GL_TRIANGLES;
        default:
            loggerWarning("Mesh face draw type not recognized! faceDrawType:%d", faceDrawType);
            return GL_TRIANGLES;
    }
}
