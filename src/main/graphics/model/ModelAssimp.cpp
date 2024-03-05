#include "ModelAssimp.h"

#include <assimp/LogStream.hpp>
#include <assimp/Logger.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/material.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>

#include "Settings.h"
#include "logger/logger.h"

#include "Material.h"
#include "Mesh.h"

#include "io/MemoryManager.h"
#include "math/MathUtils.h"
#include "math/TransformationMatrix.h"
#include "graphics/Image.h"
#include "graphics/Texture.h"
#include "graphics/Camera.h"
#include "graphics/Light.h"
#include "graphics/Graphics.h"
#include "graphics/Fbo.h"

#include "EnginePlayer.h"

#include <sstream>

#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"

static std::string matrixToString(const aiMatrix4x4& aiMat) {
    double matrix[16] = {
        aiMat[0][0], aiMat[0][1], aiMat[0][2], aiMat[0][3],
        aiMat[1][0], aiMat[1][1], aiMat[1][2], aiMat[1][3],
        aiMat[2][0], aiMat[2][1], aiMat[2][2], aiMat[2][3],
        aiMat[3][0], aiMat[3][1], aiMat[3][2], aiMat[3][3]
    };

    aiVector3D scale = aiVector3D();
    aiQuaternion rotate = aiQuaternion();
    aiVector3D translate = aiVector3D();
    aiMat.Decompose(scale, rotate, translate);

    std::stringstream ss;
    ss << "scale (x:" << scale.x << ", y:" << scale.y << ", z:" << scale.z << ")" << std::endl;
    ss << "rotate (w:" << rotate.w << ", x:" << rotate.x << ", y:" << rotate.y << ", z:" << rotate.z << ")" << std::endl;
    ss << "translate (x:" << translate.x << ", y:" << translate.y << ", z:" << translate.z << ")" << std::endl;
    ss << "[" << aiMat[0][0] << " " << aiMat[0][1] << " " << aiMat[0][2] << " " << aiMat[0][3] << "]" << std::endl;
    ss << "[" << aiMat[1][0] << " " << aiMat[1][1] << " " << aiMat[1][2] << " " << aiMat[1][3] << "]" << std::endl;
    ss << "[" << aiMat[2][0] << " " << aiMat[2][1] << " " << aiMat[2][2] << " " << aiMat[2][3] << "]" << std::endl;
    ss << "[" << aiMat[3][0] << " " << aiMat[3][1] << " " << aiMat[3][2] << " " << aiMat[3][3] << "]" << std::endl;

    return ss.str();
}

Model* Model::newInstance(std::string filePath) {
    ModelAssimp *model = new ModelAssimp(filePath);
    if (model == NULL) {
        loggerFatal("Could not allocate memory for model");
    }

    return model;
}

ModelAssimp::ModelAssimp(std::string filePath) : Model(filePath) {
    loggerInfo("Model init: '%s'", getFilePath().c_str());
}

ModelAssimp::~ModelAssimp() {
    clear();
}

bool ModelAssimp::isSupported() {
    std::string fileExtension = getFileExtension();
    std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::tolower);

    if (fileExtension == "obj"
        || fileExtension == "blend"
        || fileExtension == "dae") {

        aiString assimpSupportedExtensions;
        importer.GetExtensionList(assimpSupportedExtensions);
        if (std::string(assimpSupportedExtensions.data).find(fileExtension) == std::string::npos) {
            loggerWarning("File format that should be supported is not supported! supportedExtensions:'%s', file:'%s'",
                assimpSupportedExtensions.data, getFilePath().c_str());

            return false;
        }

        return true;
    }


    return false;
}


class AssimpCustomIOStream : public Assimp::IOStream {
public:
    AssimpCustomIOStream() {};
    ~AssimpCustomIOStream() {};
    size_t Read(void* pvBuffer, size_t pSize, size_t pCount) {
        return 0;
    }
    size_t Write(const void* pvBuffer, size_t pSize, size_t pCount) {
        return 0;
    }
    aiReturn Seek(size_t pOffset, aiOrigin pOrigin) { 
        return AI_SUCCESS;
    }
    size_t Tell() const { 
        return 0;
    }
    size_t FileSize() const { 
        return 0;
    }
    void Flush () { 
    }
};

class AssimpCustomIOSystem : public Assimp::IOSystem {
public:
    AssimpCustomIOSystem() {}
    ~AssimpCustomIOSystem() {}
private:
    bool Exists(const char* pFile) const {
        return false;
    }
    char GetOsSeparator() const { 
      return '/'; 
    }
    Assimp::IOStream* Open(const char* strFile, const char* strMode) {
          return new AssimpCustomIOStream(); 
    }
    void Close(Assimp::IOStream* pFile) {
        delete pFile;
    }
};

class AssimpCustomLogStream :public Assimp::LogStream {
public:
    void write(const char* message) {
        logPrint(message);
    }
private:
    void logPrint(const char *message) {
        static const char *WARNING_PREFIX = "Warn";
        static const char *ERROR_PREFIX = "Error";

        if (strncmp(WARNING_PREFIX, message, strlen(WARNING_PREFIX)) == 0) {
            // Warnings really don't seem that interesting but could be relevant for fuckupperies in rendering
            loggerInfo("%s", message);
        } else if (strncmp(ERROR_PREFIX, message, strlen(ERROR_PREFIX)) == 0) {
            // Errors really don't seem that interesting either but could be relevant for fuckupperies in rendering
            loggerInfo("%s", message);
        } else {
            // Treat assimp INFO and DEBUG messages as debug entries
            loggerDebug("%s", message);
        }
    }

};

Material *ModelAssimp::getMaterial(unsigned int index) {
    return materials[index];
}

void ModelAssimp::addMaterial(Material* material) {
    materials.push_back(material);
}

void ModelAssimp::addMesh(Mesh* mesh) {
    meshes.push_back(mesh);
}

Mesh *ModelAssimp::getMesh(unsigned int index) {
    return meshes[index];
}

Mesh *ModelAssimp::getMesh(std::string name) {
    for (Mesh* mesh : meshes) {
        if (mesh && mesh->getName() == name) {
            return mesh;
        }
    }

    return NULL;
}

bool ModelAssimp::isLoaded() {
    if (importer.GetScene() == NULL) {
        return false;
    }

    return true;
}

void ModelAssimp::draw() {
    const aiScene* scene = importer.GetScene();
    if (scene == NULL) {
        loggerWarning("No scene imported successfully, can't draw");
        return;
    }

    const aiNode *rootNode = scene->mRootNode;
    drawNode(scene, rootNode);
}

void ModelAssimp::drawNode(const aiScene* scene, const aiNode *node) {
    // ref: http://assimp.sourceforge.net/lib_html/structai_node.html

    aiVector3D scale = aiVector3D();
    aiQuaternion rotate = aiQuaternion();
    aiVector3D translate = aiVector3D();
    node->mTransformation.Decompose(scale, rotate, translate);

    TransformationMatrix& transformationMatrix = TransformationMatrix::getInstance();
    transformationMatrix.push();
    transformationMatrix.setModelMode();

    if (scene->HasAnimations()) {
        // Animations are handled last, as they may refer to previously processed data (nodes, meshes, cameras, lights)
        for (unsigned int animationI = 0; animationI < scene->mNumAnimations; animationI++) {
            const aiAnimation* animation = scene->mAnimations[animationI];

            // Nice write-up about animation: https://gamedev.stackexchange.com/questions/26382/i-cant-figure-out-how-to-animate-my-loaded-model-with-assimp/26442#26442
            // ref: http://assimp.sourceforge.net/lib_html/structai_animation.html

            double ticksPerSecond = animation->mTicksPerSecond; // Ticks per second. 
            double duration = animation->mDuration; // Duration of the animation in ticks.

            for (unsigned int channelI = 0; channelI < animation->mNumChannels; channelI++) {
                // ref: http://assimp.sourceforge.net/lib_html/structai_node_anim.html
                const aiNodeAnim* channel = animation->mChannels[channelI];

                if (strcmp(channel->mNodeName.data, node->mName.data)) {
                    // skip irrelevant animations
                    continue;
                }

                aiAnimBehaviour postState = channel->mPostState; // Defines how the animation behaves after the last key was processed.
                aiAnimBehaviour preState = channel->mPreState; // Defines how the animation behaves before the first key is encountered.
                // aiAnimBehaviour_DEFAULT // The value from the default node transformation is taken.
                // aiAnimBehaviour_CONSTANT // The nearest key value is used without interpolation.
                // aiAnimBehaviour_LINEAR   // The value of the nearest two keys is linearly extrapolated for the current time value.
                // aiAnimBehaviour_REPEAT // The animation is repeated. If the animation key go from n to m and the current time is t, use the value at (t-n) % (|m-n|). 

                //unsigned int numPositionKeys = channel->mNumPositionKeys; // The number of position keys.
                //unsigned int numRotationKeys = channel->mNumRotationKeys; // The number of rotation keys.
                //unsigned int numScalingKeys = channel->mNumScalingKeys; // The number of scaling keys.

                //aiVectorKey * positionKeys = channel->mPositionKeys; // The position keys of this animation channel.
                //aiQuatKey * rotationKeys = channel->mRotationKeys; // The rotation keys of this animation channel.
                //aiVectorKey * scalingKeys = channel->mScalingKeys; // The scaling keys of this animation channel. 
//#define interpolateLinear(P, A, B) ((P)*((B)-(A)) + (A))

                Timer &timer = EnginePlayer::getInstance().getTimer();
                double currentTicks = timer.getTimeInSeconds() / animation->mTicksPerSecond;
                double startTime = channel->mPositionKeys[0].mTime;
                double endTime = channel->mPositionKeys[channel->mNumPositionKeys - 1].mTime;
                bool skipAnimation = false;
                if (currentTicks < startTime) {
                    switch(channel->mPostState) {
                        case aiAnimBehaviour_LINEAR:
                            // FIXME: The value of the nearest two keys is linearly extrapolated for the current time value.
                        case aiAnimBehaviour_CONSTANT:
                            // The nearest key value is used without interpolation.
                            currentTicks = startTime;
                            break;
                        case aiAnimBehaviour_REPEAT:
                            // The animation is repeated. If the animation key go from n to m and the current time is t, use the value at (t-n) % (|m-n|). 
                            currentTicks = fmod(currentTicks, endTime);
                            break;
                        case aiAnimBehaviour_DEFAULT:
                        default:
                            // The value from the default node transformation is taken.
                            skipAnimation = true;
                            break;
                    }

                } else if (currentTicks > endTime) {
                    switch(channel->mPostState) {
                        case aiAnimBehaviour_LINEAR:
                            // FIXME: The value of the nearest two keys is linearly extrapolated for the current time value.
                        case aiAnimBehaviour_CONSTANT:
                            // The nearest key value is used without interpolation.
                            currentTicks = endTime;
                            break;
                        case aiAnimBehaviour_REPEAT:
                            // The animation is repeated. If the animation key go from n to m and the current time is t, use the value at (t-n) % (|m-n|). 
                            currentTicks = fmod(currentTicks, endTime);
                            break;
                        case aiAnimBehaviour_DEFAULT:
                        default:
                            // The value from the default node transformation is taken.
                            skipAnimation = true;
                            break;
                    }
                }

                for(unsigned int positionI = 0; positionI < channel->mNumPositionKeys && skipAnimation == false; positionI++) {
                    aiVectorKey* position1 = &channel->mPositionKeys[positionI];
                    aiVectorKey* position2 = &channel->mPositionKeys[positionI+1 < channel->mNumPositionKeys ? positionI+1 : positionI];

                    double percent = 0.0;
                    if (currentTicks >= position1->mTime && currentTicks <= position2->mTime) {
                        percent = (currentTicks - position1->mTime) / (position2->mTime - position1->mTime);
                    } else {
                        continue;
                    }

                    translate.x = interpolateLinear(percent, position1->mValue.x, position2->mValue.x);
                    translate.y = interpolateLinear(percent, position1->mValue.y, position2->mValue.y);
                    translate.z = interpolateLinear(percent, position1->mValue.z, position2->mValue.z);

                    break;
                }

                for(unsigned int scaleI = 0; scaleI < channel->mNumScalingKeys && skipAnimation == false; scaleI++) {
                    aiVectorKey* scale1 = &channel->mScalingKeys[scaleI];
                    aiVectorKey* scale2 = &channel->mScalingKeys[scaleI+1 < channel->mNumScalingKeys ? scaleI+1 : scaleI];

                    double percent = 0.0;
                    if (currentTicks >= scale1->mTime && currentTicks <= scale2->mTime) {
                        percent = (currentTicks - scale1->mTime) / (scale2->mTime - scale1->mTime);
                    } else {
                        continue;
                    }

                    scale.x = interpolateLinear(percent, scale1->mValue.x, scale2->mValue.x);
                    scale.y = interpolateLinear(percent, scale1->mValue.y, scale2->mValue.y);
                    scale.z = interpolateLinear(percent, scale1->mValue.z, scale2->mValue.z);

                    //loggerInfo("%.2f scale: x:%.2f, y:%.2f, z: %.2f", percent, scale.x, scale.y, scale.z);
                    break;
                }

                for(unsigned int rotateI = 0; rotateI < channel->mNumRotationKeys && skipAnimation == false; rotateI++) {
                    aiQuatKey* rotate1 = &channel->mRotationKeys[rotateI];
                    aiQuatKey* rotate2 = &channel->mRotationKeys[rotateI+1 < channel->mNumRotationKeys ? rotateI+1 : rotateI];

                    double percent = 0.0;
                    if (currentTicks >= rotate1->mTime && currentTicks <= rotate2->mTime) {
                        percent = (currentTicks - rotate1->mTime) / (rotate2->mTime - rotate1->mTime);
                    } else {
                        continue;
                    }


                    glm::quat quaternion1 = glm::quat(rotate1->mValue.w, rotate1->mValue.x, rotate1->mValue.y, rotate1->mValue.z);
                    glm::quat quaternion2 = glm::quat(rotate2->mValue.w, rotate2->mValue.x, rotate2->mValue.y, rotate2->mValue.z);

                    glm::quat quaternionDelta = glm::slerp(quaternion1, quaternion2, static_cast<float>(percent));

                    rotate.w = quaternionDelta.w;
                    rotate.x = quaternionDelta.x;
                    rotate.y = quaternionDelta.y;
                    rotate.z = quaternionDelta.z;

                    break;
                }
            }

        }
    }

    // apply node specific matrix transformations (also applicable for the children nodes)
    transformationMatrix.scale(scale.x, scale.y, scale.z); // TODO: Should scale be where?
    transformationMatrix.translate(translate.x, translate.y, translate.z);
    transformationMatrix.rotateQuaternion(rotate.w, rotate.x, rotate.y, rotate.z);

    // draw meshes related to the node
    for (unsigned int meshIndex = 0; meshIndex < node->mNumMeshes; meshIndex++) {
        Mesh* modelMesh = meshes[node->mMeshes[meshIndex]];
        modelMesh->draw();
    }

    // process children nodes
    for (unsigned int nodeI = 0; nodeI < node->mNumChildren; nodeI++) {
        drawNode(scene, node->mChildren[nodeI]);
    }

    transformationMatrix.pop();
}


bool ModelAssimp::load() {
    //File file = File("vitunufo2.3ds");
    //File file = File("dollar.obj");
    //File file = File("box.obj");

    loadLastModified = lastModified();

    if (!isFile()) {
        loggerError("Not a file. file:'%s'", getFilePath().c_str());
        return false;
    }

    if (!isSupported()) {
        loggerError("File type not supported. file:'%s'", getFilePath().c_str());
        return false;
    }

    /*if (!loadRaw()) {
        loggerError("Could not load file. file:'%s'", getFilePath().c_str());
        return false;
    }*/

    /*if (!file.load()) {
        loggerError("Could not load a file '%s'", file.getFilePath().c_str());
        return;
    }*/

    unsigned int severity = Assimp::Logger::Err;
    switch (Settings::logger.logLevel) {
        case LEVEL_TRACE:
            // Info and Debugging logging is really fine level, so TRACE is 
            severity = Assimp::Logger::Info | Assimp::Logger::Err | Assimp::Logger::Warn | Assimp::Logger::Debugging;
            break;
        default:
            severity = Assimp::Logger::Err | Assimp::Logger::Warn;
            break;

    }

    Assimp::DefaultLogger::create(NULL, Assimp::Logger::VERBOSE, 0, NULL);
    Assimp::DefaultLogger::get()->attachStream(new AssimpCustomLogStream(), severity);

    //importer.SetIOHandler( new AssimpCustomIOSystem());

    //Importer.ReadFileFromMemory is not supported if .OBJ has .MTL files...
    //TODO Recreate Assimp's DefaultIOSystem, DefaultIOStream to handle engine's EmbeddedResource class.
    //TODO Use Importer.SetIOHandler(...IOSystem) to set the thing...
    //TODO Enable logging: http://www.assimp.org/lib_html/usage.html
    //Post process flags: http://assimp.sourceforge.net/lib_html/postprocess_8h.html#a64795260b95f5a4b3f3dc1be4f52e410

    unsigned int pFlags = 0;
    if (Settings::demo.graphics.model.joinIdenticalVertices) {
        pFlags |= aiProcess_JoinIdenticalVertices;
    }
    if (Settings::demo.graphics.model.triangulate) {
        pFlags |= aiProcess_Triangulate;
    }
    if (Settings::demo.graphics.model.generateNormals) {
        pFlags |= aiProcess_GenSmoothNormals;
    }
    if (Settings::demo.graphics.model.validate) {
        pFlags |= aiProcess_ValidateDataStructure;
    }
    if (Settings::demo.graphics.model.removeRedundant) {
        pFlags |= aiProcess_RemoveRedundantMaterials;
    }
    if (Settings::demo.graphics.model.fixInvalidData) {
        pFlags |= aiProcess_FindInvalidData;
    }
    if (Settings::demo.graphics.model.optimizeMeshes) {
        pFlags |= aiProcess_OptimizeMeshes;
    }
    if (Settings::demo.graphics.model.optimizeGraph) {
        pFlags |= aiProcess_OptimizeGraph;
    }

    const aiScene* scene = importer.ReadFile(
        getFilePath().c_str(),
        pFlags);

    /*const aiScene* scene = importer.ReadFileFromMemory(
        static_cast<void*>(file.getData()), file.length(),
        aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);*/

    if (scene == NULL) {
        loggerError("Could not load file. file:'%s', error:%s", getFilePath().c_str(), importer.GetErrorString());
        return false;
    }

    // http://assimp.sourceforge.net/lib_html/structai_node.html

    clear();

    if (scene->HasMaterials()) {
        for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
            const aiMaterial* material = scene->mMaterials[i];
            handleMaterial(material);
        }
    }

    unsigned int faces = 0;
    if (scene->HasMeshes()) {
        for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
            const aiMesh* mesh = scene->mMeshes[i];
            faces += mesh->mNumFaces;

            handleMesh(scene, mesh);
        }
    } else {
        loggerWarning("No meshes found in the object, can't load. file:'%s'");
        return false;
    }

    if (scene->HasLights()) {
        for (unsigned int i = 0; i < scene->mNumLights; i++) {
            const aiLight* light = scene->mLights[i];
            handleLight(light);            
        }
    }

    if (scene->HasCameras()) {
        for (unsigned int i = 0; i < scene->mNumCameras; i++) {
            const aiCamera* camera = scene->mCameras[i];
            handleCamera(camera);            
        }
    }

    if (!scene->HasAnimations()) {
        loggerTrace("No animation data in the object. file:'%s'", getFilePath().c_str());
    }

    if (Settings::logger.logLevel < LEVEL_INFO) {
        // Print some fine information about the meshes
        for(Mesh* mesh : meshes) {
            mesh->print();
        }
    }

    loggerInfo("Loaded 3d object. file:'%s', totalFaces:%u, materials:%d, meshes:%u, lights:%u, cameras:%u, animations:%u, ptr:0x%p",
        getFilePath().c_str(), faces, scene->mNumMaterials, scene->mNumMeshes, scene->mNumLights, scene->mNumCameras, scene->mNumAnimations, this);

    return true;
}

static TextureType getTextureType(aiTextureType assimpTextureType) {
    switch(assimpTextureType) {
        case aiTextureType_NORMALS:
            return TextureType::NORMAL;
        case aiTextureType_SPECULAR:
            return TextureType::SPECULAR;
        case aiTextureType_AMBIENT:
            return TextureType::AMBIENT;
        case aiTextureType_DIFFUSE:
        default:
            return TextureType::DIFFUSE;
    }
}

bool ModelAssimp::handleMaterial(const aiMaterial* material) {
    const int supportedTextureCount = 4;
    static aiTextureType textureTypes[supportedTextureCount] = {
        aiTextureType_NORMALS,
        aiTextureType_DIFFUSE,
        aiTextureType_SPECULAR,
        aiTextureType_AMBIENT
    };


    Material* modelMaterial = new Material();
    if (modelMaterial == NULL) {
        loggerFatal("Could not allocate memory for material");
        return false;
    }

    //TODO: plenty of materials to support... http://assimp.sourceforge.net/lib_html/materials.html
    //for (unsigned int j = 0; j < material->mNumProperties; j++) {
    //    aiMaterialProperty *materialProperty = material->mProperties[j];
    //    loggerInfo("Plaa 3d object material property. file:'%s', material:%d, property:%d, key:%s", file.getFilePath().c_str(), i, j, materialProperty->mKey);
    //}

    aiString name;
    if (AI_SUCCESS == material->Get(AI_MATKEY_NAME,name)) {
        modelMaterial->setName(std::string(name.data));
    }

    aiColor3D color(0.0f,0.0f,0.0f);
    if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE,color)) {
        modelMaterial->setDiffuse(color.r, color.g, color.b);
    }
    if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_SPECULAR,color)) {
        modelMaterial->setSpecular(color.r, color.g, color.b);
    }
    if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_AMBIENT,color)) {
        modelMaterial->setAmbient(color.r, color.g, color.b);
    }

    /*float shininess = 0.0f;
    if (AI_SUCCESS == material->Get(AI_MATKEY_SHININESS,&shininess)) {
        modelMaterial->setShininess(shininess);
    }*/

    for(int t = 0; t < supportedTextureCount; t++) {
        aiTextureType type = textureTypes[t];
        if (material->GetTextureCount(type) > 0) {
            aiString path;

            //TODO: convert paths and texture names, if needed (ie: "C:\random\directory\model_tex.JPG" -> "model_tex.png")
            if (material->GetTexture(type, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
                Texture *texture = NULL;

                std::string filePath = std::string(path.data);
                Image *image = MemoryManager<Image>::getInstance().getResource(filePath);
                if (image) {
                    if (!image->isLoaded() || image->modified()) {
                        if (!image->load()) {
                            loggerWarning("Image not loaded, texture not added to material. file:'%s', path:'%s'", getFilePath().c_str(), filePath.c_str());
                        }
                    }

                    texture = image->getTexture();
                } else {
                    std::size_t nameIndex = filePath.find_first_of(".");
                    if (nameIndex != std::string::npos && filePath.substr(nameIndex) == ".color.fbo") {
                        std::string fboName = filePath.substr(0, nameIndex);

                        MemoryManager<Fbo>& fboMemory = MemoryManager<Fbo>::getInstance();
                        Fbo *fbo = fboMemory.getResource(fboName);


                        if (fbo) {
                            if (fbo->getColorTexture() == NULL) {
                                fbo->generate();
                            }

                            texture = fbo->getColorTexture();
                        }
                    }
                }

                if (texture) {
                    texture->setType(getTextureType(type));
                    modelMaterial->setTexture(texture, static_cast<int>(getTextureType(type)));

                    loggerDebug("Attempting to add texture unit: %s (0x%p), unit:%d", filePath.c_str(), texture, static_cast<int>(getTextureType(type)));
                } else {
                    loggerWarning("Image not found, texture not added to material. file:'%s', path:'%s'", getFilePath().c_str(), filePath.c_str());
                }
            } else {
                loggerDebug("Type %d has textures but no texture could be loaded. file:'%s'", type, getFilePath().c_str());
            }
        } else {
            aiString path;
            if (material->GetTexture(type, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
                Texture *texture = NULL;

                std::string filePath = std::string(path.data);
                loggerDebug("Type %d not supported. Will not load image: '%s'. file:'%s'", type, filePath.c_str(), getFilePath().c_str());
            }
        }
    }

    loggerDebug("Material: %s, file:%s", modelMaterial->toString().c_str(), getFilePath().c_str());
    addMaterial(modelMaterial);

    return true;
}

bool ModelAssimp::handleMesh(const aiScene* scene, const aiMesh* mesh) {
    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    //m_Entries[Index].MaterialIndex = paiMesh->mMaterialIndex;
    Mesh *modelMesh = new Mesh();
    if (modelMesh == NULL) {
        loggerFatal("Could not allocate memory for mesh!");
        return false;
    }
    modelMesh->setName(std::string(mesh->mName.data));

    //if (scene->mNumMaterial)
    if (scene->HasMaterials()) {
        modelMesh->setMaterial(getMaterial(mesh->mMaterialIndex));
    }

    for (unsigned int i = 0; i < mesh->mNumVertices ; i++) {
        const aiVector3D* pPos      = &(mesh->mVertices[i]);
        const aiVector3D* pNormal   = &(mesh->mNormals[i]);
        const aiVector3D* pTexCoord = mesh->HasTextureCoords(0) ? &(mesh->mTextureCoords[0][i]) : &Zero3D;

        modelMesh->addVertex(pPos->x, pPos->y, pPos->z);
        if (mesh->HasNormals()) {
            modelMesh->addNormal(pNormal->x, pNormal->y, pNormal->z);
        }
        modelMesh->addTexCoord(pTexCoord->x, pTexCoord->y);
        /*loggerTrace("%u/%u: v: x:%.2f y:%.2f z:%2.f - vt: u:%.2f v:%.2f - vn: x:%.2f y:%.2f z:%2.f",
            i, mesh->mNumVertices,
            pPos->x, pPos->y, pPos->z,
            pTexCoord->x, pTexCoord->y,
            pNormal->x, pNormal->y, pNormal->z);*/
    }

    unsigned int origNumIndices = 0;
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        //FIXME: addIndex should take only one parameter
        const aiFace& face = mesh->mFaces[i];
        for (unsigned int faceIndex = 0; faceIndex < face.mNumIndices; faceIndex++) {
            modelMesh->addIndex(face.mIndices[faceIndex]);
        }
        if (origNumIndices > 0 && origNumIndices != face.mNumIndices) {
            loggerInfo("Face indice count varying. face:%u/%u, expected:%u, actual:%u", i, mesh->mNumFaces, origNumIndices, face.mNumIndices);
        }
        /*
        // [00:00.000] LOAD a9e48eb7 ModelAssimp.cpp:handleMesh():590 WARNING: 3d object mesh face indice count not supported. file:'data - sicherheit/tupolev/tupolev.obj', meshName:'Cylinder.029_Cylinder.046', mesh:721, face:721, indices:721
        if (face.mNumIndices != 3) {
            loggerWarning("3d object mesh face indice count not supported. file:'%s', meshName:'%s', mesh:%d, face:%d, indices:%d",
                getFilePath().c_str(), mesh->mName.data,
                i,i,i);
            continue;
        }*/
        origNumIndices = face.mNumIndices;
    }
    /*if (origNumIndices != 3) {
        loggerInfo("Face has %u indices. Not a triangle? faces:%u, file:'%s'", origNumIndices, mesh->mNumFaces, getFilePath().c_str());
    }*/

    std::string materialName = "NULL";
    if (modelMesh->getMaterial()) {
        materialName = modelMesh->getMaterial()->getName();
    }
    if (modelMesh->generate()) {
        loggerDebug("Generated mesh. file:'%s', meshName:'%s', faces:%u, material:'%s'", getFilePath().c_str(), mesh->mName.data, mesh->mNumFaces, materialName.c_str());
        addMesh(modelMesh);
    } else {
        loggerWarning("Could not generate mesh. file:'%s', meshName:'%s', material:'%s'", getFilePath().c_str(), mesh->mName.data, materialName.c_str());
        return false;
    }

    return true;
}

bool ModelAssimp::handleCamera(const aiCamera* camera) {
    // ref: http://assimp.sourceforge.net/lib_html/structai_camera.html

    //FIXME: How to use
    Camera cam;
    //Camera& cam = EnginePlayer::getInstance().getActiveCamera();
    cam.setName(std::string(camera->mName.data));
    cam.setAspectRatio(static_cast<double>(camera->mAspect));
    cam.setClipPlaneNear(static_cast<double>(camera->mClipPlaneNear));
    cam.setClipPlaneFar(static_cast<double>(camera->mClipPlaneFar));
    // Half horizontal field of view angle, in radians.
    cam.setHorizontalFov(static_cast<double>(camera->mHorizontalFOV));

    // 'LookAt' - vector of the camera coordinate system relative to the coordinate space defined by the corresponding node.
    aiVector3D lookAt = camera->mLookAt; 
    cam.setLookAt(lookAt.x, lookAt.y, lookAt.z);

    // Position of the camera relative to the coordinate space defined by the corresponding node.
    aiVector3D position = camera->mPosition; 
    cam.setPosition(position.x, position.y, position.z);

    // 'Up' - vector of the camera coordinate system relative to the coordinate space defined by the corresponding node. 
    aiVector3D up = camera->mUp; 
    cam.setUp(up.x, up.y, up.z);

    //FIXME: Need to
    //loggerDebug("FIXME: Camera not supported. %s", cam.toString().c_str());
    loggerInfo("Static 3D model camera: %s", cam.toString().c_str());

    return true;
}

bool ModelAssimp::handleLight(const aiLight* light) {
    // ref: http://assimp.sourceforge.net/lib_html/structai_light.html

    // FIXME: handling needed for cone and attenuation
    // float angleInnerCone = light->mAngleInnerCone; // Inner angle of a spot light's light cone.
    // float angleOuterCone = light->mAngleOuterCone; // Outer angle of a spot light's light cone.
    // float attenuationConstant = light->mAttenuationConstant; // Constant light attenuation factor.
    // float attenuationLinear = light->mAttenuationLinear; // Linear light attenuation factor.
    // float attenuationQuadratic = light->mAttenuationQuadratic; // Quadratic light attenuation factor.

    Light l;
    l.setName(std::string(light->mName.data));

    l.setAmbient(light->mColorAmbient.r, light->mColorAmbient.g, light->mColorAmbient.b, 1.0);
    l.setDiffuse(light->mColorDiffuse.r, light->mColorDiffuse.g, light->mColorDiffuse.b, 1.0);
    l.setSpecular(light->mColorSpecular.r, light->mColorSpecular.g, light->mColorSpecular.b, 1.0);

    l.setPosition(light->mPosition.x, light->mPosition.y, light->mPosition.z);
    l.setDirection(light->mDirection.x, light->mDirection.y, light->mDirection.z);

    LightType type;
    switch(light->mType) {
        case aiLightSource_DIRECTIONAL:
            type = LightType::DIRECTIONAL;
            break;
        case aiLightSource_POINT:
            type = LightType::POINT;
            break;
        case aiLightSource_SPOT:
            type = LightType::SPOT;
            break;
        case aiLightSource_UNDEFINED:
        default:
            loggerWarning("Light type undefined! lightName:%s, file:'%s'", l.getName().c_str(), getFilePath().c_str());
            return false;
    }
    l.setType(type);

    loggerInfo("Light handled: %s", l.toString().c_str());

    return true;
}

void ModelAssimp::clear() {
    for (Mesh* mesh : meshes) {
        if (mesh) {
            delete mesh;
        }
    }
    meshes.clear();

    for (Material* material : materials) {
        if (material) {
            delete material;
        }
    }
    materials.clear();
}
