#include "ShaderProgramOpenGl.h"
#include "ShaderOpenGl.h"
#include "Graphics.h"
#include "Settings.h"
#include "Camera.h"

#include "EnginePlayer.h"
#include "time/Timer.h"
#include "time/Date.h"
#include "time/TimeFormatter.h"
#include "logger/logger.h"
#include "io/MemoryManager.h"
#include "math/TransformationMatrix.h"
#include "audio/Audio.h"
#include "sync/Sync.h"
#include "graphics/LightManager.h"
#include "graphics/Shadow.h"
#include "graphics/model/Material.h"

#include <stdlib.h>
#include <string.h>

#include <string>
#include <regex>
#include <type_traits>

std::vector<ShaderProgramOpenGl*> ShaderProgramOpenGl::bindStack = {};
ShaderProgramOpenGl* ShaderProgramOpenGl::shaderProgramDefault = NULL;
ShaderProgramOpenGl* ShaderProgramOpenGl::shaderProgramDefaultShadow = NULL;

ShaderProgram *ShaderProgram::newInstance(std::string name) {
    return new ShaderProgramOpenGl(name);
}

GLuint ShaderProgramOpenGl::getCurrentBindId() {
    GLuint currentBindId = 0;
    if (! bindStack.empty()) {
        currentBindId = bindStack.back()->getId();
    } else if (shaderProgramDefault) {
        currentBindId = shaderProgramDefault->getId();
    }

    return currentBindId;
}

void ShaderProgram::useCurrentBind() {
    ShaderProgramOpenGl::useCurrentBind();    
}

void ShaderProgramOpenGl::useCurrentBind() {
    ShaderProgramOpenGl *shaderProgram = shaderProgramDefault;
    if (!bindStack.empty()) {
        shaderProgram = bindStack.back();
    }

    if (!shaderProgram) {
        loggerWarning("Shader program is empty");
        return;
    }

    //loggerTrace("Using current bind. program:'%s', programId:%d", shaderProgram->getName().c_str(), shaderProgram->getId());

    glUseProgram(shaderProgram->getId());
    shaderProgram->assignUniforms();
}


ShaderProgramOpenGl::ShaderProgramOpenGl(std::string name) : ShaderProgram(name) {
    id = 0;
    linked = false;

    uniforms = std::map<std::string, std::function<void()>>();

}

ShaderProgramOpenGl::~ShaderProgramOpenGl() {
    free();

    for(ShaderOpenGl *shader : shaders) {
        shader->removeShaderProgram(this);
    }
    shaders.clear();
}

void ShaderProgramOpenGl::free() {
    PROFILER_BLOCK("ShaderProgramOpenGl::free");

    if (id != 0) {
        loggerDebug("Freeing shader program. program:'%s', programId:%d", getName().c_str(), id);

        if (shaderProgramDefault == this) {
            shaderProgramDefault = NULL;
        }
        if (shaderProgramDefaultShadow == this) {
            shaderProgramDefaultShadow = NULL;
        }

        if (getCurrentBindId() == id && id != 0) {
            unbind();
        }

        glDeleteProgram(id);

        Graphics &graphics = Graphics::getInstance();
        if (graphics.handleErrors()) {
            loggerError("Could not free shader program. program:'%s'", getName().c_str());
        }

        id = 0;
    }
}

bool ShaderProgramOpenGl::generate() {
    PROFILER_BLOCK("ShaderProgramOpenGl::generate");

    free();
    id = glCreateProgram();
    if (id == 0) {
        Graphics &graphics = Graphics::getInstance();
        graphics.handleErrors();
        loggerError("Could not create shader program. program:'%s'", getName().c_str());
        return false;
    }

    return true;
}

bool ShaderProgramOpenGl::addShader(Shader *shader) {
    if (shader == NULL) {
        loggerError("Can't add non-compiled shader. program:'%s', shader:'0x%p'", getName().c_str(), shader);
        return false;
    }

    shader->addShaderProgram(this);
    shaders.push_back(dynamic_cast<ShaderOpenGl*>(shader));
    return true;
}

bool ShaderProgramOpenGl::attach() {
    PROFILER_BLOCK("ShaderProgramOpenGl::attach");

    if (id == 0) {
        loggerError("Program ID invalid. program:'%s', programId:%d", getName().c_str(), id);
        return false;
    }

    //TODO: Kinda hack to determine that vertex shader is missing and needs to be added (==legacy compatibility)
    bool hasVertexShader = false;
    bool hasFragmentShader = false;

    for(ShaderOpenGl *shader : shaders) {
        GLenum type = shader->determineShaderType();
        if (type == GL_VERTEX_SHADER) {
            hasVertexShader = true;
        }
        if (type == GL_FRAGMENT_SHADER) {
            hasFragmentShader = true;
        }
    }
    
    if (! hasVertexShader) {
        Shader *defaultVs = MemoryManager<Shader>::getInstance().getResource(std::string("_embedded/default.vs"), true);
        addShader(defaultVs);
    }
    if (! hasFragmentShader) {
        Shader *defaultFs = MemoryManager<Shader>::getInstance().getResource(std::string("_embedded/default.fs"), true);
        addShader(defaultFs);
    }

    bool attachSuccess = true;
    for(ShaderOpenGl *shader : shaders) {
        if (shader == NULL || shader->getId() == 0) {
            GLuint shaderId = 0;
            const char *shaderFile = "N/A";
            if (shader) {
                shaderId = shader->getId();
                shaderFile = shader->getFilePath().c_str();
            }
            loggerError("Can't attach shader. program:'%s', shaderFile:'%s' shader:'0x%p', shaderId:%u", getName().c_str(), shaderFile, shader, shaderId);
            attachSuccess = false;
            continue;
        }

        glAttachShader(id, shader->getId());
        Graphics &graphics = Graphics::getInstance();
        if (graphics.handleErrors()) {
            loggerError("Could not attach shader to program. program:'%s', shader:'%s'", getName().c_str(), shader->getFilePath().c_str());
            attachSuccess = false;
            continue;
        }

        loggerTrace("Attached shader to program. program:'%s', shader:'%s'", getName().c_str(), shader->getFilePath().c_str());
    }

    GLint attachedShaders = 0;
    glGetProgramiv(id, GL_ATTACHED_SHADERS, &attachedShaders);
    if (attachedShaders != static_cast<GLint>(shaders.size())) {
        loggerWarning("Program expected to have %d attached shaders but only %d were attached. program:'%s', programId:%d",
            shaders.size(), attachedShaders, getName().c_str(), id);
        attachSuccess = false;
    }

    return attachSuccess;
}

bool ShaderProgramOpenGl::detach() {
    PROFILER_BLOCK("ShaderProgramOpenGl::detach");

    if (id == 0) {
        loggerError("Program ID invalid. program:'%s'", getName().c_str());
        return false;
    }

    bool detachSuccess = true;

    for(ShaderOpenGl *shader : shaders) {
        if (shader == NULL || shader->getId() == 0) {
            loggerError("Can't detach shader. program:'%s', shader:'0x%p'", getName().c_str(), shader);
            detachSuccess = false;
            continue;
        }

        glDetachShader(id, shader->getId());
        Graphics &graphics = Graphics::getInstance();
        if (graphics.handleErrors()) {
            loggerError("Could not detach shader from program. program:'%s', shader:'%s'", getName().c_str(), shader->getFilePath().c_str());
            detachSuccess = false;
            continue;
        }

        loggerTrace("Detached shader from program. program:'%s', shader:'%s'", getName().c_str(), shader->getFilePath().c_str());
    }

    return detachSuccess;
}

bool ShaderProgramOpenGl::link() {
    PROFILER_BLOCK("ShaderProgramOpenGl::link");

    linked = false;

    bool currentlyInUse = false;
    if (getCurrentBindId() == id && id != 0) {
        currentlyInUse = true;
        unbind();
    }

    // FIXME: implement defensive way, where non-linking shader program won't override previous shader program
    if (!generate()) {
        return false;
    }

    if (!attach()) {
        return false;
    }

    glLinkProgram(id);
    linked = checkLinkStatus();

    detach();

    if (! linked) {
        return linked;
    }

    loggerInfo("Linked program. program:'%s', programId:%d, shaders:%d", getName().c_str(), id, shaders.size());

    determineUniforms();

    if (shaderProgramDefault != this && Settings::demo.graphics.shaderProgramDefault == getName()) {
        shaderProgramDefault = this;
    }

    if (shaderProgramDefaultShadow != this && Settings::demo.graphics.shaderProgramDefaultShadow == getName()) {
        shaderProgramDefaultShadow = this;
    }

    if (currentlyInUse) {
        bind();
    }

    return linked;
}

bool ShaderProgramOpenGl::containsUniform(std::string uniformKey) {
    PROFILER_BLOCK("ShaderProgramOpenGl::containsUniform");

    GLint uniformCount = 0;
    glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &uniformCount);
    for (GLuint i = 0; i < static_cast<GLuint>(uniformCount); i++) {
        GLsizei length = 0;
        GLint size = 0;
        GLenum type = 0;
        const GLsizei bufSize = 128;

        GLchar *tmpname = new GLchar [bufSize];
        glGetActiveUniform(getId(), i, bufSize, &length, &size, &type, tmpname);
        std::string name = std::string(tmpname);
        delete [] tmpname;
        if (name == uniformKey) {
            return true;
        }
    }

    return false;
}

enum class UniformType {
    DOUBLE_MAT4,
    DOUBLE_MAT3,
    TEXTURE,
    DOUBLE_VEC4,
    DOUBLE_VEC3,
    DOUBLE_VEC2,
    DOUBLE,
    INTEGER,
    UNKNOWN
};

static UniformType getUniformType(GLenum type) {
    switch(type) {
        case GL_SAMPLER_1D:
        case GL_SAMPLER_2D:
        case GL_SAMPLER_3D:
            return UniformType::TEXTURE;
        case GL_FLOAT_MAT4:
        case GL_DOUBLE_MAT4:
            return UniformType::DOUBLE_MAT4;
        case GL_FLOAT_MAT3:
        case GL_DOUBLE_MAT3:
            return UniformType::DOUBLE_MAT3;
        case GL_FLOAT_VEC4:
        case GL_DOUBLE_VEC4:
            return UniformType::DOUBLE_VEC4;
        case GL_FLOAT_VEC3:
        case GL_DOUBLE_VEC3:
            return UniformType::DOUBLE_VEC3;
        case GL_FLOAT_VEC2:
        case GL_DOUBLE_VEC2:
            return UniformType::DOUBLE_VEC2;
        case GL_FLOAT:
        case GL_DOUBLE:
            return UniformType::DOUBLE;
        case GL_INT:
            return UniformType::INTEGER;
        default:
            return UniformType::UNKNOWN;
    }
}

// TODO: Add support for binding all shadertoy.com supported uniforms
// TODO: support rendering audio stream (f.e. shadertoy.com)
void ShaderProgramOpenGl::determineUniforms() {
    PROFILER_BLOCK("ShaderProgramOpenGl::determineUniforms");

    if (!isLinked()) {
        loggerError("Shader must be linked in order to determine uniforms! name:'%s'", getName().c_str());
        return;
    }

    std::smatch regexMatch;
    static const std::regex lightRegex("light\\[([0-3])\\]\\.([\\w]+)");
    static const std::regex cameraRegex("camera\\.([\\w]+)");
    static const std::regex materialRegex("material\\.([\\w]+)");

    const GLsizei bufSize = 256;
    GLchar *tmpname = new GLchar [bufSize];

    GLint uniformCount = 0;
    glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &uniformCount);
    for (GLuint i = 0; i < static_cast<GLuint>(uniformCount); i++) {
        GLsizei length = 0;
        GLint size = 0;
        GLenum glType = 0;

        glGetActiveUniform(getId(), i, bufSize, &length, &size, &glType, tmpname);
        std::string name = std::string(tmpname);

        UniformType type = getUniformType(glType);
        if (type == UniformType::UNKNOWN) {
            loggerTrace("Shader program '%s' uniform:'%s' type(%u) unknown, not determined", getName().c_str(), name.c_str(), glType);
            continue;
        }

        if (length < bufSize) {
            //GLint uniformId = glGetUniformLocation(getId(), name.c_str());
            //loggerTrace("Shader program '%s' has uniform:'%s'", getName().c_str(), name.c_str());

            if (std::regex_match(name, regexMatch, lightRegex)) {
                int lightIndex = atoi(regexMatch[1].str().c_str());
                std::string variable = regexMatch[2].str();

                if (static_cast<unsigned int>(lightIndex) < LightManager::getInstance().getActiveLightCount()) {
                    if (variable == "diffuse") {
                        setUniformFunction4fv(name, [lightIndex]() {
                            const Color &color = LightManager::getInstance().getLight(lightIndex).getDiffuse();
                            return std::array<float, 4>{
                                static_cast<float>(color.r),
                                static_cast<float>(color.g),
                                static_cast<float>(color.b),
                                static_cast<float>(color.a)
                            };
                        });
                    }
                    else if (variable == "ambient") {
                        setUniformFunction4fv(name, [lightIndex]() {
                            const Color &color = LightManager::getInstance().getLight(lightIndex).getAmbient();
                            return std::array<float, 4>{
                                static_cast<float>(color.r),
                                static_cast<float>(color.g),
                                static_cast<float>(color.b),
                                static_cast<float>(color.a)
                            };
                        });
                    }
                    else if (variable == "specular") {
                        setUniformFunction4fv(name, [lightIndex]() {
                            const Color &color = LightManager::getInstance().getLight(lightIndex).getSpecular();
                            return std::array<float, 4>{
                                static_cast<float>(color.r),
                                static_cast<float>(color.g),
                                static_cast<float>(color.b),
                                static_cast<float>(color.a)
                            };
                        });
                    }
                    else if (variable == "position") {
                        setUniformFunction3fv(name, [lightIndex]() {
                            const Vector3 &position = LightManager::getInstance().getLight(lightIndex).getPosition();
                            return std::array<float, 3>{
                                static_cast<float>(position.x),
                                static_cast<float>(position.y),
                                static_cast<float>(position.z)
                            };
                        });
                    }
                    else if (variable == "direction") {
                        setUniformFunction3fv(name, [lightIndex]() {
                            const Vector3 &direction = LightManager::getInstance().getLight(lightIndex).getDirection();
                            return std::array<float, 3>{
                                static_cast<float>(direction.x),
                                static_cast<float>(direction.y),
                                static_cast<float>(direction.z)
                            };
                        });
                    }
                    else if (variable == "type") {
                        setUniformFunction1i(name, [lightIndex]() {
                            return static_cast<std::underlying_type<LightType>::type>(LightManager::getInstance().getLight(lightIndex).getType());
                        });
                    }
                }
            }
            else if (std::regex_match(name, regexMatch, materialRegex)) {
                std::string variable = regexMatch[1].str();
                if (variable == "diffuse") {
                    setUniformFunction4fv(name, []() {
                        Material* material = Material::getCurrentMaterial();
                        Color color;
                        if (material) {
                            color = material->getDiffuse();
                        }

                        return std::array<float, 4>{
                            static_cast<float>(color.r),
                            static_cast<float>(color.g),
                            static_cast<float>(color.b),
                            static_cast<float>(color.a)
                        };
                    });
                }
                else if (variable == "ambient") {
                    setUniformFunction4fv(name, []() {
                        Material* material = Material::getCurrentMaterial();
                        Color color;
                        if (material) {
                            color = material->getAmbient();
                        }

                        return std::array<float, 4>{
                            static_cast<float>(color.r),
                            static_cast<float>(color.g),
                            static_cast<float>(color.b),
                            static_cast<float>(color.a)
                        };
                    });
                }
                else if (variable == "specular") {
                    setUniformFunction4fv(name, []() {
                        Material* material = Material::getCurrentMaterial();
                        Color color;
                        if (material) {
                            color = material->getSpecular();
                        }

                        return std::array<float, 4>{
                            static_cast<float>(color.r),
                            static_cast<float>(color.g),
                            static_cast<float>(color.b),
                            static_cast<float>(color.a)
                        };
                    });
                }
            }
            else if (std::regex_match(name, regexMatch, cameraRegex)) {
                std::string variable = regexMatch[1].str();

                if (variable == "position") {
                    setUniformFunction3fv(name, []() {
                        Camera& camera = EnginePlayer::getInstance().getActiveCamera();
                        const Vector3& position = camera.getPosition();
                        return std::array<float, 3>{
                            static_cast<float>(position.x),
                            static_cast<float>(position.y),
                            static_cast<float>(position.z)
                        };
                    });
                }
                else if (variable == "lookAt") {
                    setUniformFunction3fv(name, []() {
                        Camera& camera = EnginePlayer::getInstance().getActiveCamera();
                        const Vector3& lookAt = camera.getLookAt();
                        return std::array<float, 3>{
                            static_cast<float>(lookAt.x),
                            static_cast<float>(lookAt.y),
                            static_cast<float>(lookAt.z)
                        };
                    });
                }
            }
            else if (type == UniformType::TEXTURE) {
                // Sampler for input textures i

                static const std::regex textureRegex("(texture|iChannel|shadow)([0-9]+)");
                if (std::regex_match(name, regexMatch, textureRegex)) {
                    int textureNumber = atoi(regexMatch[2].str().c_str());
                    setUniformFunction1i(name, [textureNumber]() {
                        return textureNumber;
                    });
                }
            } else if (type == UniformType::DOUBLE_MAT4) {
                // TODO: Encapsulate OpenGL implementation to a generic form...
                GLint uniformId = glGetUniformLocation(getId(), name.c_str());
                if (name == "mvp") {
                    setUniformFunction(name, uniformId, [uniformId]() {
                        glUniformMatrix4fv(uniformId, 1, GL_FALSE, TransformationMatrix::getInstance().getMvp());
                    });
                } else if (name == "projection") {
                    setUniformFunction(name, uniformId, [uniformId]() {
                        glUniformMatrix4fv(uniformId, 1, GL_FALSE, TransformationMatrix::getInstance().getProjectionMatrix());
                    });
                } else if (name == "model") {
                    setUniformFunction(name, uniformId, [uniformId]() {
                        glUniformMatrix4fv(uniformId, 1, GL_FALSE, TransformationMatrix::getInstance().getModelMatrix());
                    });
                } else if (name == "view") {
                    setUniformFunction(name, uniformId, [uniformId]() {
                        glUniformMatrix4fv(uniformId, 1, GL_FALSE, TransformationMatrix::getInstance().getViewMatrix());
                    });
                } else if (name == "shadowMvp") {
                    setUniformFunction(name, uniformId, [uniformId]() {
                        //glUniformMatrix4fv(uniformId, 1, GL_FALSE, TransformationMatrix::getInstance().getMvp());
                        glUniformMatrix4fv(uniformId, 1, GL_FALSE, EnginePlayer::getInstance().getShadow().getMvp());
                    });
                }
            } else if (type == UniformType::DOUBLE_MAT3) {
                // TODO: Encapsulate OpenGL implementation to a generic form...
                GLint uniformId = glGetUniformLocation(getId(), name.c_str());
                if (name == "normalMatrix") {
                    setUniformFunction(name, uniformId, [uniformId]() {
                        glUniformMatrix3fv(uniformId, 1, GL_FALSE, TransformationMatrix::getInstance().getNormalMatrix());
                    });
                } 
            } else if (type == UniformType::DOUBLE_VEC4) {
                if (name == "color") {
                    setUniformFunction4fv(name, []() {
                        Color& color = Graphics::getInstance().getColor();
                        return std::array<float, 4>{
                            static_cast<float>(color.r),
                            static_cast<float>(color.g),
                            static_cast<float>(color.b),
                            static_cast<float>(color.a)
                        };
                    });
                }
                // Year, month, day, time in seconds in .xyzw
                else if (name == "iDate") {
                    setUniformFunction4fv(name, []() {
                        static TimeFormatter yearFormatter = TimeFormatter("yyyy");
                        static TimeFormatter monthFormatter = TimeFormatter("MM");
                        static TimeFormatter dayFormatter = TimeFormatter("dd");
                        static TimeFormatter hourFormatter = TimeFormatter("HH");
                        static TimeFormatter minuteFormatter = TimeFormatter("mm");
                        static TimeFormatter secondFormatter = TimeFormatter("ss");

                        Date now;
                        float year = static_cast<float>(std::stoi(yearFormatter.format(now)));
                        float month = static_cast<float>(std::stoi(monthFormatter.format(now)));
                        float day = static_cast<float>(std::stoi(dayFormatter.format(now)));
                        float hour = static_cast<float>(std::stoi(hourFormatter.format(now)));
                        float minute = static_cast<float>(std::stoi(minuteFormatter.format(now)));
                        float second = static_cast<float>(std::stoi(secondFormatter.format(now)));
                        float timeInSeconds = static_cast<float>(hour*60*60 + minute*60 + second);

                        return std::array<float, 4>{year, month, day, timeInSeconds};
                    });
                }
                // TODO: vec4    iMouse    image/buffer    xy = current pixel coords (if LMB is down). zw = click pixel
                else if (name == "iMouse") {
                    loggerWarning("%s auto-binding not currently supported", name.c_str());
                }
            } else if (type == UniformType::DOUBLE_VEC3) {
                // TODO: vec3    iChannelResolution[4]    image/buffer/sound    Input texture resolution for each channel
                static const std::regex channelResolutionRegex("(iChannelResolution)([0-3])");
                if (std::regex_match(name, regexMatch, channelResolutionRegex)) {
                    int channelResolutionNumber = atoi(regexMatch[2].str().c_str());
                    /*uniforms[name] = [uniformId, channelResolutionNumber]() {
                        glUniform1i(uniformId, channelResolutionNumber);
                    };*/
                    loggerWarning("%s auto-binding not currently supported", name.c_str());
                }
                // The viewport resolution (z is pixel aspect ratio, usually 1.0)
                else if (name == "iResolution") {
                    setUniformFunction3fv(name, []() {
                        return std::array<float, 3>{Settings::demo.graphics.canvasWidth, Settings::demo.graphics.canvasHeight, Settings::demo.graphics.aspectRatio};
                    });
                }
            } else if (type == UniformType::DOUBLE) {
                // Current time in seconds
                if (name == "time" || name == "iTime") {
                    setUniformFunction1f(name, []() {
                        return EnginePlayer::getInstance().getTimer().getTimeInSeconds();
                    });
                }
                // The sound sample rate, for example, 44100
                else if (name == "iSampleRate") {
                    setUniformFunction1f(name, []() {
                        return EnginePlayer::getInstance().getAudio().getSampleRate();
                    });
                }
                // Time it takes to render a frame, in seconds
                else if (name == "iTimeDelta") {
                    setUniformFunction1f(name, []() {
                        return EnginePlayer::getInstance().getFps().getCurrentRenderTime();
                    });
                }
                // Number of frames rendered per second
                else if (name == "iFrameRate") {
                    setUniformFunction1f(name, []() {
                        return EnginePlayer::getInstance().getFps().getFps();
                    });
                }
                else {
                    // Sync / GNU Rocket variable auto-binding
                    Sync& sync = Sync::getInstance();

                    std::string uniformName = name;
                    std::string syncName = name;
                    std::size_t tabSplitIndex = syncName.find_first_of("_");
                    if (tabSplitIndex != std::string::npos) {
                        //rocket tab tracks have names like "tab1:var". GLSL variables do not support ":", so "_" will be used as replacement.
                        std::string syncTabName = name.substr(0, tabSplitIndex) + std::string(":") + name.substr(tabSplitIndex + 1);

                        if (sync.containsVariable(syncTabName.c_str())) {
                            syncName = syncTabName;
                        }
                    }

                    if (sync.containsVariable(syncName.c_str())) {
                        std::string trackName = syncName;
                        setUniformFunction1f(uniformName, [trackName]() {
                            return EnginePlayer::getInstance().getSync().getVariableCurrentValue(trackName.c_str());
                        });
                    }
                }
            } else if (type == UniformType::INTEGER) {
                // Current frame
                if (name == "iFrame") {
                    setUniformFunction1i(name, []() {
                        return static_cast<int>(EnginePlayer::getInstance().getFps().getTotalFrameCount());
                    });
                }
                else if (name == "activeLightCount") {
                    setUniformFunction1i(name, []() {
                        return LightManager::getInstance().getActiveLightCount();
                    });
                }
            }
        } else {
            loggerWarning("Shader program '%s' uniform:'%s...' name oversized (%d/%d)", getName().c_str(), name.c_str(), bufSize, length);
        }
    }

    delete [] tmpname;

}

bool ShaderProgramOpenGl::setUniformFunction1f(std::string uniformName, std::function<float()> function) {
    GLint uniformId = glGetUniformLocation(getId(), uniformName.c_str());
    return setUniformFunction(uniformName, uniformId, [uniformId, function]() {
        glUniform1f(uniformId, static_cast<GLfloat>(function()));
    });
}

bool ShaderProgramOpenGl::setUniformFunction2fv(std::string uniformName, std::function<std::array<float, 2>()> function) {
    GLint uniformId = glGetUniformLocation(getId(), uniformName.c_str());
    return setUniformFunction(uniformName, uniformId, [uniformId, function]() {
        glUniform2fv(uniformId, 1, static_cast<GLfloat*>(function().data()));
    });
}

bool ShaderProgramOpenGl::setUniformFunction3fv(std::string uniformName, std::function<std::array<float, 3>()> function) {
    GLint uniformId = glGetUniformLocation(getId(), uniformName.c_str());
    return setUniformFunction(uniformName, uniformId, [uniformId, function]() {
        glUniform3fv(uniformId, 1, static_cast<GLfloat*>(function().data()));
    });
}

bool ShaderProgramOpenGl::setUniformFunction4fv(std::string uniformName, std::function<std::array<float, 4>()> function) {
    GLint uniformId = glGetUniformLocation(getId(), uniformName.c_str());
    return setUniformFunction(uniformName, uniformId, [uniformId, function]() {
        glUniform4fv(uniformId, 1, static_cast<GLfloat*>(function().data()));
    });
}

bool ShaderProgramOpenGl::setUniformFunction1i(std::string uniformName, std::function<int()> function) {
    GLint uniformId = glGetUniformLocation(getId(), uniformName.c_str());
    return setUniformFunction(uniformName, uniformId, [uniformId, function]() {
        glUniform1i(uniformId, static_cast<GLint>(function()));
    });
}

bool ShaderProgramOpenGl::setUniformFunction(std::string uniformName, GLint uniformId, std::function<void()> function) {
    if (uniformId == -1) {
        loggerWarning("Uniform doesn't exist! uniformName:'%s', name:'%s'", uniformName.c_str(), getName().c_str());
        return false;
    }
    uniforms[uniformName] = function;

    if (function) {
        loggerTrace("Shader program '%s' uniform:'%s' auto-determined!", getName().c_str(), uniformName.c_str());
    } else {
        loggerTrace("Shader program '%s' uniform:'%s' auto-determination removed!", getName().c_str(), uniformName.c_str());
    }

    return true;
}

void ShaderProgramOpenGl::assignUniforms() {
    PROFILER_BLOCK("ShaderProgramOpenGl::assignUniforms");

    for (auto& uniform : uniforms) {
        std::function<void()>& uniformFunction = uniform.second;
        if (uniformFunction) {
            uniformFunction();
        }
    }
}

bool ShaderProgramOpenGl::isLinked() {
    return linked;
}

bool ShaderProgramOpenGl::checkLinkStatus() {
    bool linkSuccessful = true;

    Graphics &graphics = Graphics::getInstance();
    if (graphics.handleErrors()) {
        linkSuccessful = false;
        loggerError("Could not link shader program. program:'%s', programId:%d", getName().c_str(), id);
    }

    //check how shader program linking went

    GLint linkStatus = GL_FALSE;
    glGetProgramiv(id, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE)
    {
        linkSuccessful = false;

        GLint logLength = 0;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);
        GLchar *log = new GLchar[logLength + 1];
        glGetProgramInfoLog(id, logLength, 0, log);
        log[logLength] = '\0';

        loggerError("Failed to successfully link shader program. program:'%s', programId:%d, log: %s", getName().c_str(), id, log);

        delete [] log;
    }

    return linkSuccessful;
}
void ShaderProgramOpenGl::bind() {
    PROFILER_BLOCK("ShaderProgramOpenGl::bind");

    bindStack.push_back(this);

    loggerTrace("Binding shader program. program:'%s', programId:%d", getName().c_str(), id);

    glUseProgram(getId());
    assignUniforms();
}

void ShaderProgramOpenGl::unbind() {
    PROFILER_BLOCK("ShaderProgramOpenGl::unbind");

    bindStack.pop_back();

    GLuint newBindId = getCurrentBindId();
    loggerTrace("Un/rebinding shader program. oldProgram:'%s', oldProgramId:%d, newProgramId:%d", getName().c_str(), id, newBindId);

    glUseProgram(newBindId);
}

GLuint ShaderProgramOpenGl::getId() {
    return id;
}

GLint ShaderProgramOpenGl::getUniformLocation(const char* variable) {
    GLuint currentBindId = getCurrentBindId();
    if (currentBindId == 0) {
        loggerWarning("Requested uniform '%s' but no shader is bind!", variable);
        return -1;
    }

    return glGetUniformLocation(currentBindId, variable);
}
