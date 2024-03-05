// cd ../build && cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
// cd ../build && cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
// cd ../build && mingw32-make && cd ../release && engine
// cd ../build && mingw32-make && cd ../release && gdb --args engine.exe

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <iostream>
#include <exception>
#include <stdexcept>

#include "optionparser.h"
#include "version.h"
#include "Settings.h"
#include "EnginePlayer.h"

#include "logger/logger.h"

#include "time/SystemTime.h"
#include "time/Timer.h"

#include "io/File.h"
#include "io/MemoryManager.h"

#include "ui/Window.h"
#include "ui/Input.h"
#include "graphics/Graphics.h"

#include "graphics/Image.h"
#include "graphics/Texture.h"
#include "graphics/Shader.h"
#include "graphics/ShaderProgram.h"
#include "graphics/Fbo.h"
#include "script/Script.h"
#include "script/ScriptEngine.h"

#include "math/TransformationMatrix.h"

#include "graphics/model/Model.h"

#include "graphics/Font.h"
#include "graphics/Text.h"

#include "graphics/model/TexturedQuad.h"

#ifdef UNUSED
#elif defined(__GNUC__)
# define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#elif defined(__LCLINT__)
# define UNUSED(x) /*@unused@*/ x
#else
# define UNUSED(x) x
#endif

struct Arg: public option::Arg {
    static option::ArgStatus Required(const option::Option& option, bool UNUSED(msg)) {
        if (option.arg != 0) {
            return option::ARG_OK;
        }

        return option::ARG_ILLEGAL;
    }
};

enum optionIndex {
    UNKNOWN, HELP, VERSION, LOG_FILE, SETTINGS_FILE, PROJECT_PATH, SHOW_MENU, AUDIO, AUDIO_TIMER_SOURCE, RESOLUTION, FULLSCREEN, VERTICAL_SYNC,
    LOG_LEVEL, TOOL, EDITOR, START_POSITION,
    PROFILER, PROFILER_LISTENER, GNU_ROCKET_HOST, GNU_ROCKET_PORT, GLSL_VALIDATOR, GLSL_VALIDATOR_COMMAND
};
const option::Descriptor usage[] = {
    {UNKNOWN,                0, "" , ""    ,                   Arg::None,     "USAGE: engine [options]\n\nOptions:" },
    {HELP,                   0, "" , "help",                   Arg::None,     "  --help                              Show help." },
    {VERSION,                0, "" , "version",                Arg::None,     "  --version                           Show version." },
    {LOG_FILE,               0, "" , "log-file",               Arg::Required, "  --log-file=<file>                   Set log output file." },
    {LOG_LEVEL,              0, "" , "log-level",              Arg::Required, "  --log-level=<level>                 Set minimum log level." },
    {SETTINGS_FILE,          0, "" , "settings-file",          Arg::Required, "  --settings-file=<file>              Load settings from specified location." },
    {PROJECT_PATH,           0, "" , "project-path",           Arg::Required, "  --project-path=<path>               Set project base path." },
    {SHOW_MENU,              0, "" , "show-menu",              Arg::Required, "  --show-menu=<true|false>            Show menu." },
    {AUDIO,                  0, "" , "audio",                  Arg::Required, "  --audio=<true|false>                Set audio on/off." },
    {AUDIO_TIMER_SOURCE,     0, "" , "audio-timer-source",     Arg::Required, "  --audio-timer-source=<true|false>   Set audio on/off." },
    {RESOLUTION,             0, "" , "resolution",             Arg::Required, "  --resolution=<widthxheight>         Set window resolution." },
    {FULLSCREEN,             0, "" , "fullscreen",             Arg::Required, "  --fullscreen=<true|false>           Set window fullscreen." },
    {VERTICAL_SYNC,          0, "" , "vertical-sync",          Arg::Required, "  --vertical-sync=<true|false>        Set vertical sync." },
    {TOOL,                   0, "" , "tool",                   Arg::Required, "  --tool=<true|false>                 Set demo tool usage." },
    {EDITOR,                 0, "" , "editor",                 Arg::Required, "  --editor=<true|false>               Set editor." },
    {START_POSITION,         0, "" , "start-position",         Arg::Required, "  --start-position=<seconds>          Set demo timer start position." },
    {PROFILER,               0, "" , "profiler",               Arg::Required, "  --profiler=<true|false>             Enable profiler." },
    {PROFILER_LISTENER,      0, "" , "profiler-listener",      Arg::Required, "  --profiler-listener=<true|false>    Enable listener vs. dump to file." },
    {GNU_ROCKET_HOST,        0, "" , "gnu-rocket-host",        Arg::Required, "  --gnu-rocket-host=<host>            Set GNU Rocket host." },
    {GNU_ROCKET_PORT,        0, "" , "gnu-rocket-port",        Arg::Required, "  --gnu-rocket-port=<port>            Set GNU Rocket port." },
    {GLSL_VALIDATOR,         0, "" , "glsl-validator",         Arg::Required, "  --glsl-validator=<true|false>       Set GLSL validator on/off." },
    {GLSL_VALIDATOR_COMMAND, 0, "" , "glsl-validator-command", Arg::Required, "  --glsl-validator-command=<cmd>      Set GLSL validator command." },
    {0, 0, 0, 0, 0, 0}
};

static bool getArgumentBoolean(option::Option* argument) {
    if (argument == NULL || argument->arg == NULL) {
        exit(EXIT_FAILURE);
    }

    bool value = false;
    if (!strcmp(argument->arg, "true")) {
        value = true;
    } else if (!strcmp(argument->arg, "false")) {
        value = false;
    } else {
        std::cout << "Argument " << argument->arg << " value expected to be either \"true\" or \"false\"" << std::endl;
        exit(EXIT_FAILURE);
    }

    return value;
}

static void parseCommandlineArguments(int argc, char* argv[]) {
    std::string programName = std::string(argv[0]);

    // skip program name argv[0] if present
    argc -= (argc > 0);
    argv += (argc > 0);

    if (!Settings::gui.logFile.empty()) {
        const char *fileWriteParameters = "w";
        if (Settings::gui.logFileAppend) {
            fileWriteParameters = "a";
        }

        fclose(stdout);
        FILE *logFile = freopen (Settings::gui.logFile.c_str(), fileWriteParameters, stdout);
        if (logFile == NULL) {
            std::cerr << "Could not open log file for writing: " << Settings::gui.logFile << std::endl;
        }
    }

    option::Stats stats(usage, argc, argv);

    option::Option *options = new option::Option[stats.options_max];
    option::Option *buffer = new option::Option[stats.buffer_max];
    option::Parser parse(usage, argc, argv, options, buffer);

    if (parse.error()) {
        std::cout << "Could not parse command line arguments!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Attempt to forward logging to the wanted location before any other actions (like HELP / USAGE argument processing)
    option::Option* logFileArgument = options[LOG_FILE];
    if (logFileArgument) {
        Settings::gui.logFile = logFileArgument->arg;
    }

    if (options[HELP]) {
        option::printUsage(std::cout, usage);
        exit(EXIT_SUCCESS);
    }

    if (options[VERSION]) {
        std::cout << ENGINE_VERSION_MAJOR << "." << ENGINE_VERSION_MINOR << "." << ENGINE_VERSION_PATCH
          << " (" << ENGINE_LATEST_COMMIT << " " << ENGINE_BUILD_TIMESTAMP << ")\n";
        exit(EXIT_SUCCESS);
    }

    option::Option* toolArgument = options[TOOL];
    if (toolArgument) {
        Settings::gui.tool = getArgumentBoolean(toolArgument);
        Settings::window.verticalSync = false; // implicitly disable vsync in tool mode
        Settings::logger.logLevel = LEVEL_INFO; // defaulting to info logging in tool mode
    }

    option::Option* logLevelArgument = options[LOG_LEVEL];
    if (logLevelArgument) {
        //TODO: Support human-readable log levels, i.e., "TRACE", "WARNING" etc...
        int logLevel = LEVEL_TRACE;
        int ret = sscanf(logLevelArgument->arg, "%d", &logLevel);
        if (ret != 1) {
            std::cout << "Could not parse argument " << logLevelArgument->name << std::endl;
            exit(EXIT_FAILURE);
        }

        Settings::logger.logLevel = logLevel;
    }

    option::Option* audioArgument = options[AUDIO];
    if (audioArgument) {
        Settings::audio.mute = ! getArgumentBoolean(audioArgument);
    }

    option::Option* resolutionArgument = options[RESOLUTION];
    if (resolutionArgument) {
        unsigned int width  = 0;
        unsigned int height = 0;
        int ret = sscanf(resolutionArgument->arg, "%4ux%4u", &width, &height);
        if (ret != 2) {
            std::cout << "Could not parse argument " << resolutionArgument->name << std::endl;
            exit(EXIT_FAILURE);
        }

        Settings::window.setWindowDimensions(width, height);
    }

    option::Option* settingsFileArgument = options[SETTINGS_FILE];
    if (settingsFileArgument) {
        std::string settingsFile = settingsFileArgument->arg;
        if (!Settings::loadSettings(settingsFile)) {
            std::cout << "Could not load settings " << settingsFile << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    option::Option* projectPathArgument = options[PROJECT_PATH];
    if (projectPathArgument) {
        Settings::gui.projectPath = projectPathArgument->arg;
    }

    std::string projectDirectory = Settings::gui.projectPath;
    if (projectDirectory.empty()) {
        //No project path given, presume project path as executable path's "data" directory
        projectDirectory = "data";

        std::size_t pathIndex = programName.find_last_of("/");
        if (pathIndex != std::string::npos) {
            projectDirectory = programName.substr(0, pathIndex + 1) + "data";
        }
    }
    File::setProjectPath(projectDirectory);

    option::Option* showMenuArgument = options[SHOW_MENU];
    if (showMenuArgument) {
        Settings::showMenu = getArgumentBoolean(showMenuArgument);
    }

    option::Option* fullscreenArgument = options[FULLSCREEN];
    if (fullscreenArgument) {
        Settings::window.fullscreen = getArgumentBoolean(fullscreenArgument);
    }

    option::Option* profilerArgument = options[PROFILER];
    if (profilerArgument) {
        Settings::gui.profiler = getArgumentBoolean(profilerArgument);
    }

    option::Option* profilerListenerArgument = options[PROFILER_LISTENER];
    if (profilerListenerArgument) {
        Settings::gui.profilerListener = getArgumentBoolean(profilerListenerArgument);
    }

    option::Option* gnuRocketHostArgument = options[GNU_ROCKET_HOST];
    if (gnuRocketHostArgument) {
        Settings::gui.gnuRocketHost = gnuRocketHostArgument->arg;
    }

    option::Option* gnuRocketPortArgument = options[GNU_ROCKET_PORT];
    if (gnuRocketPortArgument) {
        unsigned short port = 0;
        int ret = sscanf(gnuRocketPortArgument->arg, "%hu", &port);
        if (ret != 1) {
            std::cout << "Could not parse argument " << gnuRocketPortArgument->name << std::endl;
            exit(EXIT_FAILURE);
        }

        Settings::gui.gnuRocketPort = port;
    }

    option::Option* glslValidatorArgument = options[GLSL_VALIDATOR];
    if (glslValidatorArgument) {
        Settings::gui.glslValidator = getArgumentBoolean(glslValidatorArgument);
    }

    option::Option* glslValidatorCommandArgument = options[GLSL_VALIDATOR_COMMAND];
    if (glslValidatorCommandArgument) {
        Settings::gui.glslValidatorCommand = glslValidatorCommandArgument->arg;
    }

    option::Option* verticalSyncArgument = options[VERTICAL_SYNC];
    if (verticalSyncArgument) {
        Settings::window.verticalSync = getArgumentBoolean(verticalSyncArgument);
    }

    option::Option* editorArgument = options[EDITOR];
    if (editorArgument) {
        Settings::gui.editor = getArgumentBoolean(editorArgument);

        if (Settings::gui.editor) {
            // Enabling editor implicitly enables the tool mode
            Settings::gui.tool = true;
        }
    }

    option::Option* startPositionArgument = options[START_POSITION];
    if (startPositionArgument) {
        float startPosition = 0.0f;
        int ret = sscanf(startPositionArgument->arg, "%f", &startPosition);
        if (ret != 1) {
            std::cout << "Could not parse argument " << startPositionArgument->name << std::endl;
            exit(EXIT_FAILURE);
        }

        Settings::gui.startPosition = startPosition;
    }

    for (option::Option* opt = options[UNKNOWN]; opt; opt = opt->next()) {
        std::cout << "Unknown option: " << opt->name << "\n\n";
        option::printUsage(std::cout, usage);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < parse.nonOptionsCount(); ++i) {
        std::cout << "Unknown option: " << parse.nonOption(i) << "\n\n";
        option::printUsage(std::cout, usage);
        exit(EXIT_FAILURE);
    }

    delete [] options;
    delete [] buffer;
}

int main(int argc, char* argv[]) {
    try {
        setLoggerPrintState("START");

        uint64_t programStart = SystemTime::getTimeInMillis();

        ScriptEngine& scriptEngine = ScriptEngine::getInstance();
        if (!scriptEngine.init()) {
            loggerFatal("Failed to initialize script engine");
            exit(EXIT_FAILURE);
        }

        File settingsFile = File("settings.json");
        if (settingsFile.isFile()) {
            if (!Settings::loadSettings(settingsFile.getFilePath())) {
                std::cout << "Could not load settings " << settingsFile.getFilePath() << std::endl;
                exit(EXIT_FAILURE);
            }
        }

        parseCommandlineArguments(argc, argv);

        loggerInfo("Program '%s' starting. Demo version %s (commit %s). Start time: %u", argv[0], ENGINE_VERSION, ENGINE_LATEST_COMMIT, programStart);

        if (Settings::gui.profiler) {
            loggerTrace("Enabling profiler");

            EASY_PROFILER_ENABLE;
            EASY_MAIN_THREAD;

            if (Settings::gui.profilerListener) {
                profiler::startListen();
                if (!profiler::isListening()) {
                    loggerWarning("Could not start listening!");
                }
            }
        }

        EnginePlayer& player = EnginePlayer::getInstance();
        if (!player.init()) {
            if (player.getInput().isUserExit()) {
                return EXIT_SUCCESS;
            }

            loggerError("Could not initialize player, exiting");
            return EXIT_FAILURE;
        }

        loggerDebug("Preloading time %u ms", SystemTime::getTimeInMillis() - programStart);

        // Load and start demo
        player.run();

        setLoggerPrintState("EXIT");

        uint64_t exitStart = SystemTime::getTimeInMillis();

        if (!player.exit()) {
            loggerWarning("Could not exit player properly!");
        }

        loggerInfo("Demo exited. Exiting time %u ms. Exit time: %u", SystemTime::getTimeInMillis() - exitStart, SystemTime::getTimeInMillis());

        if (Settings::gui.profiler) {
            if (Settings::gui.profilerListener) {
                profiler::stopListen();
                if (!profiler::isListening()) {
                    loggerWarning("Could not stop listening!");
                }
            } else {
                profiler::dumpBlocksToFile("engine_easy_profiler_dump.prof");
            }
        }

    } catch (const std::invalid_argument& e) {
        loggerError("Invalid argument exception thrown! message:'%s'", e.what());
        throw;
    } catch (const std::length_error& e) {
        loggerError("Length error thrown! message:'%s'", e.what());
        throw;
    } catch (const std::out_of_range& e) {
        loggerError("Out of range exception thrown! message:'%s'", e.what());
        throw;
    } catch (const std::logic_error& e) {
        loggerError("Logic error thrown! message:'%s'", e.what());
        throw;
    } catch (const std::runtime_error& e) {
        loggerError("Runtime error thrown! message:'%s'", e.what());
        throw;
    } catch (...) {
        try {
            if (std::current_exception()) {
                std::rethrow_exception(std::current_exception());
            }

            loggerError("Unknown exception thrown!");
        } catch(const std::exception& e) {
            loggerError("Exception thrown! message:'%s'", e.what());
        }

        throw;
    }

    if (!Settings::gui.logFile.empty()) {
        fclose(stdout);
    }

    return EXIT_SUCCESS;
}
