#ifndef ENGINE_SETTINGS_H_
#define ENGINE_SETTINGS_H_

#include <string>
#include <vector>

#include "graphics/datatypes.h"

#include "json.hpp"

#define JSON_UNMARSHAL_VAR(parent, type, name) \
    if (j.find(#name) != j.end()) parent . name = j.at(#name).get<type>();

// FIXME: use STRING variables or so in enum serialization
// int type casting seems to not work
#define JSON_MARSHAL_ENUM(type, name) \
    j = nlohmann::json(static_cast<int>(name));

#define JSON_UNMARSHAL_ENUM(parent, type, name) \
    if (j.find(#name) != j.end()) parent . name = static_cast<type>(j.at(#name).get<int>());

struct GuiSettings {
    GuiSettings();

    std::string projectPath;
    std::string logFile;
    bool logFileAppend;
    bool tool;
    bool profiler;
    bool profilerListener;
    bool editor;
    int fileModifyGracePeriod;
    int largeFileModifyGracePeriod;
    int fileRefreshThreadSleep;
    int fileHistorySize;
    std::string gnuRocketHost;
    unsigned short gnuRocketPort;
    double startPosition;
    std::string glslValidatorHealthCommand;
    std::string glslValidatorCommand;
    bool glslValidator;
    std::string diffHealthCommand;
    std::string diffCommand;
    bool diff;
};

struct WindowSettings {
    WindowSettings();

    unsigned int height;
    unsigned int width;
    bool fullscreen;
    bool verticalSync;

    unsigned int canvasPositionX;
    unsigned int canvasPositionY;
    unsigned int screenAreaHeight;
    unsigned int screenAreaWidth;

    void setWindowDimensions(unsigned int height, unsigned int width);
    void fitCanvasToWindow();
};

struct AudioSettings {
    AudioSettings();

    bool mute;
    bool capture;
    bool timeSource;
    unsigned short samples;
    double captureMixVolume;
    std::string device;
};

struct LoggerSettings {
    LoggerSettings();

    bool showMessageBox;
    int logLevel;
    int exitLogLevel;
    int titleLogLevel;
    int pauseLogLevel;
    uint64_t duplicateLogGracePeriod;
};

struct DisplayMode;

struct ModelSettings {
    ModelSettings();
    bool joinIdenticalVertices;
    bool triangulate;
    bool generateNormals;
    bool validate;
    bool removeRedundant;
    bool fixInvalidData;
    bool optimizeMeshes;
    bool optimizeGraph;
};

struct GraphicsSettings {
    GraphicsSettings();

    std::string shaderProgramDefault;
    std::string shaderProgramDefaultShadow;

    ModelSettings model;

    std::vector<DisplayMode> displayModes;

    Color clearColor;

    TextureDataType defaultTextureDataType;
    TextureTargetType defaultTextureTargetType;
    TextureType defaultTextureType;
    TextureFilter defaultTextureFilter;
    TextureWrap defaultTextureWrap;
    TextureFormat defaultTextureFormat;

    TextureFilter defaultFboTextureFilter;
    TextureWrap defaultFboTextureWrap;

    float canvasHeight;
    float canvasWidth;
    float aspectRatio;

    unsigned int maxTextureUnits;
    unsigned int maxActiveLightCount;

    unsigned int requestedMajorVersion;
    unsigned int requestedMinorVersion;

    void setCanvasDimensions(float height, float width);
    void calculateAspectRatio();
};

struct FftSettings {
    FftSettings();
    bool enable;
    double divisor;
    double clipMin;
    double clipMax;
    unsigned int size;
    unsigned int history;
};

struct DemoSettings {
    DemoSettings();
    FftSettings fft;
    GraphicsSettings graphics;
    double length;
    std::string song;
    bool songLoop;
    double targetFps;
    double beatsPerMinute;
    double rowsPerBeat;
    std::string title;
    std::string rocketXmlFile;
    std::string midiManagerFile;
    nlohmann::json custom;
    bool networking;

    static bool loadDemoSettings(std::string file = "script.json");
    static bool loadDemoSettingsFromString(std::string data);
    static bool saveDemoSettings(std::string file = "script.json");
    static std::string serialize();
};

struct Settings {
    static DemoSettings demo;
    static GuiSettings gui;
    static WindowSettings window;
    static AudioSettings audio;
    static LoggerSettings logger;
    static bool showMenu;
    static std::string settingsFile;
    static bool loadSettings(std::string file = "settings.json");
    static bool loadSettingsFromString(std::string data);
    static bool saveSettings(std::string file = "");
    static std::string serialize();
};

#endif /*ENGINE_SETTINGS_H_*/
