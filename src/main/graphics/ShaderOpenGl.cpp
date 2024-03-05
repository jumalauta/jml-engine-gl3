#include "ShaderOpenGl.h"
#include "ShaderProgramOpenGl.h"
#include "Graphics.h"
#include "logger/logger.h"
#include "Settings.h"

#include <algorithm>
#include <regex>
#include <cstdio>

#ifdef WIN32
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

// method for suppressing command prompt from popping up
int quietSystem(const char *cmd) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if(cmd == NULL) {
        return 0xFF;
    }

    char *cmdEdit = strdup(cmd);
    if(!CreateProcess(NULL, cmdEdit, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        free(cmdEdit);
        return 0xFF;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD ret;
    GetExitCodeProcess(pi.hProcess, &ret);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    free(cmdEdit);

    return static_cast<int>(ret);
}

#else // non-windows platforms
    int quietSystem( const char *cmd ) {
        return system(cmd);
    }
#endif

Shader* Shader::newInstance(std::string filePath) {
    Shader *shader = new ShaderOpenGl(filePath);

    if (shader == NULL) {
        loggerFatal("Could not allocate memory for shader. file:'%s'", filePath.c_str());
        return NULL;
    }

    if (! shader->isSupported()) {
        delete shader;
        return NULL;
    }

    return shader;
}

ShaderOpenGl::ShaderOpenGl(std::string filePath) : Shader(filePath) {
    id = 0;
    diff = true;
}

ShaderOpenGl::~ShaderOpenGl() {
    free();
}

bool ShaderOpenGl::generate() {
    if (!isSupported()) {
        loggerError("Could not process shader. File type not supported. file:'%s'", getFilePath().c_str());
        return false;
    }

    free();
    id = glCreateShader(determineShaderType());
    if (id == 0) {
        Graphics &graphics = Graphics::getInstance();
        graphics.handleErrors();
        loggerError("Could not create shader. file:'%s'", getFilePath().c_str());
        return false;
    }

    return true;
}

void ShaderOpenGl::free() {
    if (id != 0) {
        /*for(ShaderProgram *shaderProgram : shaderPrograms) {
            shaderProgram->free();
        }*/

        loggerDebug("Freeing shader. file:'%s', shaderId:%d", getFilePath().c_str(), id);
        glDeleteShader(id);

        Graphics &graphics = Graphics::getInstance();
        if (graphics.handleErrors()) {
            loggerError("Could not delete shader. file:'%s'", getFilePath().c_str());
        }

        id = 0;
    }
}

bool ShaderOpenGl::isSupported() {
    if (determineShaderType() != 0) {
        return true;
    }

    return false;
}

GLenum ShaderOpenGl::determineShaderType() {
    std::string fileExtension = getFileExtension();
    std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::tolower);

    GLenum type = 0;

    if (fileExtension == "fs") {
        type = GL_FRAGMENT_SHADER;
    } else if (fileExtension == "vs") {
        type = GL_VERTEX_SHADER;
    } else if (fileExtension == "gs") {
        type = GL_GEOMETRY_SHADER;
    } else {
        loggerWarning("File extension not recognized. file:'%s'", getFilePath().c_str());
    }

    return type;
}

bool ShaderOpenGl::isShadertoyShader() {
    if (determineShaderType() != GL_FRAGMENT_SHADER) {
        return false;
    }

    static const std::regex shadertoyMainFunctionRegex("\\s*void\\s+mainImage\\s*\\(\\s*out\\s+vec4\\s+fragColor\\s*,\\s*in\\s+vec2\\s+fragCoord\\s*\\)\\s*(?!;)");
    std::string fileData = std::string(reinterpret_cast<const char*>(getData()));
    if (std::regex_search(fileData, shadertoyMainFunctionRegex)) {
        // ensure that main function is not in the file before shadertoy.com bootstrapping
        static const std::regex shaderMainFunctionRegex("\\s*void\\s+main\\s*\\(\\s*\\)\\s*");
        if (! std::regex_search(fileData, shaderMainFunctionRegex)) {
            return true;
        }
    }

    return false;
}

void ShaderOpenGl::makeShadertoyBootstrap() {
    loggerDebug("Bootstrapping shader with shadertoy.com header shader");

    File f = File(std::string("_embedded/shadertoyBootstrap.fs.part"));
    if (! f.exists()) {
        loggerFatal("Could not find shadertoyBootstrap.fs.part!");
        return;
    }
    if (! f.loadRaw()) {
        loggerFatal("Could not load shadertoy.com bootstrap! file:'%s'", f.getFilePath().c_str());
        return;
    }

    size_t bootstrapFileDataSize = strlen(reinterpret_cast<const char*>(f.getData()));
    size_t sourceFileDataSize = strlen(reinterpret_cast<const char*>(getData()));

    unsigned char *finalShader = new unsigned char[bootstrapFileDataSize + sourceFileDataSize + 1](); // zero-allocate
    if (finalShader == NULL) {
        loggerFatal("Out of memory in shader bootstrapping");
        return;
    }

    // bootstrap file as the header
    memcpy(finalShader, f.getData(), bootstrapFileDataSize);

    // apply user-generated content here
    memcpy(finalShader+bootstrapFileDataSize, getData(), sourceFileDataSize);

    delete [] dataList.back();
    dataList.back() = finalShader;
}

bool ShaderOpenGl::load(bool rollback) {
    loadLastModified = lastModified();

    if (!isFile()) {
        loggerError("Not a file. file:'%s'", getFilePath().c_str());
        return false;
    }

    if (!isSupported()) {
        loggerError("File type not supported. file:'%s'", getFilePath().c_str());
        return false;
    }

    if (!loadRaw(rollback ? 1 : 0)) {
        return false;
    }

    if (Settings::gui.tool) {
        // Validate shaders in tool mode
        if (!validate()) {
            compareFiles();
            if (!rollback) {
                return load(true);
            }
            return false;
        }
    }

    generate();

    if (isShadertoyShader()) {
        makeShadertoyBootstrap();
    }

    const GLchar *shaderSourceString = reinterpret_cast<const GLchar *>(getData());
    glShaderSource(id, 1, &shaderSourceString, NULL);
    Graphics &graphics = Graphics::getInstance();
    if (graphics.handleErrors()) {
        loggerError("Invalid shader source. file:'%s', length:%d", getFilePath().c_str(), length());
        compareFiles();
        if (!rollback) {
            return load(true);
        }
        return false;
    }

    glCompileShader(id);

    bool compileSuccessful = checkCompileStatus();
    setError(compileSuccessful);
    if (!compileSuccessful) {
        compareFiles();
        if (!rollback) {
            return load(true);
        }
        return false;
    }

    for(ShaderProgram *shaderProgram : shaderPrograms) {
        if (!shaderProgram->link()) {
            if (!rollback) {
                compareFiles();
                return load(true);
            }
        }
    }

    return true;
}

bool ShaderOpenGl::isLoaded() {
    if (getData() != NULL && getId() != 0) {
        return true;
    }

    return false;
}

bool ShaderOpenGl::validate() {
    if (Settings::gui.glslValidator == false) {
        loggerTrace("Omitted GLSL validation, validation disabled in settings. file:'%s'", getFilePath().c_str());
        return true;
    }

    static bool initCheck = false;
    if (!initCheck) {
        initCheck = true;
        int ret = quietSystem(Settings::gui.glslValidatorHealthCommand.c_str());
        if (ret != 0) {
            Settings::gui.glslValidator = false;
            loggerWarning("Could not find GLSL validator. command:'%s'", Settings::gui.glslValidatorHealthCommand.c_str());
            return true;
        }
    }

    if (getData() == NULL || length() == 0) {
        loggerWarning("Shader has no data, can't validate. file:'%s'", getFilePath().c_str());
        return false;
    }

    char *temporaryName = tempnam("", "engine_glsl_");
    std::FILE* f = std::fopen(temporaryName, "w");
    if (!f) {
        loggerWarning("Could not open temporary file for writing. temporaryFile:'%s', shader:'%s'", temporaryName, getFilePath().c_str());
        std::free(temporaryName);
        return false;
    }
    std::size_t writtenBytes = std::fwrite(getData(), sizeof(char), length(), f);
    std::fclose(f);

    if (writtenBytes != length()) {
        loggerWarning("Could not write temporary file. temporaryFile:'%s', shader:'%s', writtenBytes:%d", temporaryName, getFilePath().c_str(), writtenBytes);
        std::remove(temporaryName);
        std::free(temporaryName);
        return false;
    }

    std::string typeWord = "<type>";
    std::string fileWord = "<file>";

    std::string command = Settings::gui.glslValidatorCommand;

    if (command.find(typeWord) != std::string::npos) {
        const char *type = NULL;
        switch(determineShaderType()) {
            case GL_FRAGMENT_SHADER:
                type = "frag";
                break;
            case GL_VERTEX_SHADER:
                type = "vert";
                break;
            case GL_GEOMETRY_SHADER:
                type = "geom";
                break;
            default:
                break;
        }
        if (!type) {
            loggerWarning("Could not determine validator shader type. shader:'%s'", command.c_str(), getFilePath().c_str());
            std::remove(temporaryName);
            std::free(temporaryName);
            return false;
        }

        command.replace(command.find(typeWord), typeWord.length(), std::string(type));
    }

    if (command.find(fileWord) != std::string::npos) {
        command.replace(command.find(fileWord), fileWord.length(), std::string(temporaryName));
    }

    //This is a hack to prevent command prompt from popping up in Windows due to popen usage
    //i.e. we'll check system() command's result and only in case of error, we'll check the program output via popen
    int ret = quietSystem(command.c_str());
    if (ret != 0) {
        //TODO: pipe instead of temp file, e.g.: cat distortion.fs | glslangValidator --stdin -S frag -d
        //TODO: windows should not use popen, as cmd.exe popups are anoying
        std::FILE* validatorProcess = popen(command.c_str(), "r");
        if (!validatorProcess) {
            loggerWarning("Could not run validator. command:'%s', shader:'%s'", command.c_str(), getFilePath().c_str());
            std::remove(temporaryName);
            std::free(temporaryName);
            return false;
        }
        
        std::array<char, 512> buffer;
        std::string validationOutput;
        while (fgets(buffer.data(), buffer.size(), validatorProcess) != NULL) {
            validationOutput += buffer.data();
        }

        pclose(validatorProcess);
        std::remove(temporaryName);
        std::free(temporaryName);

        loggerWarning("Shader not valid GLSL. shader:'%s', returnCode:%d, validation results:\n%s", getFilePath().c_str(), ret, validationOutput.c_str());
        return false;
    }

    std::remove(temporaryName);
    std::free(temporaryName);
    return true;
}

bool ShaderOpenGl::checkCompileStatus()
{
    bool compileSuccessful = true;

    Graphics &graphics = Graphics::getInstance();
    if (graphics.handleErrors()) {
        compileSuccessful = false;
        loggerError("Error in shader. shaderId:%d, file:'%s', length:%d", id, getFilePath().c_str(), length());
    }

    // Output compiler logs. Non-errors, such as warnings, are interesting.
    GLint logLength = 0;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);
    GLchar *log = new GLchar[logLength + 1];
    glGetShaderInfoLog(id, logLength, NULL, log);
    log[logLength] = '\0';

    GLint compileStatus = GL_TRUE;
    glGetShaderiv(id, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == GL_TRUE)
    {
        if (getFileScope() == FileScope::CONSTANT) {
            loggerDebug("Compiled shader. shaderId:%d, file:'%s', log: %s", id, getFilePath().c_str(), static_cast<const char*>(log));
        } else {
            loggerInfo("Compiled shader. shaderId:%d, file:'%s', log: %s", id, getFilePath().c_str(), static_cast<const char*>(log));            
        }
    } else {
        compileSuccessful = false;

        loggerError("Failed to successfully compile shader. shaderId:%d, file:'%s', log: %s", id, getFilePath().c_str(), static_cast<const char*>(log));
    }

    delete [] log;

    return compileSuccessful;
}

GLuint ShaderOpenGl::getId() {
    return id;
}

bool ShaderOpenGl::addShaderProgram(ShaderProgram *shaderProgram) {
    shaderPrograms.push_back(dynamic_cast<ShaderProgramOpenGl*>(shaderProgram));
    return true;
}

bool ShaderOpenGl::removeShaderProgram(ShaderProgram *shaderProgram) {
    ShaderProgramOpenGl *shaderProgramOpenGl = dynamic_cast<ShaderProgramOpenGl*>(shaderProgram);
    for(auto it = shaderPrograms.begin(); it != shaderPrograms.end(); it++) {
        if (*it == shaderProgramOpenGl) {
            shaderPrograms.erase(it);
            return true;
        }
    }

    return false;
}
