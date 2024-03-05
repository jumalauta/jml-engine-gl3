#ifndef ENGINE_SCRIPT_SCRIPTDUKTAPE_H_
#define ENGINE_SCRIPT_SCRIPTDUKTAPE_H_

#include "Script.h"

class ScriptDuktape : public Script {
public:
    explicit ScriptDuktape(std::string filePath);
    ~ScriptDuktape();
    bool load(bool rollback=false);
    bool isSupported();
    void free();
    void setInitClassCall(std::string eval);
    void setInitCall(std::string eval);
    void setExitCall(std::string eval);
    void setExitClassCall(std::string eval);
    bool evalString(std::string eval);
private:
    std::string initEval;
    std::string initClassEval;
    std::string exitEval;
    std::string exitClassEval;
    bool initialized;
};

#endif /*ENGINE_SCRIPT_SCRIPTDUKTAPE_H_*/
