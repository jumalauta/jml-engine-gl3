#include "Settings.h"

#include "version.h"
#include "ui/Gui.h"
#include "logger/logger.h"
#include "io/File.h"

#include "graphics/Graphics.h"
#include "EnginePlayer.h"
#include "time/Fps.h"
#include "time/Timer.h"
#include "sync/SyncRocket.h"

#include <fstream>
#include <sstream>

GuiSettings::GuiSettings() {
    projectPath = "";

#ifdef _WIN32
    logFile = "stdout.log";
#else
    logFile = ""; // print to actual stdout
#endif

    logFileAppend = false;

    tool = false;
    profiler = false;
    profilerListener = true;
    editor = false;
    fileModifyGracePeriod = 25;
    largeFileModifyGracePeriod = 100;
    fileHistorySize = 1;
    startPosition = 0.0;
    fileRefreshThreadSleep = 150;

    gnuRocketHost = "localhost";
    gnuRocketPort = 1338;

    glslValidatorHealthCommand = "glslangValidator -v";
    glslValidatorCommand = "glslangValidator -d -S <type> <file>";
    glslValidator = true;

    diffHealthCommand = "diff --version";
    diffCommand = "diff --ignore-all-space --unified '<oldFile>' '<newFile>'";
    diff = true;
}

WindowSettings::WindowSettings() {
    fullscreen = true;
    verticalSync = true;

    setWindowDimensions(1920, 1080);
}

void WindowSettings::setWindowDimensions(unsigned int width, unsigned int height) {
    this->width = width;
    this->height = height;

    fitCanvasToWindow();
}

void WindowSettings::fitCanvasToWindow() {
    // fit the canvas with correct aspect ratio to the window
    screenAreaWidth = this->width * Settings::demo.graphics.aspectRatio + 0.5;
    if (screenAreaWidth > this->width)
    {
        screenAreaWidth = this->width;
    }

    screenAreaHeight = this->width / Settings::demo.graphics.aspectRatio + 0.5;
    if (screenAreaHeight > this->height)
    {
        screenAreaHeight = this->height;
    }

    // align canvas X/Y position to center of the window
    canvasPositionX = static_cast<unsigned int>((this->width - screenAreaWidth) / 2.0f);
    canvasPositionY = static_cast<unsigned int>((this->height - screenAreaHeight) / 2.0f);
}

AudioSettings::AudioSettings() {
    mute = false;
    capture = false;
    timeSource = false; // TODO: considered experimental, need to be made smarter
    // audio buffer size in samples (power of 2)
    // for lower latency audio capture-playback sample amount should be kept
    // small, like 512 or so
    samples = 4096;
    captureMixVolume = 1.0;
    device = "";
}

LoggerSettings::LoggerSettings() {
    showMessageBox = true;
    logLevel = LEVEL_WARNING;
    // WARNING: Some logic expects FATAL to exit immediately, otherwise it would segfault
    exitLogLevel = LEVEL_FATAL;
    duplicateLogGracePeriod = 1000; // in ms
    titleLogLevel = LEVEL_WARNING;
    pauseLogLevel = LEVEL_WARNING;
}

DemoSettings Settings::demo = DemoSettings();
GuiSettings Settings::gui = GuiSettings();
WindowSettings Settings::window = WindowSettings();
AudioSettings Settings::audio = AudioSettings();
LoggerSettings Settings::logger = LoggerSettings();

bool Settings::showMenu = true;

static void to_json(nlohmann::json& j, const Color& color) {
    j = nlohmann::json::object();
    j["r"] = color.r;
    j["g"] = color.g;
    j["b"] = color.b;
    j["a"] = color.a;
}

static void from_json(const nlohmann::json& j, Color& color) {
    JSON_UNMARSHAL_VAR(color, double, r);
    JSON_UNMARSHAL_VAR(color, double, g);
    JSON_UNMARSHAL_VAR(color, double, b);
    JSON_UNMARSHAL_VAR(color, double, a);
}



static void to_json(nlohmann::json& j, const TextureDataType& textureDataType) {
    JSON_MARSHAL_ENUM(TextureDataType, textureDataType);
}


static void to_json(nlohmann::json& j, const TextureType& textureType) {
    JSON_MARSHAL_ENUM(TextureType, textureType);
}

static void to_json(nlohmann::json& j, const TextureFilter& textureFilter) {
    JSON_MARSHAL_ENUM(TextureFilter, textureFilter);
}

static void to_json(nlohmann::json& j, const TextureWrap& textureWrap) {
    JSON_MARSHAL_ENUM(TextureWrap, textureWrap);
}

static void to_json(nlohmann::json& j, const TextureFormat& textureFormat) {
    JSON_MARSHAL_ENUM(TextureFormat, textureFormat);
}

static void to_json(nlohmann::json& j, const DisplayMode& displayMode) {
    j = nlohmann::json::object();
    j["width"] = displayMode.width;
    j["height"] = displayMode.height;
}

static void from_json(const nlohmann::json& j, DisplayMode& displayMode) {
    JSON_UNMARSHAL_VAR(displayMode, unsigned int, width);
    JSON_UNMARSHAL_VAR(displayMode, unsigned int, height);
}


static void to_json(nlohmann::json& j, const GuiSettings& gui) {
    j = nlohmann::json::object();
    j["logFile"] = gui.logFile;
    j["logFileAppend"] = gui.logFileAppend;
    j["projectPath"] = gui.projectPath;
    j["tool"] = gui.tool;
    j["profiler"] = gui.profiler;
    j["profilerListener"] = gui.profilerListener;
    j["editor"] = gui.editor;
    j["fileModifyGracePeriod"] = gui.fileModifyGracePeriod;
    j["largeFileModifyGracePeriod"] = gui.largeFileModifyGracePeriod;
    j["fileHistorySize"] = gui.fileHistorySize;
    j["fileRefreshThreadSleep"] = gui.fileRefreshThreadSleep;
    j["startPosition"] = gui.startPosition;    
    j["gnuRocketHost"] = gui.gnuRocketHost;
    j["gnuRocketPort"] = gui.gnuRocketPort;
    j["glslValidatorHealthCommand"] = gui.glslValidatorHealthCommand;
    j["glslValidatorCommand"] = gui.glslValidatorCommand;
    j["glslValidator"] = gui.glslValidator;
    j["diffHealthCommand"] = gui.diffHealthCommand;
    j["diffCommand"] = gui.diffCommand;
    j["diff"] = gui.diff;
}

static void from_json(const nlohmann::json& j, GuiSettings& gui) {
    JSON_UNMARSHAL_VAR(gui, std::string, logFile);
    JSON_UNMARSHAL_VAR(gui, bool, logFileAppend);
    JSON_UNMARSHAL_VAR(gui, std::string, projectPath);
    JSON_UNMARSHAL_VAR(gui, bool, tool);
    JSON_UNMARSHAL_VAR(gui, bool, profiler);
    JSON_UNMARSHAL_VAR(gui, bool, profilerListener);
    JSON_UNMARSHAL_VAR(gui, bool, editor);
    JSON_UNMARSHAL_VAR(gui, int, fileModifyGracePeriod);
    JSON_UNMARSHAL_VAR(gui, int, largeFileModifyGracePeriod);
    JSON_UNMARSHAL_VAR(gui, int, fileHistorySize);
    JSON_UNMARSHAL_VAR(gui, int, fileRefreshThreadSleep);
    JSON_UNMARSHAL_VAR(gui, double, startPosition);
    JSON_UNMARSHAL_VAR(gui, std::string, gnuRocketHost);
    JSON_UNMARSHAL_VAR(gui, unsigned short, gnuRocketPort);
    JSON_UNMARSHAL_VAR(gui, std::string, glslValidatorHealthCommand);
    JSON_UNMARSHAL_VAR(gui, std::string, glslValidatorCommand);
    JSON_UNMARSHAL_VAR(gui, bool, glslValidator);
    JSON_UNMARSHAL_VAR(gui, std::string, diffHealthCommand);
    JSON_UNMARSHAL_VAR(gui, std::string, diffCommand);
    JSON_UNMARSHAL_VAR(gui, bool, diff);
}

static void to_json(nlohmann::json& j, const WindowSettings& window) {
    j = nlohmann::json::object();
    j["height"] = window.height;
    j["width"] = window.width;
    j["fullscreen"] = window.fullscreen;
    j["verticalSync"] = window.verticalSync;
}

static void from_json(const nlohmann::json& j, WindowSettings& window) {
    JSON_UNMARSHAL_VAR(window, unsigned int, height);
    JSON_UNMARSHAL_VAR(window, unsigned int, width);
    JSON_UNMARSHAL_VAR(window, bool, fullscreen);
    JSON_UNMARSHAL_VAR(window, bool, verticalSync);

    //setup canvas position and screen areas
    window.setWindowDimensions(window.width, window.height);
}

static void to_json(nlohmann::json& j, const AudioSettings& audio) {
    j = nlohmann::json::object();
    j["mute"] = audio.mute;
    j["capture"] = audio.capture;
    j["timeSource"] = audio.timeSource;
    j["samples"] = audio.samples;
    j["captureMixVolume"] = audio.captureMixVolume;
    j["device"] = audio.device;
}

static void from_json(const nlohmann::json& j, AudioSettings& audio) {
    JSON_UNMARSHAL_VAR(audio, bool, mute);
    JSON_UNMARSHAL_VAR(audio, bool, capture);
    JSON_UNMARSHAL_VAR(audio, bool, timeSource);
    JSON_UNMARSHAL_VAR(audio, unsigned short, samples);
    JSON_UNMARSHAL_VAR(audio, double, captureMixVolume);
    JSON_UNMARSHAL_VAR(audio, std::string, device);
}


static void to_json(nlohmann::json& j, const LoggerSettings& logger) {
    j = nlohmann::json::object();
    j["showMessageBox"] = logger.showMessageBox;
    j["logLevel"] = logger.logLevel;
    j["exitLogLevel"] = logger.exitLogLevel;
    j["titleLogLevel"] = logger.titleLogLevel;
    j["pauseLogLevel"] = logger.pauseLogLevel;
    j["duplicateLogGracePeriod"] = logger.duplicateLogGracePeriod;
}

static void from_json(const nlohmann::json& j, LoggerSettings& logger) {
    JSON_UNMARSHAL_VAR(logger, bool, showMessageBox);
    JSON_UNMARSHAL_VAR(logger, int, logLevel);
    JSON_UNMARSHAL_VAR(logger, int, exitLogLevel);
    JSON_UNMARSHAL_VAR(logger, int, titleLogLevel);
    JSON_UNMARSHAL_VAR(logger, int, pauseLogLevel);
    JSON_UNMARSHAL_VAR(logger, uint64_t, duplicateLogGracePeriod);
}


static void to_json(nlohmann::json& j, const FftSettings& fft) {
    j = nlohmann::json::object();
    j["enable"] = fft.enable;
    j["divisor"] = fft.divisor;
    j["clipMin"] = fft.clipMin;
    j["clipMax"] = fft.clipMax;
    j["size"] = fft.size;
    j["history"] = fft.history;
}

static void from_json(const nlohmann::json& j, FftSettings& fft) {
    JSON_UNMARSHAL_VAR(fft, bool, enable);
    JSON_UNMARSHAL_VAR(fft, double, divisor);
    JSON_UNMARSHAL_VAR(fft, double, clipMin);
    JSON_UNMARSHAL_VAR(fft, double, clipMax);
    JSON_UNMARSHAL_VAR(fft, unsigned int, size);
    JSON_UNMARSHAL_VAR(fft, unsigned int, history);
}

static void to_json(nlohmann::json& j, const ModelSettings& model) {
    j = nlohmann::json::object();
    j["joinIdenticalVertices"] = model.joinIdenticalVertices;
    j["triangulate"] = model.triangulate;
    j["generateNormals"] = model.generateNormals;
    j["validate"] = model.validate;
    j["removeRedundant"] = model.removeRedundant;
    j["fixInvalidData"] = model.fixInvalidData;
    j["optimizeMeshes"] = model.optimizeMeshes;
    j["optimizeGraph"] = model.optimizeGraph;
}

static void from_json(const nlohmann::json& j, ModelSettings& model) {
    JSON_UNMARSHAL_VAR(model, bool, joinIdenticalVertices);
    JSON_UNMARSHAL_VAR(model, bool, triangulate);
    JSON_UNMARSHAL_VAR(model, bool, generateNormals);
    JSON_UNMARSHAL_VAR(model, bool, validate);
    JSON_UNMARSHAL_VAR(model, bool, removeRedundant);
    JSON_UNMARSHAL_VAR(model, bool, fixInvalidData);
    JSON_UNMARSHAL_VAR(model, bool, optimizeMeshes);
    JSON_UNMARSHAL_VAR(model, bool, optimizeGraph);
}

static void to_json(nlohmann::json& j, const GraphicsSettings& graphics) {
    j = nlohmann::json::object();
    j["displayModes"] = graphics.displayModes;
    j["model"] = graphics.model;
    j["clearColor"] = graphics.clearColor;
    j["canvasHeight"] = graphics.canvasHeight;
    j["canvasWidth"] = graphics.canvasWidth;
    j["aspectRatio"] = graphics.aspectRatio; //TODO: do we want to store aspect ratio?

    j["defaultTextureDataType"] = graphics.defaultTextureDataType;
    j["defaultTextureTargetType"] = graphics.defaultTextureTargetType;
    j["defaultTextureType"] = graphics.defaultTextureType;
    j["defaultTextureFilter"] = graphics.defaultTextureFilter;
    j["defaultTextureWrap"] = graphics.defaultTextureWrap;
    j["defaultTextureFormat"] = graphics.defaultTextureFormat;

    j["defaultFboTextureFilter"] = graphics.defaultFboTextureFilter;
    j["defaultFboTextureWrap"] = graphics.defaultFboTextureWrap;

    j["shaderProgramDefault"] = graphics.shaderProgramDefault;
    j["shaderProgramDefaultShadow"] = graphics.shaderProgramDefaultShadow;

    j["maxTextureUnits"] = graphics.maxTextureUnits;
    j["maxActiveLightCount"] = graphics.maxActiveLightCount;

    // Graphics context data setup might be volatile, let's not yet implement settings support
    //j["requestedMajorVersion"] = graphics.requestedMajorVersion;
    //j["requestedMinorVersion"] = graphics.requestedMinorVersion;
}

static void from_json(const nlohmann::json& j, GraphicsSettings& graphics) {
    JSON_UNMARSHAL_VAR(graphics, std::vector<DisplayMode>, displayModes);

    JSON_UNMARSHAL_VAR(graphics, ModelSettings, model);

    JSON_UNMARSHAL_VAR(graphics, Color, clearColor);
    Graphics::getInstance().setClearColor(graphics.clearColor);

    JSON_UNMARSHAL_VAR(graphics, float, canvasHeight);
    JSON_UNMARSHAL_VAR(graphics, float, canvasWidth);

    JSON_UNMARSHAL_ENUM(graphics, TextureDataType, defaultTextureDataType);
    JSON_UNMARSHAL_ENUM(graphics, TextureTargetType, defaultTextureTargetType);
    JSON_UNMARSHAL_ENUM(graphics, TextureType, defaultTextureType);
    JSON_UNMARSHAL_ENUM(graphics, TextureFilter, defaultTextureFilter);
    JSON_UNMARSHAL_ENUM(graphics, TextureWrap, defaultTextureWrap);
    JSON_UNMARSHAL_ENUM(graphics, TextureFormat, defaultTextureFormat);

    JSON_UNMARSHAL_ENUM(graphics, TextureFilter, defaultFboTextureFilter);
    JSON_UNMARSHAL_ENUM(graphics, TextureWrap, defaultFboTextureWrap);

    JSON_UNMARSHAL_VAR(graphics, std::string, shaderProgramDefault);
    JSON_UNMARSHAL_VAR(graphics, std::string, shaderProgramDefaultShadow);

    JSON_UNMARSHAL_VAR(graphics, unsigned int, maxTextureUnits);
    JSON_UNMARSHAL_VAR(graphics, unsigned int, maxActiveLightCount);

    if (j.find("aspectRatio") != j.end()) {
        JSON_UNMARSHAL_VAR(graphics, float, aspectRatio);
    } else {
        graphics.calculateAspectRatio();
    }

    Settings::window.fitCanvasToWindow();

    //JSON_UNMARSHAL_VAR(graphics, unsigned int, requestedMajorVersion);
    //JSON_UNMARSHAL_VAR(graphics, unsigned int, requestedMinorVersion);
}

static void to_json(nlohmann::json& j, const DemoSettings& demo) {
    j = nlohmann::json::object();

    j["title"] = demo.title;
    j["rocketXmlFile"] = demo.rocketXmlFile;
    j["midiManagerFile"] = demo.midiManagerFile;
    j["graphics"] = demo.graphics;
    j["length"] = demo.length;
    j["song"] = demo.song;
    j["songLoop"] = demo.songLoop;
    j["targetFps"] = demo.targetFps;
    j["beatsPerMinute"] = demo.beatsPerMinute;
    j["rowsPerBeat"] = demo.rowsPerBeat;
    j["fft"] = demo.fft;
    j["custom"] = demo.custom;
    j["networking"] = demo.networking;
}

static void from_json(const nlohmann::json& j, DemoSettings& demo) {
    JSON_UNMARSHAL_VAR(demo, FftSettings, fft);
    JSON_UNMARSHAL_VAR(demo, GraphicsSettings, graphics);
    JSON_UNMARSHAL_VAR(demo, double, length);
    JSON_UNMARSHAL_VAR(demo, std::string, song);
    JSON_UNMARSHAL_VAR(demo, bool, songLoop);
    JSON_UNMARSHAL_VAR(demo, std::string, title);
    JSON_UNMARSHAL_VAR(demo, std::string, rocketXmlFile);
    JSON_UNMARSHAL_VAR(demo, std::string, midiManagerFile);

    JSON_UNMARSHAL_VAR(demo, double, targetFps);
    JSON_UNMARSHAL_VAR(demo, double, beatsPerMinute);
    JSON_UNMARSHAL_VAR(demo, double, rowsPerBeat);

    if (j.find("custom") != j.end()) { demo.custom = j.at("custom"); }

    JSON_UNMARSHAL_VAR(demo, bool, networking);

    EnginePlayer::getInstance().getFps().setTargetFps(demo.targetFps);

    EnginePlayer::getInstance().getTimer().setBeatsPerMinute(demo.beatsPerMinute);

    SyncRocket& sync = dynamic_cast<SyncRocket&>(EnginePlayer::getInstance().getSync());
    sync.setRowsPerBeat(demo.rowsPerBeat);
}

std::string Settings::serialize() {
    nlohmann::json jsonSettings = nlohmann::json::object();
    // DemoSettings not saved as considered demo specific and handled via script.json, no serialization here
    jsonSettings["gui"] = Settings::gui;
    jsonSettings["window"] = Settings::window;
    jsonSettings["logger"] = Settings::logger;
    jsonSettings["audio"] = Settings::audio;
    jsonSettings["showMenu"] = Settings::showMenu;

    return jsonSettings.dump(4);
}

static void deserializeSettingsJson(const nlohmann::json& j) {
    if (j.find("gui") != j.end()) {
        Settings::gui = j.at("gui").get<GuiSettings>();
    }

    if (j.find("window") != j.end()) {
        Settings::window = j.at("window").get<WindowSettings>();
    }

    if (j.find("logger") != j.end()) {
        Settings::logger = j.at("logger").get<LoggerSettings>();
    }

    if (j.find("audio") != j.end()) {
        Settings::audio = j.at("audio").get<AudioSettings>();
    }

    if (j.find("showMenu") != j.end()) {
        Settings::showMenu = j.at("showMenu").get<bool>();
    }
}

std::string Settings::settingsFile = "settings.json";

// TODO: GCC does not want to compile nlohmann::json::parse_error &e, so we're forced to use broader-than-comfortable std::exception catching...

bool Settings::loadSettings(std::string file) {
    if (file == "") {
        file = Settings::settingsFile;
    }

    std::ifstream input(file);
    if(!input.is_open()) {
        loggerTrace("Settings file not found: '%s'", file.c_str());
        return false;
    }

    try {
        loggerInfo("Loading settings. file:'%s'", file.c_str());

        nlohmann::json jsonSettings;
        input >> jsonSettings;

        deserializeSettingsJson(jsonSettings);
    } catch (const std::exception& e) {
        loggerError("Could not parse json! file:'%s', message:'%s'", file.c_str(), e.what());
        return false;
    }

    settingsFile = file;

    return true;
}

bool Settings::loadSettingsFromString(std::string data) {
    std::stringstream input(data);

    try {
        loggerDebug("Loading settings from string.");

        nlohmann::json jsonSettings;
        input >> jsonSettings;

        deserializeSettingsJson(jsonSettings);
    } catch (const std::exception& e) {
        loggerError("Could not parse json! data:'%s', message:'%s'", data.c_str(), e.what());
        return false;
    }

    return true;
}

bool Settings::saveSettings(std::string file) {
    if (file == "") {
        file = Settings::settingsFile;
    }

    try {
        loggerInfo("Saving settings. file:'%s'", file.c_str());

        std::string jsonSettings = serialize();

        std::ofstream output(file);
        output << jsonSettings << std::endl;
    } catch (const std::exception& e) {
        loggerError("Could not parse json! file:'%s', message:'%s'", file.c_str(), e.what());
        return false;
    }

    return true;
}


// Demo specific settings, a.k.a. script.json

DemoSettings::DemoSettings() {
    length = -1;
    song = "music.ogg";
    songLoop = false;
    title = "AV experience";
    midiManagerFile = "syncMidi.json";
    rocketXmlFile = "sync.rocket";

    targetFps = 500.0;
    beatsPerMinute = 100.0;
    rowsPerBeat = 8.0;

    networking = false;

    custom = nlohmann::json::object();
}

FftSettings::FftSettings() {
    enable = false;
    divisor = 25.0;
    clipMin = 0.0;
    clipMax = 1.0;
    size = 256;
    history = 16;
}

ModelSettings::ModelSettings() {
    joinIdenticalVertices = true;
    triangulate = true;
    generateNormals = true;
    validate = true;
    removeRedundant = true;
    fixInvalidData = true;
    optimizeMeshes = false;
    optimizeGraph = false;
}

GraphicsSettings::GraphicsSettings() : clearColor(0, 0, 0, 0) {
    // OpenGL 3.3 should be enough generally available, so let's stick with that
    // Semi ref: http://feedback.wildfiregames.com/report/opengl/
    // Also aim to have feature set close to WebGL 2 / OpenGL ES range capabilities compatibility
    // GLSL lang ref: https://www.khronos.org/registry/OpenGL/specs/gl/GLSLangSpec.3.30.pdf
    requestedMajorVersion = 3;
    requestedMinorVersion = 3;

    maxTextureUnits = 4;
    maxActiveLightCount = 4;

    defaultTextureDataType = TextureDataType::UNSIGNED_BYTE;
    defaultTextureTargetType = TextureTargetType::TEXTURE_2D;
    defaultTextureType = TextureType::DIFFUSE;
    defaultTextureFilter = TextureFilter::MIPMAP;
    defaultTextureWrap = TextureWrap::CLAMP_TO_EDGE;
    defaultTextureFormat = TextureFormat::RGBA;

    defaultFboTextureFilter = TextureFilter::MIPMAP;
    defaultFboTextureWrap = TextureWrap::CLAMP_TO_EDGE;

    shaderProgramDefault = "Default";
    shaderProgramDefaultShadow = "DefaultPlain";

    setCanvasDimensions(1920.0f, 1080.0f);

    displayModes = std::vector<DisplayMode>();
    displayModes.push_back(DisplayMode(1920, 1080));
    displayModes.push_back(DisplayMode(1280, 720));
    displayModes.push_back(DisplayMode(1024, 768));
    displayModes.push_back(DisplayMode(800, 600));
    displayModes.push_back(DisplayMode(640, 480));
}

void GraphicsSettings::setCanvasDimensions(float width, float height) {
    canvasWidth = width;
    canvasHeight = height;

    calculateAspectRatio();
}

void GraphicsSettings::calculateAspectRatio() {
    aspectRatio = canvasWidth / canvasHeight;
}

std::string DemoSettings::serialize() {
    nlohmann::json scriptJson = Settings::demo;

    return scriptJson.dump(4);
}

static void deserializeScriptJson(const nlohmann::json& j) {
    Settings::demo = j.get<DemoSettings>();
}

// TODO: GCC does not want to compile nlohmann::json::parse_error &e, so we're forced to use broader-than-comfortable std::exception catching...

bool DemoSettings::loadDemoSettings(std::string file) {
    File scriptJsonFile = File(file);
    file = scriptJsonFile.getFilePath();

    std::ifstream input(file);
    if(!input.is_open()) {
        loggerTrace("Settings file not found: '%s'", file.c_str());
        return false;
    }

    try {
        loggerDebug("Loading demo settings. file:'%s'", file.c_str());

        nlohmann::json scriptJson;
        input >> scriptJson;

        deserializeScriptJson(scriptJson);
    } catch (const std::exception& e) {
        loggerError("Could not parse json! file:'%s', message:'%s'", file.c_str(), e.what());
        return false;
    }

    return true;
}

bool DemoSettings::loadDemoSettingsFromString(std::string data) {
    std::stringstream input(data);

    try {
        loggerDebug("Loading demo settings from string.");

        nlohmann::json jsonSettings;
        input >> jsonSettings;

        deserializeScriptJson(jsonSettings);
    } catch (const std::exception& e) {
        loggerError("Could not parse json! data:'%s', message:'%s'", data.c_str(), e.what());
        return false;
    }

    return true;
}

bool DemoSettings::saveDemoSettings(std::string file) {
    try {
        File scriptJsonFile = File(file);
        if (scriptJsonFile.exists()) {
            file = scriptJsonFile.getFilePath();
        }

        loggerInfo("Saving settings. file:'%s'", file.c_str());

        std::string scriptJson = serialize();

        std::ofstream output(file);
        output << scriptJson << std::endl;
    } catch (const std::exception& e) {
        loggerError("Could not parse json! file:'%s', message:'%s'", file.c_str(), e.what());
        return false;
    }

    return true;
}
