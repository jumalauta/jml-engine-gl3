#ifndef ENGINE_LOGGER_LOGGER_H_
#define ENGINE_LOGGER_LOGGER_H_

#include <easy/profiler.h>

#ifndef PROFILER_BLOCK
    #define PROFILER_BLOCK(name) EASY_BLOCK(name, profiler::colors::Default)
#endif

enum {
    LEVEL_TRACE,
    LEVEL_DEBUG,
    LEVEL_INFO,
    LEVEL_WARNING,
    LEVEL_ERROR,
    LEVEL_FATAL
};

/*#ifdef NDEBUG
    #define loggerTrace (void)sizeof
    #define loggerDebug (void)sizeof
    #define loggerInfo (void)sizeof
    #define loggerWarning (void)sizeof
    #define loggerError (void)sizeof
    #define loggerFatal (void)sizeof
#else*/
    #define loggerTrace(...) __debugPrintf(__FILE__, __func__, __LINE__, LEVEL_TRACE, __VA_ARGS__)
    #define loggerDebug(...) __debugPrintf(__FILE__, __func__, __LINE__, LEVEL_DEBUG, __VA_ARGS__)
    #define loggerInfo(...) __debugPrintf(__FILE__, __func__, __LINE__, LEVEL_INFO, __VA_ARGS__)
    #define loggerWarning(...) __debugPrintf(__FILE__, __func__, __LINE__, LEVEL_WARNING, __VA_ARGS__)
    #define loggerError(...) __debugPrintf(__FILE__, __func__, __LINE__, LEVEL_ERROR, __VA_ARGS__)
    #define loggerFatal(...) __debugPrintf(__FILE__, __func__, __LINE__, LEVEL_FATAL, __VA_ARGS__)
//#endif

class Timer;
extern void setLoggerPrintState(const char *state);
extern void setLoggerTimer(Timer *timer);
extern void __debugPrintf(const char *fileName, const char *functionName, int sourceLine, int level, const char *fmt, ...);

#endif /*ENGINE_LOGGER_LOGGER_H_*/
