#include "ScriptDuktape.h"

#include "ScriptEngine.h"
#include "logger/logger.h"

#include <algorithm>


Script* Script::newInstance(std::string filePath) {
    Script *script = new ScriptDuktape(filePath);
    if (script == NULL) {
        loggerFatal("Could not allocate memory for script file:'%s'", filePath.c_str());
        return NULL;
    }

    if (! script->isSupported()) {
        delete script;
        return NULL;
    }

    return script;
}

ScriptDuktape::ScriptDuktape(std::string filePath) : Script(filePath) {
    initEval = "";
    exitEval = "";
    initClassEval = "";
    exitClassEval = "";
    initialized = false;
}

ScriptDuktape::~ScriptDuktape() {
    if (initialized) {
        free();
    }
}

bool ScriptDuktape::isSupported() {
    std::string fileExtension = getFileExtension();
    std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::tolower);
    if (fileExtension == "js") {
        return true;
    }

    return false;
}

bool ScriptDuktape::load(bool rollback) {
    loadLastModified = lastModified();

    setError(true);

    if (!isFile()) {
        loggerError("Not a file. file:'%s'", getFilePath().c_str());
        return false;
    }

    if (!isSupported()) {
        loggerError("File type not supported. file:'%s'", getFilePath().c_str());
        return false;
    }

    if (!loadRaw(rollback ? 1 : 0)) {
        loggerError("Could not load file. file:'%s'", getFilePath().c_str());
        return false;
    }

    if (initialized) {
        free();
    }
    
    if (!initClassEval.empty()) {
        if (!evalString(initClassEval)) {
            if (!rollback) {
                return load(true);
            }
            return false;
        }
    }

    ScriptEngine &scriptEngine = ScriptEngine::getInstance();
    if (!scriptEngine.evalScript(*this)) {
        compareFiles();
        if (!rollback) {
            return load(true);
        }
        return false;
    }

    if (!initEval.empty()) {
        if (!evalString(initEval)) {
            compareFiles();
            if (!rollback) {
                return load(true);
            }
            return false;
        }
    }

    if (getFileScope() == FileScope::CONSTANT) {
        loggerDebug("Loaded script. file:'%s', pointer:0x%p", getFilePath().c_str(), this);
    } else {        
        loggerInfo("Loaded script. file:'%s', pointer:0x%p", getFilePath().c_str(), this);
    }

    initialized = true;

    setError(false);

    return true;
}

void ScriptDuktape::setInitClassCall(std::string eval) {
    initClassEval = eval;
}

void ScriptDuktape::setExitClassCall(std::string eval) {
    exitClassEval = eval;
}

void ScriptDuktape::setInitCall(std::string eval) {
    initEval = eval;
}

void ScriptDuktape::setExitCall(std::string eval) {
    exitEval = eval;
}

bool ScriptDuktape::evalString(std::string eval) {
    if (eval.empty()) {
        return true;
    }

    ScriptEngine &scriptEngine = ScriptEngine::getInstance();
    bool success = scriptEngine.evalString(eval.c_str());

    if (!success) {
        setError(!success);
        compareFiles();
    }

    return success;
}


void ScriptDuktape::free() {
    if (!exitEval.empty()) {
        evalString(exitEval);
    }

    if (!exitClassEval.empty()) {
        evalString(exitClassEval);
    }

    ScriptEngine &scriptEngine = ScriptEngine::getInstance();
    scriptEngine.garbageCollect();
    initialized = false;

    loggerDebug("Deinitialized script. file:'%s', pointer:0x%p", getFilePath().c_str(), this);
}
