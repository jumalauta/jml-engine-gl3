#include "EnginePlayer.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <map>

#include <math.h>

#include "version.h"
#include "Settings.h"
#include "logger/logger.h"

#include "time/TimeFormatter.h"
#include "time/SystemTime.h"
#include "time/Timer.h"
#include "time/Fps.h"

#include "io/FileRefreshManager.h"
#include "io/NetworkManager.h"
#include "io/EmbeddedResourceManager.h"
#include "io/MemoryManager.h"
#include "io/MidiManager.h"

#include "ui/Gui.h"
#include "ui/Input.h"
#include "ui/Window.h"
#include "ui/WindowSdl.h"
#include "ui/menu/Menu.h"
#include "audio/Audio.h"
#include "audio/AudioFile.h"
#include "graphics/Graphics.h"
#include "graphics/GraphicsOpenGl.h"
#include "graphics/Camera.h"
#include "sync/Sync.h"
#include "sync/SyncRocket.h"
#include "math/TransformationMatrix.h"

#include "graphics/LightManager.h"
#include "graphics/Light.h"
#include "graphics/Image.h"
#include "graphics/Texture.h"
#include "graphics/Font.h"
#include "graphics/TextureOpenGl.h"
#include "graphics/Fbo.h"
#include "graphics/model/TexturedQuad.h"
#include "graphics/model/Model.h"
#include "graphics/video/VideoFile.h"
#include "graphics/Shadow.h"

#include "graphics/Shader.h"
#include "graphics/ShaderProgram.h"
#include "graphics/ShaderProgramOpenGl.h"
#include "script/Script.h"
#include "script/ScriptEngine.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

extern void fftTextureInit();
extern void fftTextureDeinit();
extern void fftTextureUpdate();
extern void fftDataSamplePosition(double position);
extern double getFftDataHistoryBufferTime();

static bool connectMidi() {
    MidiManager& midiManager = EnginePlayer::getInstance().getMidiManager();
    midiManager.exit();

    bool success = false;
    MidiController* midiController = midiManager.addMidiController();
    if (midiController->init()) {
        std::vector<MidiPort> portList = midiController->listAvailablePorts();
        if (!portList.empty()) {
            if (midiController->connect(portList[0].number)) {
                success = true;
            }
        }
    }

    if (!success) {
        midiManager.exit();
    }

    return success;
}

// TODO: 3D handling gizmo option: https://github.com/CedricGuillemet/ImGuizmo

class Recorder {
public:
    Recorder() {
        recording = false;
        fps = 60.0; // Standard FPS
        stop = false;
        captureFps = 0.0;
        progress = 0.0;
        outputSize = 0.0;
        recordStartTime = 0.0;
        recordEndTime = Settings::demo.length;
        encode = false;
        encodeCommand = "";
    }

    void setFps(double fps) {
        this->fps = fps;
    }

    void setStop(bool stop = true) {
        this->stop = true;
    }

    double getCaptureFps() {
        return captureFps;
    }

    double getProgress() {
        return progress;
    }

    double getOutputSize() {
        return outputSize;
    }

    double getRecordStartTime() {
        return recordStartTime;
    }

    void setRecordStartTime(double recordStartTime) {
        this->recordStartTime = recordStartTime;
    }

    double getRecordEndTime() {
        return recordEndTime;
    }

    void setRecordEndTime(double recordEndTime) {
        this->recordEndTime = recordEndTime;
    }

    bool getEncode() {
        return encode;
    }
    void setEncode(bool encode) {
        this->encode = encode;
    }

    const std::string& getEncodeCommand() {
        return encodeCommand;
    }
    void setEncodeCommand(std::string encodeCommand) {
        this->encodeCommand = encodeCommand;
    }

    bool capture(Fbo &fbo) {
        stop = false;
        bool status = false;

        if (isRecording()) {
            loggerWarning("Can't capture, recording on-going");
            return status;
        }

        loggerInfo("Video capture requested!");

        uint64_t loadStart = SystemTime::getTimeInMillis();

        recording = true;

        char fileName[256] = {'\0'};

        FILE * videoFile = NULL;
        std::FILE* encodeProcess = NULL;
        std::string command = getEncodeCommand();
        if (encode) {
            sprintf(fileName, "engine_output_%dx%d.mp4", fbo.getWidth(), fbo.getHeight());

            std::string widthWord = "<width>";
            while (command.find(widthWord) != std::string::npos) {
                command.replace(command.find(widthWord), widthWord.length(), std::to_string(fbo.getWidth()));
            }
            std::string heightWord = "<height>";
            while (command.find(heightWord) != std::string::npos) {
                command.replace(command.find(heightWord), heightWord.length(), std::to_string(fbo.getHeight()));
            }

            std::string audioFileWord = "<audioFile>";
            while (command.find(audioFileWord) != std::string::npos) {
                std::string audioFileString = "";
                File *audioFile = EnginePlayer::getInstance().getAudio().getAudioFile();
                if (audioFile) {
                    audioFileString = std::string("\"") + audioFile->getFilePath() + std::string("\"");
                } else {
                    loggerWarning("No audio file found, but it's required by the capture");
                    return status;
                }
                command.replace(command.find(audioFileWord), audioFileWord.length(), audioFileString);
            }
            std::string outputFileWord = "<outputFile>";
            while (command.find(outputFileWord) != std::string::npos) {

                std::string outputFileString = std::string("\"") + std::string(fileName) + std::string("\"");
                command.replace(command.find(outputFileWord), outputFileWord.length(), outputFileString);
            }
            std::string fpsWord = "<fps>";
            while (command.find(fpsWord) != std::string::npos) {
                command.replace(command.find(fpsWord), fpsWord.length(), std::to_string(static_cast<int>(fps)));
            }


            encodeProcess = popen(command.c_str(), "wb");
            if (!encodeProcess) {
                loggerWarning("Could not encode with ffmpeg. command:'%s'", command.c_str());
                return status;
            }
        } else {
            sprintf(fileName, "engine_rawvideo_%dx%d.rgb24", fbo.getWidth(), fbo.getHeight());
            videoFile = fopen(fileName, "wb");
            if (videoFile == NULL) {
                loggerWarning("Could not write video!");
                recording = false;
                return status;
            }
        }

        int channels = 3; //RGB
        size_t rawFrameSize = fbo.getWidth() * fbo.getHeight() * channels;
        unsigned char *pixels = new unsigned char[rawFrameSize];
        if (pixels == NULL) {
            loggerFatal("Could not allocate memory for video capture");
            return status;
        }

        EnginePlayer& enginePlayer = EnginePlayer::getInstance();
        Timer& timer = enginePlayer.getTimer();
        Audio& audio = enginePlayer.getAudio();

        bool paused = timer.isPause();

        bool muted = Settings::audio.mute;
        Settings::audio.mute = true;

        int pauseLogLevel = Settings::logger.pauseLogLevel;
        Settings::logger.pauseLogLevel = Settings::logger.exitLogLevel;

        timer.synchronizeToAudio(NULL);

        if (!paused) {
            timer.pause(true);
        }
        double timeNow = timer.getTimeInSeconds();

        timer.setTimeInSeconds(recordStartTime);


        if (Settings::demo.fft.enable) {
            audio.stop();
            audio.play(Settings::demo.song.c_str());
            audio.pause(false);
            SystemTime::sleepInMillis(1000);
        }

        const double FRAME_INCREMENT = 1.0 / fps;

        unsigned char *flippedRawData = new unsigned char[rawFrameSize];
        if (flippedRawData == NULL) {
            loggerFatal("Could not allocate memory for image writing");
            return false;
        }

        status = true;
        unsigned int frame = 0;
        outputSize = 0.0;
        double capturingTime = 0.0;
        while (!enginePlayer.getInput().isUserExit() && !stop) {
            frame++;

            if (Settings::demo.fft.enable) {
                // buffering FFT data
                for(int i = 0; i < 10; i++) {
                    if (timer.getTimeInSeconds() > getFftDataHistoryBufferTime()) {
                        SystemTime::sleepInMillis(100);
                    }
                }

                audio.pause(true);

                fftDataSamplePosition((frame - 1) / fps);
            }

            enginePlayer.forceRedraw();
            // TODO: perf can be semi-greatly improved by removing sleeps and redundant code
            enginePlayer.processFrame();

            if (Settings::demo.fft.enable) {
                audio.pause(false);
            }

            fbo.bind();
            glReadPixels(0, 0, fbo.getWidth(), fbo.getHeight(), GL_RGB, GL_UNSIGNED_BYTE, static_cast<void*>(pixels));
            fbo.unbind();

            // flip vertically
            const size_t lineSize = fbo.getWidth() * channels;
            for(size_t pos = 0; pos < rawFrameSize; pos += lineSize) {
                memcpy(flippedRawData + pos, pixels + (rawFrameSize - lineSize - pos), lineSize);
            }

            size_t ret = 0;
            if (encodeProcess) {
                ret = fwrite(flippedRawData, sizeof(unsigned char), rawFrameSize, encodeProcess);
                fflush(encodeProcess);
            }

            if (videoFile) {
                ret = fwrite(flippedRawData, sizeof(unsigned char), rawFrameSize, videoFile);
                fflush(videoFile);
            }
            if (ret != rawFrameSize) {
                perror("Could not allocate memory for video capture");
                loggerWarning("Could not successfully write frame! captureTime:%.3f, ret:%d, rawFrameSize:%d", timer.getTimeInSeconds(), ret, rawFrameSize);
                status = false;
                break;
            }

            outputSize += ret / 1024. / 1024.;

            timer.setTimeInSeconds((frame - 1) / fps);

            if (timer.getTimeInSeconds() >= recordEndTime) {
                status = true;
                break;
            }

            progress = (timer.getTimeInSeconds() - recordStartTime) / (recordEndTime - recordStartTime);

            capturingTime = (static_cast<double>((SystemTime::getTimeInMillis() - loadStart)) / 1000.0);
            captureFps = frame / capturingTime;

            //fprintf(stdout, "Capturing video: %.2f %% (%.2f MB)\r", progress, outputSize);
            //fflush(stdout);
        }

        if (encodeProcess) {
            pclose(encodeProcess);
        }

        if (videoFile) {
            fclose(videoFile);
        }

        delete [] pixels;
        delete [] flippedRawData;

        recording = false;

        loggerInfo("Captured video! file:'%s' frames:%u, fps:%.2f, startTime:%.2f, endTime:%.2f, rawOutputSize: %.2f MB, capturingTime:%.2f seconds",
            fileName, frame, fps, recordStartTime, recordEndTime, outputSize, capturingTime);

        timer.synchronizeToAudio(&audio);
        timer.setTimeInSeconds(timeNow);

        Settings::logger.pauseLogLevel = pauseLogLevel;
        Settings::audio.mute = muted;

        timer.pause(paused);

        return status;
    }

    bool isRecording() {
        return recording;
    }

private:
    bool recording;
    bool stop;
    bool encode;
    std::string encodeCommand;
    double fps;
    double captureFps;
    double progress;
    double outputSize;
    double recordStartTime;
    double recordEndTime;
};

static Recorder* recorder = NULL;

/*static ImVec2 value_raw = ImVec2(0,0);
static ImVec2 mouseDelta = ImVec2(0,0);
static ImVec2 itemRectSize = ImVec2(100,40);
static bool resizeDrag = false;
*/

class TimelineElement {
public:
    TimelineElement(int layer, int start, int duration, const char *name);
    void setLayer(int layer);
    void setStart(int start);
    void setDuration(int duration);
    void calculateElementDetails();
    void draw();
private:
    int layer;
    int start;
    int duration;
    int scrollingY;
    int scrollingX;
    int spacingX;
    int spacingY;

    ImVec2 startPos;
    ImVec2 value_raw;
    ImVec2 mouseDelta;
    ImVec2 itemRectSize;
    bool resizeDrag;
    const char *name;
};

TimelineElement::TimelineElement(int layer, int start, int duration, const char *name) {
    this->name = name;

    scrollingY = 400;
    scrollingX = 500;
    spacingX = 100;
    spacingY = 40;

    value_raw = ImVec2(0,0);
    mouseDelta = ImVec2(0,0);
    itemRectSize = ImVec2(0, spacingY);
    resizeDrag = false;

    startPos = ImGui::GetCursorPos();
    setLayer(layer);
    setStart(start);
    setDuration(duration);
}

void TimelineElement::setLayer(int layer) {
    this->layer = layer;
    startPos.y += spacingY * layer;
}

void TimelineElement::setStart(int start) {
    this->start = start;
    startPos.x += (start / 1000.0) * spacingX;
}

void TimelineElement::setDuration(int duration) {
    this->duration = duration;
    itemRectSize.x = (duration / 1000.0) * spacingX;
}

void TimelineElement::calculateElementDetails() {
    //setLayer((startPos.y / static_cast<double>(spacingY)) + 0.5);
    //setStart((startPos.x / static_cast<double>(spacingX)) * 1000.0);
    //setDuration((itemRectSize.x / static_cast<double>(spacingX)) * 1000.0);
    //loggerInfo("layer: %d -> %d", this->layer, (startPos.y / static_cast<double>(spacingY)) + 0.5);
    //loggerInfo("start: %.2f -> %.2f", this->start, (startPos.x / static_cast<double>(spacingX)) * 1000.0);
    //loggerInfo("duration: %.2f -> %.2f", this->duration, (itemRectSize.x / static_cast<double>(spacingX)) * 1000.0);
}

void TimelineElement::draw() {
    int scrollingY = 400;
    int scrollingX = 500;
    int spacingX = 100;
    int spacingY = 40;

    ImGui::SetCursorPos(startPos);

    ImGui::Button(name, itemRectSize);

    itemRectSize = ImGui::GetItemRectSize();
    ImVec2 mousePosition = ImGui::GetMousePos();

    int buttonRightSide = (ImGui::GetWindowPos().x + startPos.x + itemRectSize.x);
    if (ImGui::IsItemHovered() && mousePosition.x - buttonRightSide > -5) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        ImGui::GetIO().MouseDrawCursor = true;

        resizeDrag = ImGui::IsMouseDown(0);
    }

    if (!ImGui::IsMouseDown(0)) {
        resizeDrag = false;
    }

    if (resizeDrag) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        ImGui::GetIO().MouseDrawCursor = true;

        itemRectSize.x += mousePosition.x - buttonRightSide;
        calculateElementDetails();
    } else {
        if (ImGui::IsItemActive())
        {
            value_raw = ImGui::GetMouseDragDelta(0, 0.0f);
            ImVec2 value_with_lock_threshold = ImGui::GetMouseDragDelta(0);
            ImVec2 mouse_delta = ImGui::GetIO().MouseDelta;

            // Draw a line between the button and the mouse cursor
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            draw_list->PushClipRectFullScreen();
            ImVec2 buttonCenter = ImGui::GetWindowPos();
            buttonCenter.x += startPos.x + itemRectSize.x / 2;
            buttonCenter.y += startPos.y + itemRectSize.y / 2;

            ImGui::SetCursorPos(ImVec2(startPos.x + value_raw.x, startPos.y + value_raw.y));
            ImGui::Button(name, itemRectSize);

            draw_list->AddLine(buttonCenter, ImGui::GetIO().MousePos, ImColor(ImGui::GetStyle().Colors[ImGuiCol_Button]), 2.0f);
            draw_list->PopClipRect();

            ImGui::SameLine(); ImGui::Text("Raw (%.1f, %.1f), WithLockThresold (%.1f, %.1f), MouseDelta (%.1f, %.1f)", value_raw.x, value_raw.y, value_with_lock_threshold.x, value_with_lock_threshold.y, mouse_delta.x, mouse_delta.y);
        } else {
            float pos = -1;
            if (startPos.y + value_raw.y < startPos.y) {
                pos = 1;
            }
            startPos.x += value_raw.x;
            startPos.y += value_raw.y;
            float modulo = fmod(startPos.y, spacingY);
            //if (modulo < spacingY) { modulo *= -1; }
            startPos.y += modulo * pos;

            value_raw.x = 0;
            value_raw.y = 0;

            calculateElementDetails();
        }
    }
}

struct TimelineWidget
{
    ImVec2 windowPosition;
    ImVec2 windowSize;

    void setWindowSize(double width, double height) {
        windowSize.x = width;
        windowSize.y = height;
    }

    void setWindowPosition(double x, double y) {
        windowPosition.x = x;
        windowPosition.y = y;
    }

    TimelineWidget() {
        windowPosition = ImVec2(0,0);
        Window& window = *EnginePlayer::getInstance().getWindow(WindowType::EDITOR);
        windowSize = ImVec2(window.getWidth() / 2.0, window.getHeight() / 2.0);
    }

    void Draw(const char* title, bool* p_open = NULL)
    {
        ImGui::SetNextWindowPos(windowPosition, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);
        ImGui::Begin(title, p_open);

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,0));

        ImGui::BeginChild("scrolling", ImVec2(0,0), false, ImGuiWindowFlags_HorizontalScrollbar);

        static int lines = 20;
        //ImGui::SliderInt("Lines", &lines, 1, 15);
        int scrollingY = windowSize.y;
        int scrollingX = windowSize.x;
        int spacingX = 100;
        int spacingY = 40;

        EnginePlayer& enginePlayer = EnginePlayer::getInstance();
        double timeProgressPosition = scrollingX * (enginePlayer.getTimer().getTimeInSeconds() / Settings::demo.length);

        static ImVec2 startPos = ImGui::GetCursorPos();

        static const int histogramTestSize = 1000;
        static float histogramTest[histogramTestSize];
        static bool calcNow = true;
        static float minValue = -1.0f;
        static float maxValue = 1.0f;
        if (calcNow) {
            //FIXME: AudioFile refresh should refresh the histogram
            AudioFile *audioFile = EnginePlayer::getInstance().getAudio().getAudioFile();
            if (audioFile) {
                calcNow = false;
                //histogramTest = new float[histogramTestSize];
                audioFile->calculateHistogram(histogramTest, histogramTestSize, minValue, maxValue);
            }
        }

        //TODO: Support!
        //static TimelineElement e1 = TimelineElement(2,500,1000,"image.png");
        //static TimelineElement e2 = TimelineElement(0,1700,2250,"poster.png");
        ImGui::BeginChild("scrolling", ImVec2(0, scrollingY), true, ImGuiWindowFlags_HorizontalScrollbar);

        //FIXME: Implement AudioCalculation & draw thingie!
        //static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
        // TODO: PlotHistogram is better but does not support negative values, this support added to master after 1.50 release, wait for new release
        ImGui::SetCursorPos(startPos);
        ImGui::PlotLines("", histogramTest, IM_ARRAYSIZE(histogramTest), 0, NULL, minValue, maxValue, ImVec2(scrollingX, scrollingY));
        //ImGui::PlotHistogram("Lines", histogramTest, histogramTestSize, 0, NULL, minValue, maxValue, ImVec2(scrollingX, scrollingY));

        ImGui::SetCursorPos(startPos);

        //static TimelineElement e2 = TimelineElement("poster.png");

        //e1.draw();
        //e2.draw();

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        //drawList->AddLine(ImVec2(ImGui::GetWindowPos().x+100,ImGui::GetWindowPos().y-200), ImVec2(ImGui::GetWindowPos().x+100,ImGui::GetWindowPos().y+400-100), ImColor::HSV(0.9, 0.7f, 0.7f), 1.0f);

        //Draw timeline element grid
        for(int col = 0; col < scrollingX/spacingX; col++) {
            drawList->AddLine(
                ImVec2(ImGui::GetWindowPos().x+col*spacingX,ImGui::GetWindowPos().y),
                ImVec2(ImGui::GetWindowPos().x+col*spacingX,ImGui::GetWindowPos().y+scrollingY),
                ImColor::HSV(0.9, 0.7f, 0.7f), 1.0f);
        }
        for(int row = 0; row < scrollingY/spacingY; row++) {
            drawList->AddLine(
                ImVec2(ImGui::GetWindowPos().x,ImGui::GetWindowPos().y+row*spacingY),
                ImVec2(ImGui::GetWindowPos().x+scrollingX,ImGui::GetWindowPos().y+row*spacingY),
                ImColor::HSV(0.9, 0.7f, 0.7f), 1.0f);
        }

        //Draw progress line
        drawList->AddLine(ImVec2(ImGui::GetWindowPos().x+timeProgressPosition,ImGui::GetWindowPos().y), ImVec2(ImGui::GetWindowPos().x+timeProgressPosition,ImGui::GetWindowPos().y+scrollingY), ImColor::HSV(0.2, 0.7f, 0.7f), 1.0f);

        //Draw progress line
        //drawList->AddLine(ImVec2(ImGui::GetMousePos().x,ImGui::GetWindowPos().y), ImVec2(ImGui::GetMousePos().x,ImGui::GetWindowPos().y+scrollingY), ImColor::HSV(0.3, 0.7f, 0.7f), 1.0f);

        //draw_list->PushClipRectFullScreen();
        //drawList->AddLine(ImGui::GetWindowPos(), ImVec2(ImGui::GetWindowPos().x+areaSize.x,ImGui::GetWindowPos().y+areaSize.y), ImColor::HSV(0.5, 0.7f, 0.7f), 1.0f);
        //draw_list->PopClipRect();

        /*for (int line = 0; line < lines; line++)
        {
            // Display random stuff
            int num_buttons = 10 + ((line & 1) ? line * 9 : line * 3);
            for (int n = 0; n < num_buttons; n++)
            {
                if (n > 0) ImGui::SameLine();
                ImGui::PushID(n + line * 1000);
                char num_buf[16];
                const char* label = (!(n%15)) ? "FizzBuzz" : (!(n%3)) ? "Fizz" : (!(n%5)) ? "Buzz" : (sprintf(num_buf, "%d", n), num_buf);
                float hue = n*0.05f;
                ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(hue, 0.6f, 0.6f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(hue, 0.7f, 0.7f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(hue, 0.8f, 0.8f));
                ImGui::Button(label, ImVec2(40.0f + sinf((float)(line + n)) * 20.0f, 0.0f));
                ImGui::PopStyleColor(3);
                ImGui::PopID();
            }
        }*/

        /*ImGui::PlotLines("Sin", [](void*data, int idx) { return sinf(idx*0.2f); }, NULL, 100);
        static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
        ImGui::PlotLines("Curve", arr, IM_ARRAYSIZE(arr));*/


        ImGui::EndChild();
        float scroll_x_delta = 0.0f;
        ImGui::SmallButton("<<"); if (ImGui::IsItemActive()) scroll_x_delta = -ImGui::GetIO().DeltaTime * 1000.0f;
        ImGui::SameLine(); ImGui::Text("Scroll from code"); ImGui::SameLine();
        ImGui::SmallButton(">>"); if (ImGui::IsItemActive()) scroll_x_delta = +ImGui::GetIO().DeltaTime * 1000.0f;
        if (scroll_x_delta != 0.0f)
        {
            ImGui::BeginChild("scrolling"); // Demonstrate a trick: you can use Begin to set yourself in the context of another window (here we are already out of your child window)
            ImGui::SetScrollX(ImGui::GetScrollX() + scroll_x_delta);
            ImGui::End();
        }
        //ImGui::TreePop();

        ImGui::EndChild();
        ImGui::PopStyleVar(3);
        ImGui::End();
    }
};

// TODO: Actual editing support
struct TextEditWidget
{
    bool readOnly;
    File *file;

    TextEditWidget() {
        file = NULL;
        readOnly = true;
    }

    TextEditWidget(Script& script) : TextEditWidget() {
        file = &script;
    }

    TextEditWidget(Shader& shader) : TextEditWidget() {
        file = &shader;
    }

    TextEditWidget(File& file) : TextEditWidget() {
        this->file = &file;
    }

    void Draw(const char* title, bool* p_open = NULL)
    {
        if (file == NULL || file->getData() == NULL) {
            loggerError("file NULL, can't open editor, title:%s, file:0x%p", title, file);
            return;
        } else if (! file->isFile()) {
            loggerError("file does not exist, can't open editor, title:%s, filePath:%s", title, file->getFilePath().c_str());
            return;
        }

        ImVec2 windowSize = ImVec2(500, 300);
        ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);
        ImGui::Begin(title, p_open);

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,0));

        windowSize = ImGui::GetWindowSize();
        if (ImGui::Button("Reload")) {
            FileRefreshManager::getInstance().markFileForRefresh(*file);
        }

        ImGui::InputTextMultiline("##source", reinterpret_cast<char*>(file->getData()), file->length(), ImVec2(-1.0f, windowSize.y),
            ImGuiInputTextFlags_AllowTabInput | (readOnly ? ImGuiInputTextFlags_ReadOnly : 0));

        ImGui::PopStyleVar(3);
        ImGui::End();
    }
};

static bool timelineView = true;
static bool demoScreenView = true;
static bool demoScreenGrid = false;
static std::map<std::string, bool> menuScripts;
static std::map<std::string, bool> menuShaders;
static std::map<std::string, bool> menuShaderPrograms;
static std::map<std::string, bool> menuImages;
static std::map<std::string, bool> menuFbos;
static bool openExportVideo = false;
static bool openMidiTest = false;
static ImVec2 menuBarSize = ImVec2(0,0);
static void ShowEditorMenuBar()
{
    static bool openNewSyncVariable = false;
    static bool openDocumentation = false;
    static bool openAbout = false;
    static bool takeScreenshot = false;
    static bool activeCameraView = false;
    static std::string midiTestLog = "";

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Export screenshot", ""))
            {
                takeScreenshot = true;
            }

            if (ImGui::MenuItem("Export video", ""))
            {
                openExportVideo = true;
            }

            if (ImGui::MenuItem("Save Settings", ""))
            {
                if (!Settings::saveSettings()) {
                    Window::showMessageBox(LEVEL_WARNING, "Save failed", "Could not save settings");
                }
            }

            if (ImGui::MenuItem("Exit", ""))
            {
                EnginePlayer::getInstance().getInput().setUserExit(true);
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem("Timeline", "", &timelineView);
            ImGui::MenuItem("Demo screen", "", &demoScreenView);
            ImGui::MenuItem("Demo screen grid", "", &demoScreenGrid);

            if (ImGui::BeginMenu("Camera"))
            {
                ImGui::MenuItem("Active camera", "", &activeCameraView);
                ImGui::EndMenu();
            }

            MemoryManager<Image>& imageMemory = MemoryManager<Image>::getInstance();
            if (ImGui::BeginMenu(imageMemory.getName()))
            {
                for (auto it : imageMemory.getResources()) {
                    Image* image = it.second;
                    const std::string& name = image->getFilePath();
                    if (menuImages.find(name) == menuImages.end()) {
                        menuImages[name] = false;
                    }

                    ImGui::MenuItem(name.c_str(), "", &menuImages[name]);
                }
                
                ImGui::EndMenu();
            }
            
            MemoryManager<Fbo>& fboMemory = MemoryManager<Fbo>::getInstance();
            if (ImGui::BeginMenu(fboMemory.getName()))
            {
                for (auto it : fboMemory.getResources()) {
                    Fbo* fbo = it.second;
                    const std::string& name = fbo->getName();
                    if (menuFbos.find(name) == menuFbos.end()) {
                        menuFbos[name] = false;
                    }

                    ImGui::MenuItem(name.c_str(), "", &menuFbos[name]);
                }
                
                ImGui::EndMenu();
            }

            MemoryManager<Script>& scriptMemory = MemoryManager<Script>::getInstance();
            if (ImGui::BeginMenu(scriptMemory.getName()))
            {
                for (auto it : scriptMemory.getResources()) {
                    Script* script = it.second;
                    const std::string& name = script->getName();
                    if (menuScripts.find(name) == menuScripts.end()) {
                        menuScripts[name] = false;
                    }

                    ImGui::MenuItem(name.c_str(), "", &menuScripts[name]);
                }
                
                ImGui::EndMenu();
            }

            MemoryManager<Shader>& shaderMemory = MemoryManager<Shader>::getInstance();
            if (ImGui::BeginMenu(shaderMemory.getName()))
            {
                for (auto it : shaderMemory.getResources()) {
                    Shader* shader = it.second;
                    const std::string& name = shader->getName();
                    if (menuShaders.find(name) == menuShaders.end()) {
                        menuShaders[name] = false;
                    }

                    ImGui::MenuItem(name.c_str(), "", &menuShaders[name]);
                }
                
                ImGui::EndMenu();
            }

            MemoryManager<ShaderProgram>& shaderProgramMemory = MemoryManager<ShaderProgram>::getInstance();
            if (ImGui::BeginMenu(shaderProgramMemory.getName()))
            {
                for (auto it : shaderProgramMemory.getResources()) {
                    ShaderProgram* shaderProgram = it.second;
                    const std::string& name = shaderProgram->getName();
                    if (menuShaderPrograms.find(name) == menuShaderPrograms.end()) {
                        menuShaderPrograms[name] = false;
                    }

                    ImGui::MenuItem(name.c_str(), "", &menuShaderPrograms[name]);
                }
                
                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Audio & Sync"))
        {
            ImGui::MenuItem("Mute", "", &Settings::audio.mute);

            Timer& timer = EnginePlayer::getInstance().getTimer();
            bool paused = timer.isPause();
            if (ImGui::MenuItem("Pause", "", paused)) {
                timer.pause(! paused);
            }

            SyncRocket& sync = dynamic_cast<SyncRocket&>(EnginePlayer::getInstance().getSync());
            bool connected = sync.isRocketEditor();

            if (ImGui::MenuItem("GNU Rocket connection", "", connected)) {
                if (!connected) {
                    // Attempt to reconnect to GNU Rocket
                    sync.init(& EnginePlayer::getInstance().getTimer());

                    if (!sync.isRocketEditor()) {
                        Window::showMessageBox(LEVEL_WARNING, "Sync init failed", "Failed to initialize sync");
                    }
                } else {
                    // Attempt to disconnect from GNU Rocket
                    sync.exit();
                    // Reload sync tracks from the disk, do not connect to GNU Rocket
                    sync.init(& EnginePlayer::getInstance().getTimer(), false);
                }
            }

            if (ImGui::MenuItem("New sync variable", "")) {
                openNewSyncVariable = true;
            }

            MidiManager& midiManager = EnginePlayer::getInstance().getMidiManager();
            bool midiConnection = ! midiManager.getMidiControllers().empty();
            if (ImGui::MenuItem("Midi", "", midiConnection)) {
                if (!midiConnection) {
                    if (connectMidi() == false) {
                        Window::showMessageBox(LEVEL_WARNING, "Midi connection failed", "Failed to connect to MIDI devices");
                    }
                } else {
                    midiManager.exit();
                }
            }
            if (ImGui::MenuItem("MidiTest", "", &openMidiTest)) {
                midiTestLog = "";
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("Documentation", "")) {
                openDocumentation = true;
            }
            if (ImGui::MenuItem("About", "")) {
                openAbout = true;
            }

            ImGui::EndMenu();
        }
        /*if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X")) {}
            if (ImGui::MenuItem("Copy", "CTRL+C")) {}
            if (ImGui::MenuItem("Paste", "CTRL+V")) {}
            ImGui::EndMenu();
        }*/

        menuBarSize = ImGui::GetWindowSize();

        ImGui::EndMainMenuBar();
    }

    if (openMidiTest) {
        ImGui::OpenPopup("MidiTest");
    }
    if (ImGui::BeginPopupModal("MidiTest", &openMidiTest))
    {
        MidiManager &midiManager = MidiManager::getInstance();
        std::vector<MidiController*> midiControllers = midiManager.getMidiControllers();

        for(MidiController* midiController : midiControllers) {
            std::vector<MidiEvent> events = midiController->pollEvents();
            if (!events.empty()) {
                for(MidiEvent event : events) {
                    midiTestLog = event.serialize() + std::string("\n") + midiTestLog;
                }
            }
        }

        ImGui::InputTextMultiline("MidiTestLog", const_cast<char*>(midiTestLog.c_str()), midiTestLog.length(), ImVec2(640, 300), ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_ReadOnly);

        ImGui::EndPopup();
    }

    if (openNewSyncVariable) {
        ImGui::OpenPopup("NewSyncVariable");
    }

    if (ImGui::BeginPopupModal("NewSyncVariable", &openNewSyncVariable))
    {
        static char buf[64] = "name";
        ImGui::InputText("New Variable", buf, IM_ARRAYSIZE(buf));

        if (ImGui::Button("Save")) {
            //TODO: Better API convention for creating variable
            EnginePlayer::getInstance().getSync().getVariableCurrentValue(buf);

            std::string syncVariableName = std::string(buf);

            // relink shader program so that the sync variables are auto-binded
            MemoryManager<ShaderProgram>& shaderProgramMemory = MemoryManager<ShaderProgram>::getInstance();
            for (auto it : shaderProgramMemory.getResources()) {
                ShaderProgram* shaderProgram = it.second;
                if (shaderProgram->containsUniform(syncVariableName)) {
                    if (!shaderProgram->link()) {
                        loggerWarning("Could not relink ShaderProgram '%s'. syncVariable:'%s'", it.first.c_str(), syncVariableName.c_str());
                    }
                }
            }

            openNewSyncVariable = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    if (activeCameraView) {
        ImVec2 windowSize = ImVec2(500, 300);
        ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);
        ImGui::Begin("Active Camera details", &activeCameraView);

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,0));

        Camera& camera = EnginePlayer::getInstance().getActiveCamera();
        Vector3 position = camera.getPosition();
        Vector3 lookAt = camera.getLookAt();
        Vector3 up = camera.getUp();

        float cameraPosition[3] = {static_cast<float>(position.x), static_cast<float>(position.y), static_cast<float>(position.z)};
        float cameraLookAt[3] = {static_cast<float>(lookAt.x), static_cast<float>(lookAt.y), static_cast<float>(lookAt.z)};
        float cameraUp[3] = {static_cast<float>(up.x), static_cast<float>(up.y), static_cast<float>(up.z)};
        float cameraClipPlane[2] = {static_cast<float>(camera.getClipPlaneNear()), static_cast<float>(camera.getClipPlaneFar())};
        float cameraHorizontalFov = static_cast<float>(camera.getHorizontalFov());
        float cameraAspectRatio = static_cast<float>(camera.getAspectRatio());

        ImGui::Text(camera.getName().c_str());
        ImGui::InputFloat3("Up", cameraUp);
        ImGui::InputFloat3("Position", cameraPosition);
        ImGui::InputFloat3("Look at", cameraLookAt);
        ImGui::InputFloat2("Clip plane", cameraClipPlane);
        ImGui::InputFloat("Horizontal FOV", &cameraHorizontalFov);
        ImGui::InputFloat("Aspect ratio", &cameraAspectRatio);

        camera.setUp(cameraUp[0], cameraUp[1], cameraUp[2]);
        camera.setPosition(cameraPosition[0], cameraPosition[1], cameraPosition[2]);
        camera.setLookAt(cameraLookAt[0], cameraLookAt[1], cameraLookAt[2]);
        camera.setClipPlaneNear(cameraClipPlane[0]);
        camera.setClipPlaneFar(cameraClipPlane[1]);
        camera.setHorizontalFov(cameraHorizontalFov);
        camera.setAspectRatio(cameraAspectRatio);

        ImGui::PopStyleVar(3);
        ImGui::End();        
    }

    if (openDocumentation) {
        static File readmeFile = File("_embedded/README.md");
        if (!readmeFile.isLoaded()) {
            readmeFile.load();
        }

        TextEditWidget(readmeFile).Draw("Documentation", &openDocumentation);
    }

    if (openAbout) {
        ImGui::OpenPopup("About");
    }

    if (ImGui::BeginPopupModal("About", &openAbout))
    {
        char version[128];
        sprintf(version, "Version: %d.%d.%d (%s %s)",
            ENGINE_VERSION_MAJOR, ENGINE_VERSION_MINOR, ENGINE_VERSION_PATCH, ENGINE_LATEST_COMMIT, ENGINE_BUILD_TIMESTAMP);
        
        ImGui::Text(version);

        ImGui::EndPopup();
    }

    if (openExportVideo) {
        ImGui::OpenPopup("ExportVideo");
    } else if (recorder) {
        recorder->setStop();
        if (!recorder->isRecording()) {
            delete recorder;
            recorder = NULL;
        }        
    }

    if (ImGui::BeginPopupModal("ExportVideo", &openExportVideo))
    {
        static float time[2] = { 0.0f, static_cast<float>(Settings::demo.length) };
        static float fps = 60.0f;

        ImGui::InputFloat2("Time", time);
        ImGui::InputFloat("FPS", &fps);
        static bool encode = true;
        ImGui::Checkbox("Encode output", &encode);

        // These settings aim to be YouTube guideline friendly:
        // AAC-LC audio with high bitrate, stereo/5.1 and 48/96kHz
        // MP4 H264 60 fps (/w nearly lossless quality)
        static char encodeCommandBuf[1024] = "ffmpeg -y" \
            // input video
            " -f rawvideo -pixel_format rgb24 -video_size <width>x<height> -framerate <fps> -i -" \
            // input audio
            " -i <audioFile> -c:a aac -b:a 512k -strict -2" \
            // output video
            " -framerate <fps> -vcodec libx264 -crf 18 -shortest <outputFile>";

        ImGui::InputText("Encoder command", encodeCommandBuf, IM_ARRAYSIZE(encodeCommandBuf));

        if (ImGui::Button(!recorder ? "Save" : "Stop")) {
            if (recorder == NULL) {
                recorder = new Recorder();
                if (recorder == NULL) {
                    loggerFatal("Could not allocate memory for Recorder");
                    return;
                }

                recorder->setEncode(encode);
                recorder->setEncodeCommand(std::string(encodeCommandBuf));
                recorder->setFps(fps);
                recorder->setRecordStartTime(time[0]);
                recorder->setRecordEndTime(time[1]);
            } else {
                recorder->setStop();
            }
        }


        Fbo *mainOutputFbo = MemoryManager<Fbo>::getInstance().getResource(std::string("mainOutputFbo"));

        char dimensions[64];
        sprintf(dimensions, "Dimensions: %dx%d", mainOutputFbo->getWidth(), mainOutputFbo->getHeight());
        ImGui::Text(dimensions);

        float captureFpsValue = 0.0f;
        float outputSizeValue = 0.0f;
        float progressValue = 0.0f;
        if (recorder) {
            captureFpsValue = recorder->getCaptureFps();
            outputSizeValue = recorder->getOutputSize();
            progressValue = recorder->getProgress();
        }
        char captureFps[64];
        sprintf(captureFps, "Capture FPS: %.2f", captureFpsValue);
        ImGui::Text(captureFps);

        char outputSize[128];
        sprintf(outputSize, "RAW output size: %.2f MB", outputSizeValue);
        ImGui::Text(outputSize);

        ImGui::ProgressBar(progressValue, ImVec2(-1.0f,0.0f));

        ImGui::EndPopup();
    }

    if (takeScreenshot) {
        takeScreenshot = false;
        Fbo *mainOutputFbo = MemoryManager<Fbo>::getInstance().getResource(std::string("mainOutputFbo"));
        if (mainOutputFbo) {
            char fileName[256];
            sprintf(fileName, "screenshot_frame_%05lu.png", static_cast<long unsigned int>(EnginePlayer::getInstance().getFps().getTotalFrameCount()));
            int channels = 3; //RGB
            unsigned char *pixels = new unsigned char[mainOutputFbo->getWidth() * mainOutputFbo->getHeight() * channels];
            if (pixels == NULL) {
                loggerFatal("Could not allocate memory for screenshot");
                return;
            }

            mainOutputFbo->bind();
            glReadPixels(0, 0, mainOutputFbo->getWidth(), mainOutputFbo->getHeight(), GL_RGB, GL_UNSIGNED_BYTE, static_cast<void*>(pixels));
            mainOutputFbo->unbind();

            Image *screenshot = Image::newInstance(fileName);
            Image::write(*screenshot, mainOutputFbo->getWidth(), mainOutputFbo->getHeight(), channels, pixels);

            delete [] pixels;
            delete screenshot;
        } else {
            loggerWarning("Taking screenshot failed, fbo missing");
        }
    }
}

struct TextureScreenWidget
{
    unsigned int id;
    int width;
    int height;
    bool showGrid;
    int spacingX;
    int spacingY;

    ImVec2 windowPosition;
    ImVec2 windowSize;

    void setWindowSize(double width, double height) {
        windowSize.x = width;
        windowSize.y = height;
    }

    void setWindowPosition(double x, double y) {
        windowPosition.x = x;
        windowPosition.y = y;
    }

    TextureScreenWidget() {
        id = 0;
        width = 0;
        height = 0;
        showGrid = false;
        spacingX = 50;
        spacingY = 50;
        windowPosition = ImVec2(0, 0);
        Window& window = *EnginePlayer::getInstance().getWindow(WindowType::EDITOR); 
        windowSize = ImVec2(window.getWidth() / 2.0, window.getHeight() / 2.0);
    }

    TextureScreenWidget(Fbo& fbo) : TextureScreenWidget() {
        id = dynamic_cast<TextureOpenGl*>(fbo.getColorTexture())->getId();
        width = fbo.getWidth();
        height = fbo.getHeight();
    }

    TextureScreenWidget(Image& image) : TextureScreenWidget() {
        id = dynamic_cast<TextureOpenGl*>(image.getTexture())->getId();
        width = image.getWidth();
        height = image.getHeight();
    }

    void Draw(const char* title, bool* p_open = NULL)
    {
        ImGui::SetNextWindowPos(windowPosition, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);
        ImGui::Begin(title, p_open);

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,0));

        windowSize = ImGui::GetWindowSize();
        double textureWidth = windowSize.x;
        double textureHeight = windowSize.x / (width / (double)height);
        if (textureHeight > windowSize.y) {
            double wantedHeight = windowSize.y - 30;
            textureWidth = wantedHeight / (height / (double)width);
            textureHeight = wantedHeight;
        }

        ImVec2 texturePos = ImGui::GetCursorPos();
        ImGui::Image((void*)id, ImVec2(textureWidth, textureHeight), ImVec2(0,1), ImVec2(1,0));
        
        if (showGrid) {
            ImGui::SetCursorPos(texturePos);

            int scrollingX = width;
            int scrollingY = height;

            ImDrawList* drawList = ImGui::GetWindowDrawList();

            for(int col = 0; col < scrollingX/spacingX; col++) {
                drawList->AddLine(
                    ImVec2(ImGui::GetWindowPos().x+col*spacingX,ImGui::GetWindowPos().y),
                    ImVec2(ImGui::GetWindowPos().x+col*spacingX,ImGui::GetWindowPos().y+scrollingY),
                    ImColor(1.0f, 1.0f, 1.0f, 0.8f), 1.0f);
            }
            for(int row = 0; row < scrollingY/spacingY; row++) {
                drawList->AddLine(
                    ImVec2(ImGui::GetWindowPos().x,ImGui::GetWindowPos().y+row*spacingY),
                    ImVec2(ImGui::GetWindowPos().x+scrollingX,ImGui::GetWindowPos().y+row*spacingY),
                    ImColor(1.0f, 1.0f, 1.0f, 0.8f), 1.0f);
            }
        }

        ImGui::PopStyleVar(3);
        ImGui::End();
    }
};

class ProgressBar {
public:
    ProgressBar();
    ~ProgressBar();
    bool init();
    bool deinit();
    void draw(double percent);
private:
    TexturedQuad *texturedQuad;
    ShaderProgram *progressBar;
};

ProgressBar::ProgressBar() {
    texturedQuad = NULL;
    progressBar = NULL;
}

ProgressBar::~ProgressBar() {
    deinit();
}

bool ProgressBar::init() {

    MemoryManager<Shader>& shaderMemory = MemoryManager<Shader>::getInstance();
    Shader *progressBarFs = shaderMemory.getResource(std::string("_embedded/progressBar.fs"), true);
    if (! progressBarFs->load()) {
        loggerFatal("Could not compile progress bar");
        return false;
    }

    MemoryManager<ShaderProgram>& shaderProgramMemory = MemoryManager<ShaderProgram>::getInstance();
    progressBar = shaderProgramMemory.getResource(std::string("DefaultProgressBar"), true);
    progressBar->addShader(progressBarFs);
    if (! progressBar->link()) {
        loggerFatal("Could not initialize progress bar");
        return false;
    }

    texturedQuad = TexturedQuad::newInstance(Settings::demo.graphics.canvasWidth, Settings::demo.graphics.canvasHeight);
    if (!texturedQuad->init()) {
        return false;
    }

    return true;
}

bool ProgressBar::deinit() {
    if (texturedQuad != NULL) {
        texturedQuad->deinit();
        delete texturedQuad;
    }

    return true;
}

void ProgressBar::draw(double percent) {
    if (texturedQuad == NULL || progressBar == NULL) {
        loggerWarning("ProgressBar not properly initialized. Cannot draw.");
        return;
    }

    progressBar->bind();

    GLint percentId = ShaderProgramOpenGl::getUniformLocation("percent");
    if (percentId != -1) {
        glUniform1f(percentId, static_cast<float>(percent));
    }

    texturedQuad->draw();

    progressBar->unbind();
}


void EnginePlayer::toolGuiRender() {
    PROFILER_BLOCK("EnginePlayer::toolGuiRender");
    setLoggerPrintState("TOOL_RENDER");

    bool show_test_window = true;

    glEnable(GL_SCISSOR_TEST);

    Window& window = *getWindow(WindowType::EDITOR);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(dynamic_cast<WindowSdl&>(window).window);
    ImGui::NewFrame();

    ShowEditorMenuBar();

    static float demoScreenHeight = 0.6;
    if (demoScreenView) {
        static TextureScreenWidget demoScreen = TextureScreenWidget(* MemoryManager<Fbo>::getInstance().getResource(std::string("mainOutputFbo"), true));
        demoScreen.showGrid = demoScreenGrid;
        demoScreen.setWindowPosition(0, menuBarSize.y);
        demoScreen.setWindowSize(window.getWidth(), window.getHeight() * demoScreenHeight - menuBarSize.y);
        demoScreen.Draw("Demo screen", &demoScreenView);
    }

    if (timelineView) {
        static TimelineWidget timeline;
        timeline.setWindowPosition(0, menuBarSize.y + window.getHeight() * demoScreenHeight);
        timeline.setWindowSize(window.getWidth(), window.getHeight() * (1.0f - demoScreenHeight) - menuBarSize.y);        
        timeline.Draw("Timeline", &timelineView);
    }

    for (auto it : menuImages) {
        if (it.second) {
            Image *image = MemoryManager<Image>::getInstance().getResource(it.first);
            TextureScreenWidget(*image).Draw(it.first.c_str(), &menuImages[it.first]);
        }
    }

    for (auto it : menuFbos) {
        if (it.second) {
            Fbo *fbo = MemoryManager<Fbo>::getInstance().getResource(it.first);
            TextureScreenWidget(*fbo).Draw(it.first.c_str(), &menuFbos[it.first]);
        }
    }

    for (auto it : menuScripts) {
        if (it.second) {
            Script *script = MemoryManager<Script>::getInstance().getResource(it.first);
            TextEditWidget(*script).Draw(it.first.c_str(), &menuScripts[it.first]);
        }
    }

    for (auto it : menuShaders) {
        if (it.second) {
            Shader *shader = MemoryManager<Shader>::getInstance().getResource(it.first);
            TextEditWidget(*shader).Draw(it.first.c_str(), &menuShaders[it.first]);
        }
    }

    for (auto it : menuShaderPrograms) {
        if (it.second) {
            ImVec2 windowSize = ImVec2(500, 300);
            ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);
            ImGui::Begin(it.first.c_str(), &menuShaderPrograms[it.first]);

            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 1.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,0));

            ShaderProgramOpenGl *shaderProgram = dynamic_cast<ShaderProgramOpenGl*>(MemoryManager<ShaderProgram>::getInstance().getResource(it.first));

            const GLsizei bufSize = 256;
            GLchar *tmpname = new GLchar [bufSize];

            GLint uniformCount = 0;
            glGetProgramiv(shaderProgram->getId(), GL_ACTIVE_UNIFORMS, &uniformCount);
            for (GLuint i = 0; i < static_cast<GLuint>(uniformCount); i++) {
                GLsizei length = 0;
                GLint size = 0;
                GLenum glType = 0;

                glGetActiveUniform(shaderProgram->getId(), i, bufSize, &length, &size, &glType, tmpname);
                std::string name = std::string(tmpname);

                const char *typeName = NULL;
                switch(glType) {
                    case GL_FLOAT:
                        typeName = "float";
                        break;
                    case GL_DOUBLE:
                        typeName = "double";
                        break;
                    case GL_INT:
                        typeName = "int";
                        break;
                    case GL_FLOAT_VEC2:
                    case GL_DOUBLE_VEC2:
                        typeName = "vec2";
                        break;
                    case GL_FLOAT_VEC3:
                    case GL_DOUBLE_VEC3:
                        typeName = "vec3";
                        break;
                    case GL_FLOAT_VEC4:
                    case GL_DOUBLE_VEC4:
                        typeName = "vec4";
                        break;
                    case GL_FLOAT_MAT3:
                    case GL_DOUBLE_MAT3:
                        typeName = "mat3";
                        break;
                    case GL_FLOAT_MAT4:
                    case GL_DOUBLE_MAT4:
                        typeName = "mat4";
                        break;
                    case GL_SAMPLER_1D:
                        typeName = "sampler1D";
                        break;
                    case GL_SAMPLER_2D:
                        typeName = "sampler2D";
                        break;
                    case GL_SAMPLER_3D:
                        typeName = "sampler3D";
                        break;
                    default:
                        typeName = NULL;
                        break;
                }

                if (typeName == NULL) {
                    continue;
                }

                GLfloat params[32];
                char buf[1024];
                sprintf(buf, "%s %s", typeName, name.c_str());

                const char* decimalPrecision = "%.3f";
                const ImGuiInputTextFlags flags = ImGuiInputTextFlags_ReadOnly;
                float step = 0.0f;
                float step_fast = 0.0f;

                if (glType == GL_INT || glType == GL_SAMPLER_1D || glType == GL_SAMPLER_2D || glType == GL_SAMPLER_3D) {
                    glGetUniformfv(shaderProgram->getId(), i, params);
                    ImGui::InputFloat(buf, &params[0], step, step_fast, "%.0f", flags);
                } else if (glType == GL_FLOAT || glType == GL_DOUBLE){
                    glGetUniformfv(shaderProgram->getId(), i, params);
                    ImGui::InputFloat(buf, &params[0], step, step_fast, decimalPrecision, flags);
                } else if (glType == GL_FLOAT_VEC2 || glType == GL_DOUBLE_VEC2){
                    glGetUniformfv(shaderProgram->getId(), i, params);
                    ImGui::InputFloat2(buf, &params[0], decimalPrecision, flags);
                } else if (glType == GL_FLOAT_VEC3 || glType == GL_DOUBLE_VEC3){
                    glGetUniformfv(shaderProgram->getId(), i, params);
                    ImGui::InputFloat3(buf, &params[0], decimalPrecision, flags);
                } else if (glType == GL_FLOAT_VEC4 || glType == GL_DOUBLE_VEC4){
                    glGetUniformfv(shaderProgram->getId(), i, params);
                    ImGui::InputFloat4(buf, &params[0], decimalPrecision, flags);
                } else {
                    static char emptyData[] = {'\0'};
                    ImGui::InputText(buf, emptyData, 1, flags);
                }


            }

            delete [] tmpname;

            /*windowSize = ImGui::GetWindowSize();
            if (ImGui::Button("Reload")) {
                FileRefreshManager::getInstance().markFileForRefresh(*file);
            }

            ImGui::InputTextMultiline("##source", reinterpret_cast<char*>(file->getData()), file->length(), ImVec2(-1.0f, windowSize.y),
                ImGuiInputTextFlags_AllowTabInput | (readOnly ? ImGuiInputTextFlags_ReadOnly : 0));*/

            ImGui::PopStyleVar(3);
            ImGui::End();
        }
    }

    ImGui::Render();

    glDisable(GL_SCISSOR_TEST);
}

EnginePlayer& EnginePlayer::getInstance() {
    static EnginePlayer enginePlayer = EnginePlayer();
    return enginePlayer;
}

EnginePlayer::EnginePlayer() {
    gui = &Gui::getInstance();
    audio = &Audio::getInstance();
    input = &Input::getInstance();
    sync = &Sync::getInstance();
    graphics = &Graphics::getInstance();
    fileRefreshManager = &FileRefreshManager::getInstance();
    networkManager = &NetworkManager::getInstance();
    midiManager = &MidiManager::getInstance();

    editorWindow = NULL;
    playerWindow = NULL;
    menu = NULL;

    defaultCamera = new Camera();
    setActiveCamera(*defaultCamera);

    shadow = new Shadow();
    shadow->setTextureUnit(10);

    redraw = false;
    forceReload();
}

EnginePlayer::~EnginePlayer() {
    delete defaultCamera;
}

Shadow& EnginePlayer::getShadow() {
    return *shadow;
}

Camera& EnginePlayer::getActiveCamera() {
    return *activeCamera;
}

void EnginePlayer::setActiveCamera(Camera& camera) {
    activeCamera = &camera;
}

void EnginePlayer::forceRedraw() {
    redraw = true;
}

void EnginePlayer::forceReload() {
    reload = true;
    setProgress(0.0);
}

void EnginePlayer::setProgress(double progress) {
    this->progress = progress;
}

void EnginePlayer::setDrawFunction(const std::function<void()> &drawFunction) {
    this->drawFunction = drawFunction;
}

Audio& EnginePlayer::getAudio() {
    return *audio;
}

Window* EnginePlayer::getWindow(WindowType type) {
    switch(type) {
        case WindowType::MENU:
            return dynamic_cast<Window*>(menu);
        case WindowType::EDITOR:
            return editorWindow;
        case WindowType::PLAYER:
            return playerWindow;
        case WindowType::CURRENT:
            if (editorWindow != NULL && editorWindow->getFocus()) {
                return editorWindow;
            } else if (playerWindow != NULL) {
                // Fallback to player window always whem available
                return playerWindow;
            } else {
                return dynamic_cast<Window*>(menu);
            }
        default:
            loggerFatal("Incorrect window type given! %d", type);
            return NULL;
    }
}

Input& EnginePlayer::getInput() {
    return *input;
}

Sync& EnginePlayer::getSync() {
    return *sync;
}

Graphics& EnginePlayer::getGraphics() {
    return *graphics;
}

FileRefreshManager& EnginePlayer::getFileRefreshManager() {
    return *fileRefreshManager;
}

NetworkManager& EnginePlayer::getNetworkManager() {
    return *networkManager;
}

MidiManager& EnginePlayer::getMidiManager() {
    return *midiManager;
}

Timer& EnginePlayer::getTimer() {
    return timer;
}

Fps& EnginePlayer::getFps() {
    return fps;
}

bool EnginePlayer::init() {
    PROFILER_BLOCK("EnginePlayer::init");
    setLoggerPrintState("PRELOAD");

    EmbeddedResourceManager::getInstance().createEmbeddedResources();

    if (!gui->init()) {
        loggerFatal("Failed to initialize the GUI");

        return false;
    }


    MemoryManager<Script>& scriptMemory = MemoryManager<Script>::getInstance();
    Script *script = scriptMemory.getResource(std::string("_embedded/polyfill.js"), true);
    script->load();

    script = scriptMemory.getResource(std::string("_embedded/Core.js"), true);
    script->load();

    script = scriptMemory.getResource(std::string("_embedded/Settings.js"), true);
    script->load();

    Settings::demo.loadDemoSettings();
    ScriptEngine::getInstance().synchronizeSettings();

    script = scriptMemory.getResource(std::string("_embedded/Utils.js"), true);
    script->load();
    script = scriptMemory.getResource(std::string("_embedded/Graphics.js"), true);
    script->load();
    script = scriptMemory.getResource(std::string("_embedded/Image.js"), true);
    script->load();
    script = scriptMemory.getResource(std::string("_embedded/Video.js"), true);
    script->load();
    script = scriptMemory.getResource(std::string("_embedded/Text.js"), true);
    script->load();
    script = scriptMemory.getResource(std::string("_embedded/Fbo.js"), true);
    script->load();
    script = scriptMemory.getResource(std::string("_embedded/Model.js"), true);
    script->load();
    script = scriptMemory.getResource(std::string("_embedded/Mesh.js"), true);
    script->load();
    script = scriptMemory.getResource(std::string("_embedded/CatmullRomSpline.js"), true);
    script->load();
    script = scriptMemory.getResource(std::string("_embedded/Light.js"), true);
    script->load();
    script = scriptMemory.getResource(std::string("_embedded/Camera.js"), true);
    script->load();
    script = scriptMemory.getResource(std::string("_embedded/Effect.js"), true);
    script->load();
    script = scriptMemory.getResource(std::string("_embedded/Shader.js"), true);
    script->load();
    script = scriptMemory.getResource(std::string("_embedded/Sync.js"), true);
    script->load();
    script = scriptMemory.getResource(std::string("_embedded/Scene.js"), true);
    script->load();
    script = scriptMemory.getResource(std::string("_embedded/Audio.js"), true);
    script->load();
    script = scriptMemory.getResource(std::string("_embedded/Loader.js"), true);
    script->load();
    script = scriptMemory.getResource(std::string("_embedded/Player.js"), true);
    script->load();
    script = scriptMemory.getResource(std::string("_embedded/Input.js"), true);
    script->load();
    script = scriptMemory.getResource(std::string("_embedded/Socket.js"), true);
    script->load();
    script = scriptMemory.getResource(std::string("_embedded/Menu.js"), true);
    script->load();

    if (!Settings::demo.song.empty() && !audio->init()) {
        gui->exit();

        loggerFatal("Failed to initialize audio");

        return false;
    }

    if (Settings::showMenu) {
        menu = &Menu::getInstance();
        menu->init();
        menu->open();
        menu->exit();

        if (menu->isQuit()) {
            gui->exit();

            loggerInfo("User requested exit in the menu");
            return false;
        }

        input->setUserExit(false);
    }

    playerWindow = Window::newInstance();
    playerWindow->init();
    playerWindow->setDimensions(Settings::window.width, Settings::window.height);
    playerWindow->setTitle(Settings::demo.title);
    playerWindow->setFullscreen(Settings::window.fullscreen);
    if (!playerWindow->open()) {
        loggerFatal("Failed opening the window");
        return false;
    }

    loggerDebug("Screen dimensions: size:%.0fx%.0f, aspectRatio:%.2f",
        Settings::demo.graphics.canvasWidth, Settings::demo.graphics.canvasHeight, Settings::demo.graphics.aspectRatio);

    loggerDebug("Window dimensions: size:%ux%u, canvasPosition:%ux%u, canvasSize:%ux%u",
        Settings::window.width, Settings::window.height,
        Settings::window.canvasPositionX, Settings::window.canvasPositionY,
        Settings::window.screenAreaWidth, Settings::window.screenAreaHeight);

    sync->init(&getTimer());

    loggerTrace("GUI context init");

    // Setup ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = NULL;


    fileRefreshManager->start();
    networkManager->start();

    MemoryManager<Image>& imageMemory = MemoryManager<Image>::getInstance();
    Image *imageDefaultWhite = imageMemory.getResource(std::string("_embedded/defaultWhite.png"), true);
    imageDefaultWhite->load();
    Texture::setDefault(imageDefaultWhite->getTexture());

    MemoryManager<Shader>& shaderMemory = MemoryManager<Shader>::getInstance();

    Shader *defaultVs = shaderMemory.getResource(std::string("_embedded/default.vs"), true);
    defaultVs->load();
    Shader *defaultFs = shaderMemory.getResource(std::string("_embedded/default.fs"), true);
    defaultFs->load();

    MemoryManager<ShaderProgram>& shaderProgramMemory = MemoryManager<ShaderProgram>::getInstance();

    ShaderProgram *defaultShaderProgram = shaderProgramMemory.getResource(std::string("Default"), true);
    defaultShaderProgram->addShader(defaultVs);
    defaultShaderProgram->addShader(defaultFs);
    if (!defaultShaderProgram->link()) {
        loggerFatal("Failed loading Default shader");
        return false;
    }
    ShaderProgram::useCurrentBind(); // This attempts to utilize the currently bind shader (=default)

    Shader *defaultPlainVs = shaderMemory.getResource(std::string("_embedded/defaultPlain.vs"), true);
    defaultPlainVs->load();
    Shader *defaultPlainFs = shaderMemory.getResource(std::string("_embedded/defaultPlain.fs"), true);
    defaultPlainFs->load();
    ShaderProgram *defaultPlainShaderProgram = shaderProgramMemory.getResource(std::string("DefaultPlain"), true);
    defaultPlainShaderProgram->addShader(defaultPlainVs);
    defaultPlainShaderProgram->addShader(defaultPlainFs);
    if (!defaultPlainShaderProgram->link()) {
        loggerFatal("Failed loading DefaultPlain shader");
        return false;
    }

    MemoryManager<Fbo>& fboMemory = MemoryManager<Fbo>::getInstance();
    mainOutputFbo = fboMemory.getResource(std::string("mainOutputFbo"), true);
    if (!mainOutputFbo->generate()) {
        loggerFatal("Failed initializing mainOutputFbo");
        return false;
    }

    if (!shadow->init()) {
        loggerFatal("Failed initializing shadows");
        return false;
    }

    mainOutputFboQuad = TexturedQuad::newInstance(mainOutputFbo);
    if (!mainOutputFboQuad->init()) {
        loggerFatal("Failed initializing mainOutputFboQuad");
        return false;
    }

    progressBar = new ProgressBar();
    if (progressBar == NULL) {
        loggerFatal("Could not initialize progress bar (memory issues?)");
        return false;
    }
    progressBar->init();

    if (graphics->handleErrors()) {
        loggerWarning("Graphics handling error occurred in init phase");
    }


    return true;
}

bool EnginePlayer::load() {
    PROFILER_BLOCK("EnginePlayer::load");
    uint64_t loadStart = SystemTime::getTimeInMillis();

    setLoggerPrintState("LOAD");

    setDrawFunction([&,this]() {
        this->progressBar->draw(this->progress);
    });

    fftTextureInit();

    Script *script = MemoryManager<Script>::getInstance().getResource(std::string("Demo.js"), true);
    script->setInitClassCall("var Demo = function() {}");
    script->setInitCall("Effect.init(\"Demo\")");
    script->setExitCall("Effect.deinit(\"Demo\")");
    script->setExitClassCall("Demo = null");
    loggerDebug("Loading script '%s'", script->getFilePath().c_str());
    script->load();

    if (graphics->handleErrors()) {
        loggerWarning("Graphics handling error occurred in loading phase");
    }

    setDrawFunction([&,this,script]() {
        EASY_BLOCK("Demo render", profiler::colors::Default);

        if (script->getError()) {
            loggerTrace("Script in error, will not draw. file:'%s'", script->getFilePath().c_str());
            return;
        }

        if (!script->evalString("Effect.run(\"Demo\")")) {
            Fbo::reset();
            script->load(true);
            this->forceRedraw();
        }
    });

    loggerDebug("Loading time %u ms", SystemTime::getTimeInMillis() - loadStart);

    return !input->isUserExit();
}

static unsigned int elapsedSeconds = 0;
static uint64_t previousTime = 0;
static TimeFormatter tf = TimeFormatter("mm:ss");
static std::string endTime = "";
void EnginePlayer::run() {
    setLoggerPrintState("RUN");

    endTime = tf.format(Date(Settings::demo.length * 1000.0));


    while (!input->isUserExit()) {
        processFrame();

        if (recorder) {
            if (openExportVideo) {
                Fbo *mainOutputFbo = MemoryManager<Fbo>::getInstance().getResource(std::string("mainOutputFbo"), true);
                if (!recorder->capture(*mainOutputFbo)) {
                    loggerWarning("Capturing failed");
                }

                delete recorder;
                recorder = NULL;
            }
        }

        if (timer.getTimeInSeconds() >= Settings::demo.length && Settings::demo.length > 0.0) {
            SyncRocket& sync = dynamic_cast<SyncRocket&>(EnginePlayer::getInstance().getSync());
            bool connected = sync.isRocketEditor();
            if (connected) {
                timer.pause(true);
                continue;
            }

            loggerDebug("Demo finished.");
            break;
        }
    }
}

void EnginePlayer::updateWindowTitle() {
    std::stringstream extraInfo;
    extraInfo << " (v" << ENGINE_VERSION << ", " << ENGINE_LATEST_COMMIT << ") ";
    extraInfo << " - " << tf.format(Date(timer.getTimeInMilliseconds())) << "/" << endTime;
    extraInfo << ", " << static_cast<int>(fps.getFps() + 0.5) << " FPS";
    if (timer.isPause()) {
        extraInfo << " (PAUSED)";
    }

    if (playerWindow != NULL) {
        playerWindow->setTitle(playerWindow->getTitle(), extraInfo.str());    
    }
    
    if (editorWindow != NULL) {
        editorWindow->setTitle(editorWindow->getTitle(), extraInfo.str());    
    }
}

void EnginePlayer::processFrame() {
    PROFILER_BLOCK("frame");

    if (reload) {
        ScriptEngine& scriptEngine = ScriptEngine::getInstance();

        Settings::demo.loadDemoSettings();
        ScriptEngine::getInstance().synchronizeSettings();

        if (load()) {
            reload = false;

            if (!Settings::demo.song.empty() && audio->load(Settings::demo.song.c_str())) {
                //Audio needs to be loaded to determine the demo length, audio playing is optional
                if (audio->play(Settings::demo.song.c_str())) {
                    loggerTrace("Playing '%s'", Settings::demo.song.c_str());
                    timer.synchronizeToAudio(audio);
                }
            }

            setLoggerTimer(&timer);

            timer.start();

            if (Settings::gui.startPosition > 0.0) {
                loggerInfo("Rewinding demo to position: %.2f", Settings::gui.startPosition);
            }
            timer.setTimeInSeconds(Settings::gui.startPosition);
        } else {
            return;
        }

        endTime = tf.format(Date(Settings::demo.length * 1000.0)); // recalc time
    }

    previousTime = timer.getTimeInMilliseconds();
    timer.update();

    sync->update();

    input->pollEvents();

    if (fileRefreshManager->isModified()) {
        setLoggerPrintState("RELOAD");

        glFinish();

        setDrawFunction([&,this]() {
            this->progressBar->draw(this->progress);
        });

        fileRefreshManager->reloadModified();
        glFinish();

        Script *script = MemoryManager<Script>::getInstance().getResource(std::string("Demo.js"), true);
        setDrawFunction([&,this,script]() {
            EASY_BLOCK("Demo render", profiler::colors::Default);

            if (script->getError()) {
                loggerTrace("Script in error, will not draw. file:'%s'", script->getFilePath().c_str());
                return;
            }

            if (!script->evalString("Effect.run(\"Demo\")")) {
                Fbo::reset();
                script->load(true);
                this->forceRedraw();
            }
        });

        if (graphics->handleErrors()) {
            loggerWarning("Graphics error occurred after reload!");
        }

        setLoggerPrintState("RUN");
    }

    if (timer.isPause()) {
        if (previousTime != timer.getTimeInMilliseconds()) {
            forceRedraw();
        }

        if (Settings::gui.editor) {
            SystemTime::sleepInMillis(10);
        } else {
            //No rewinding done => generous sleep, when no editor/input handling
            SystemTime::sleepInMillis(100);
        }

        updateWindowTitle();

        if (!redraw) {
            SystemTime::sleepInMillis(10);
            if (!Settings::gui.editor) {
                return;
            }
        }
    }

    mainScreenDraw();

    fps.update();
    SystemTime::sleepInMillis(static_cast<uint64_t>(fps.getTargetFpsSleepInMillis()));

    unsigned int newElapsedSeconds = static_cast<unsigned int>(timer.getTimeInSeconds());
    if (newElapsedSeconds != elapsedSeconds) {
        elapsedSeconds = newElapsedSeconds;

        updateWindowTitle();
    }
}

void EnginePlayer::mainScreenDraw() {
    if (!timer.isPause() || redraw) {
        if (redraw) {
            redraw = false;
            loggerTrace("Redraw forced");
        }

        playerWindow->bindGraphicsContext();

        graphics->setClearColor(Settings::demo.graphics.clearColor);
        graphics->setViewport();
        graphics->clear();


        // TODO: A bit of a logic snafu: lights might not be defined in first render pass... need to get state of lights first
        bool shadows = false;
        LightManager& lightManager = LightManager::getInstance();
        if (lightManager.getLighting()) {
            for(unsigned int light_i = 0; light_i < lightManager.getActiveLightCount(); light_i++) {
                Light& light = lightManager.getLight(light_i);
                if (light.getGenerateShadowMap()) {
                    shadows = true;
                    shadow->setCameraFromLight(light);
                    setActiveCamera(shadow->getCamera());

                    shadow->captureStart();
                    setLoggerPrintState("SHADOW RENDER");
                    // TODO: draw function should be limited only to stuff that generates shadows
                    drawFunction();
                    if (graphics->handleErrors()) {
                        loggerWarning("Graphics error occurred in shadow render pass");
                    }
                    shadow->captureEnd();

                    setActiveCamera(*defaultCamera);
                }
            }
        }

        fftTextureUpdate();

        mainOutputFbo->start();
        setLoggerPrintState("RENDER");
        if (shadows) {
            shadow->textureBind();
        }

        drawFunction();

        if (shadows) {
            shadow->textureUnbind();
        }

        if (graphics->handleErrors()) {
            loggerWarning("Graphics error occurred in render pass");
        }
        setLoggerPrintState("RUN");
        mainOutputFbo->end();
    if (graphics->handleErrors()) {
        loggerWarning("Graphics error occurred in main screen draw1");
    }

        mainOutputFboQuad->draw();
    if (graphics->handleErrors()) {
        loggerWarning("Graphics error occurred in main screen draw2");
    }
    }

    if (graphics->handleErrors()) {
        loggerWarning("Graphics error occurred in main screen draw");
    }

    playerWindow->swapBuffers();

    if (Settings::gui.editor == true) {
        if (editorWindow == NULL) {
            editorWindow = Window::newInstance();
            editorWindow->init();
            editorWindow->setDimensions(Settings::window.width, Settings::window.height);
            editorWindow->setTitle(Settings::demo.title);
            editorWindow->setFullscreen(Settings::window.fullscreen);
            if (!editorWindow->open()) {
                loggerFatal("Failed opening the editor window");
                return;
            }
            editorWindow->show(Settings::gui.editor);

#if __APPLE__
            // GL 3.2 Core + GLSL 150
            const char* glslVersion = "#version 150";
#else
            // GL 3.0 + GLSL 130
            const char* glslVersion = "#version 130";
#endif

            // Setup ImGui binding
            ImGui_ImplSDL2_InitForOpenGL(dynamic_cast<WindowSdl*>(editorWindow)->window, static_cast<SDL_GLContext>(editorWindow->getGraphicsContext()));
            ImGui_ImplOpenGL3_Init(glslVersion);
        }


        setLoggerPrintState("EDITOR");
        editorWindow->bindGraphicsContext();
        graphics->setClearColor(Settings::demo.graphics.clearColor);
        graphics->setViewport();
        graphics->clear();

        toolGuiRender();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        editorWindow->swapBuffers();
        playerWindow->bindGraphicsContext();
        setLoggerPrintState("RUN");
    }

}

bool EnginePlayer::exit() {
    PROFILER_BLOCK("EnginePlayer::exit");

    fftTextureDeinit();
    audio->exit();

    delete progressBar;

    midiManager->exit();

    fileRefreshManager->exit();

    networkManager->exit();

    MemoryManager<Script>::getInstance().clear();

    ScriptEngine::getInstance().exit();

    MemoryManager<Fbo>::getInstance().clear();

    MemoryManager<Image>::getInstance().clear();

    MemoryManager<VideoFile>::getInstance().clear();

    MemoryManager<Model>::getInstance().clear();

    MemoryManager<Font>::getInstance().clear();

    if (shadow) {
        delete shadow;
    }

    MemoryManager<ShaderProgram>::getInstance().clear();

    MemoryManager<Shader>::getInstance().clear();

    MemoryManager<AudioFile>::getInstance().clear();


    graphics->exit();

    if (playerWindow != NULL) {
        playerWindow->close();
        playerWindow->exit();
        delete playerWindow;
    }

    if (editorWindow != NULL) {
        loggerTrace("GUI context destroy");
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        editorWindow->close();
        editorWindow->exit();
        delete editorWindow;
    }

    gui->exit();
    sync->exit();

    setLoggerTimer(NULL);

    return true;
}
