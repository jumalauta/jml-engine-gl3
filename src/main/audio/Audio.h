#ifndef ENGINE_AUDIO_AUDIO_H_
#define ENGINE_AUDIO_AUDIO_H_

#include <vector>
#include <string>

class AudioFile;

class Audio {
public:
    static Audio& getInstance();
    virtual ~Audio() {}
    virtual bool init() = 0;
    virtual bool exit() = 0;
    virtual void pause(bool audioPause) = 0;
    virtual void setDuration(const char *filename, double duration) = 0;
    virtual bool load(const char *filename) = 0;
    virtual bool play(const char *filename) = 0;
    virtual void stop() = 0;
    virtual void setPosition(double position) = 0;
    virtual double getTimeInSeconds() = 0;
    virtual double getSampleRate() = 0;
    virtual bool isPaused() = 0;
    virtual AudioFile* getAudioFile() = 0;
    virtual std::vector<std::string> getOutputDevices() = 0;
protected:
    Audio() {};
};

#endif /*ENGINE_AUDIO_AUDIO_H_*/
