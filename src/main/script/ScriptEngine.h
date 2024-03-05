#ifndef ENGINE_SCRIPT_SCRIPTENGINE_H_
#define ENGINE_SCRIPT_SCRIPTENGINE_H_

class Script;

class ScriptEngine {
public:
    static ScriptEngine& getInstance();
    virtual ~ScriptEngine() {};
    virtual bool init() = 0;
    virtual bool exit() = 0;
    virtual void garbageCollect() = 0;
    virtual bool evalString(const char *string) = 0;
    virtual bool evalScript(Script &file) = 0;
    virtual void synchronizeSettings() = 0;
protected:
    ScriptEngine() {};
};

#endif /*ENGINE_SCRIPT_SCRIPTENGINE_H_*/
