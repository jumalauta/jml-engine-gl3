#ifndef ENGINE_AUDIO_AUDIO_SDL_H_
#define ENGINE_AUDIO_AUDIO_SDL_H_

#include "Audio.h"

#ifdef __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

class AudioStream {
public:
    AudioStream();
    ~AudioStream();
    AudioFile* audioFile;
    //TODO: forcing to signed 16 bit is nasty, refactor please
    Uint32 audioBufferSize;
    Uint32 audioBufferDecodedSize;
    Sint16 *audioBuffer;
    unsigned int samplePosition;
    bool loop;
    uint64_t audioCallbackTime;
    double duration;

    void incrementSamplePosition(unsigned int length);
    void resetSamplePosition();
    unsigned int getSamplePosition();
    unsigned int getAudioBufferDecodedSize();
    const void * getAudioBufferCurrentPosition();
    double getTimeInSeconds();

    bool isEnd();
    void setDuration(double duration);
};

class AudioSdl : public Audio {
public:
    AudioSdl();
    virtual ~AudioSdl();
    bool init();
    bool exit();
    void pause(bool audioPause);
    void stop();
    void setDuration(const char *filename, double duration);
    bool load(const char *filename);
    bool play(const char *filename);
    void setPosition(double position);
    double getTimeInSeconds();
    //void incrementSamplePosition(unsigned int length);
    //void resetSamplePosition();
    //unsigned int getSamplePosition();
    //unsigned int getAudioBufferDecodedSize();
    //const void * getAudioBufferCurrentPosition();
    bool isPaused();
    AudioFile* getAudioFile();
    std::vector<std::string> getOutputDevices();


    double getSampleRate();
    SDL_AudioDeviceID deviceIn;
    SDL_AudioDeviceID deviceOut;
    SDL_AudioSpec requestAudioOutSpec;
    SDL_AudioSpec requestAudioInSpec;
    std::vector<AudioStream> audioStreams; // FIXME PERKELE
private:
    static void audioCallback(void *userData, Uint8 *outputStream, int outputStreamLength);
    //void audioDecode();
    //void decodeFrame();
    bool closeAudioDevice();
    bool openAudioDevice();

    void setAudioSpecFromFile(SDL_AudioSpec& fileAudioSpec, AudioFile* audioFile);
    std::string getFormat(SDL_AudioFormat format);


    //bool synth;
    bool running;
    bool initialized;
    bool stopping;
    bool paused;
    int decodeThreadSleep;
    //TODO: forcing to signed 16 bit is nasty, refactor please
    //unsigned int samplePosition;
    //AudioFile* audioFile;
    //Uint32 audioBufferSize;
    //Uint32 audioBufferDecodedSize;
    //Sint16 *audioBuffer;
};

#endif /*ENGINE_AUDIO_AUDIO_SDL_H_*/
