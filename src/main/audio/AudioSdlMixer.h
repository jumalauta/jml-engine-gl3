#ifndef ENGINE_AUDIO_AUDIO_SDL_MIXER_H_
#define ENGINE_AUDIO_AUDIO_SDL_MIXER_H_

#include "Audio.h"

#include "SDL.h"
#include "SDL_mixer.h"

class AudioSdlMixer : public Audio {
public:
    AudioSdlMixer();
    bool init();
    bool exit();
    void pause(bool audioPause);
    void stop();
    bool load(const char *filename);
    bool play();
    void setPosition(double position);
    double getTimeInSeconds();
    void incrementSamplePosition(int len);
    double getSampleRate();
private:
    Mix_Music *sound;
    int sampleRate;
    Uint16 audioFormat;
    int audioChannels;
    int chunkSize;
    unsigned int sampleSize;
    unsigned int samplePosition;
    int currentChannel;
};

#endif /*ENGINE_AUDIO_AUDIO_SDL_MIXER_H_*/
