#ifndef ENGINE_SCRIPT_SCRIPTENGINEDUKTAPE_H_
#define ENGINE_SCRIPT_SCRIPTENGINEDUKTAPE_H_

#include "ScriptEngine.h"
#include <duktape.h>

class ScriptEngineDuktape : public ScriptEngine {
public:
    ScriptEngineDuktape();
    ~ScriptEngineDuktape();
    bool init();
    bool exit();
    void garbageCollect();
    bool evalString(const char *string);
    bool evalScript(Script &file);
    bool callClassMethod(const char *className, const char *methodName, const char *effectClassName);
    void synchronizeSettings();
private:
    void bindFunctions();

    duk_context *ctx;
    int stackTraceCalled;
};

#endif /*ENGINE_SCRIPT_SCRIPTENGINEDUKTAPE_H_*/
