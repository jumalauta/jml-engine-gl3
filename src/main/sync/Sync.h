#ifndef ENGINE_SYNC_SYNC_H_
#define ENGINE_SYNC_SYNC_H_

class Timer;

class Sync {
public:
    static Sync& getInstance();
    virtual ~Sync() {};
    virtual bool containsVariable(const char *variableName) = 0;
    virtual double getVariableCurrentValue(const char *variableName) = 0;
    virtual void update() = 0;
    virtual bool exit() = 0;
    virtual bool init(Timer *timer) = 0;
protected:
    Sync() {};
    Timer *getTimer();
    void setTimer(Timer *timer);
private:
    Timer *timer;
};

#endif /*ENGINE_SYNC_SYNC_H_*/
