#include "logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include <string>
#include <functional>
#include <thread>

#include "time/Date.h"
#include "time/Timer.h"
#include "time/TimeFormatter.h"
#include "Settings.h"
#include "EnginePlayer.h"

#include "ui/Window.h"

static TimeFormatter demoTimerFormat = TimeFormatter("HH:mm:ss.SSS");

static Timer *loggerTimer = NULL;
void setLoggerTimer(Timer *timer) {
    loggerTimer = timer;
}

static std::string previousLogOutput = "";
static uint64_t previousLogOutputTime = 0;

static std::function<void(const char *, const char *, const char *, const char *, int, const char *, const char *)> printFunction;
static const char *printState = "N/A";

void setLoggerPrintState(const char *state) {
    printState = state;
}

void setLoggerPrintFunction(const std::function<void(const char *, const char *, const char *, const char *, int, const char *, const char *)> &_printFunction) {
    printFunction = _printFunction;
}

static const char *stripFilePath(const char *fileName) {
    const char *fileNameOnly = strrchr(fileName, '/');

    if (fileNameOnly == NULL) {
        fileNameOnly = strrchr(fileName, '\\');
    }

    if (fileNameOnly != NULL) {
        fileNameOnly = fileNameOnly + 1; // remove the directory separator
    } else {
        // as a fallback show whole file name
        fileNameOnly = fileName;
    }

    return fileNameOnly;
}

void __debugPrintf(const char *fileName, const char *functionName, int sourceLine, int level, const char *fmt, ...) {
    if (level < Settings::logger.logLevel) {
        return; // skip logging
    }

    int size = 1024;
    char *p = NULL, *np = NULL;
    va_list ap;
    
    if ((p = (char*)malloc(size)) == NULL) {
        return;
    }

    const char *levelName = NULL;
    FILE *levelFile = stdout;

    switch(level) {
        case LEVEL_FATAL:
            levelName = "FATAL";
            break;
        case LEVEL_ERROR:
            levelName = "ERROR";
            break;
        case LEVEL_WARNING:
            levelName = "WARNING";
            break;
        case LEVEL_INFO:
            levelName = "INFO";
            break;
        case LEVEL_DEBUG:
            levelName = "DEBUG";
            break;                    
        case LEVEL_TRACE:
        default:
            levelName = "TRACE";
            break;
    }

    std::string time = "00:00:00.000";
    if (loggerTimer != NULL) {
        time = demoTimerFormat.format(loggerTimer->getElapsedTime());
    }

    const char *fileNameOnly = stripFilePath(fileName);

    while (1)
    {
        va_start(ap, fmt);
        int n = vsnprintf(p, size, fmt, ap);
        va_end(ap);

        if (n < 0) {
            free(p);
            return; // encoding error
        }
        
        if (n < size) {
            //Some libraries (assimp, opengl) insist in forcing a new line sometimes in their messages
            //We've the only new line forcer in this village
            if (p[n-1] == '\n') {
                p[n-1] = '\0';
            }

            if (loggerTimer != NULL) {
                //do not allow adjacent duplicate log messages (within the grace period)
                if (loggerTimer->getTimeInMilliseconds() - previousLogOutputTime < Settings::logger.duplicateLogGracePeriod
                        && !strcmp(previousLogOutput.c_str(), p)) {

                    free(p);
                    return;
                }

                previousLogOutputTime = loggerTimer->getTimeInMilliseconds();
                previousLogOutput = std::string(p);
            }

            unsigned int threadId = static_cast<unsigned int>(std::hash<std::thread::id>{}(std::this_thread::get_id()));
            fprintf(levelFile, "[%s] %s %x %s:%s():%d %s:\n",
                time.c_str(), printState, threadId, fileNameOnly, functionName, sourceLine, levelName);
            fprintf(levelFile, "%s\n", p);
            fflush(levelFile);

            if (printFunction) {
                printFunction(time.c_str(), printState, fileNameOnly, functionName, sourceLine, levelName, p);
            }

            if (level >= Settings::logger.exitLogLevel) {
                Window::showMessageBox(level, "Failure", p);
                free(p);

                setLoggerPrintState("EXIT_FAILURE");
                exit(EXIT_FAILURE);
            }

            if (level >= Settings::logger.titleLogLevel) {
                std::string title = (levelName + std::string(": ") + std::string(p)).substr(0, 128); 

                Window* playerWindow = EnginePlayer::getInstance().getWindow(WindowType::PLAYER);
                if (playerWindow != NULL) {
                    playerWindow->setTitle(title);
                }
                Window* editorWindow = EnginePlayer::getInstance().getWindow(WindowType::EDITOR);
                if (editorWindow != NULL) {
                    editorWindow->setTitle(title);
                }
            }

            if (loggerTimer != NULL && level >= Settings::logger.pauseLogLevel && Settings::gui.tool) {
                loggerTimer->pause(true);
            }

            free(p);

            return;
        } else {
            // not enough buffer to write log => reallocate larger buffer
            size = n+1;

            if ((np = (char*)realloc(p, size)) == NULL) {
                free(p);
                p = NULL;
                fprintf(stderr, "Could not allocate memory for logging...");
                exit(EXIT_FAILURE); //We'll consider not allocating memory for logging very fatal...
            } else {
                p = np;
            }
        }
    }
}
