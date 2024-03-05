#include "AudioSdlMixer.h"

#include "Settings.h"

#include "logger/logger.h"
#include "io/File.h"

Audio& Audio::getInstance() {
    static AudioSdlMixer audio = AudioSdlMixer();
    return audio;
}

AudioSdlMixer::AudioSdlMixer() {
    sampleRate = 44100;
    audioFormat = MIX_DEFAULT_FORMAT;
    audioChannels = 2;
    chunkSize = 512;
    sampleSize = 0;
    samplePosition = 0;
    currentChannel = 0;
    sound = NULL;
}

static void processAudio(void *udata, Uint8 *stream, int len)
{
    AudioSdlMixer& audio = dynamic_cast<AudioSdlMixer&>(Audio::getInstance());

    if (Settings::audio.mute) {
        SDL_memset(stream, 0, len);
    }

    audio.incrementSamplePosition(len);

    // TODO: Add ability to draw sound waves if needed? To enable FFT support etc...
    // memcpy(stream[currentChannel],_stream,len);
    // currentChannel = (currentChannel + 1) % 2;
}

double AudioSdlMixer::getTimeInSeconds() {
    return samplePosition/static_cast<double>(sampleRate);
}

void AudioSdlMixer::incrementSamplePosition(int len) {
    samplePosition += len / sampleSize;
}

bool AudioSdlMixer::init() {
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        loggerError("SDL_mixer audio could not initialize! SDL Error: %s", SDL_GetError());
        return false;
    }
    if (Mix_OpenAudio(sampleRate, audioFormat, audioChannels, chunkSize) < 0) {
        loggerError("SDL_mixer could not initialize! SDL_mixer Error: %s", Mix_GetError());
        return false;
    }

    Mix_QuerySpec(&sampleRate, &audioFormat, &audioChannels);
    sampleSize = (audioFormat & 0xFF) / 8 + audioChannels;
    loggerDebug("Audio initialized successfully! rate:%d, format:%d, channels:%d, sampleSize:%u", sampleRate, audioFormat, audioChannels, sampleSize);

    return true;
}

bool AudioSdlMixer::exit() {
    Mix_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);

    return true;
}

void AudioSdlMixer::pause(bool audioPause) {
    if (audioPause) {
        Mix_Pause(-1);
        Mix_PauseMusic();
        Mix_SetPostMix(NULL, NULL);
    } else {
        Mix_Resume(-1);
        Mix_ResumeMusic();
        Mix_SetPostMix(processAudio, NULL);
    }
}

void AudioSdlMixer::stop() {
    Mix_HaltChannel(-1);
    Mix_HaltMusic();

    setPosition(0.0);
}

bool AudioSdlMixer::load(const char *filename) {
    File file = File(filename);
    if (!file.exists()) {
        loggerWarning("File not found: '%s'", file.getFilePath().c_str());
        return false;
    }

    sound = Mix_LoadMUS(file.getFilePath().c_str());
    if (!sound) {
        loggerError("Could not load music. error:'%s'", Mix_GetError());
        return false;
    }

    return true;
}

bool AudioSdlMixer::play() {
    if (!sound) {
        loggerError("Music not loaded!");
        return false;
    }

    const int loop = 0;
    if (Mix_PlayMusic(sound, loop) < 0) {
        loggerError("Could not play music. error:'%s'", Mix_GetError());
        return false;
    }

    samplePosition = 0;
    Mix_SetPostMix(processAudio, NULL);

    return true;
}

void AudioSdlMixer::setPosition(double position) {
    Mix_RewindMusic();
    if (Mix_SetMusicPosition(position) < 0) {
        loggerWarning("Could not change music position. error:'%s'", Mix_GetError());
        return;
    }

    samplePosition = static_cast<unsigned int>(position * static_cast<double>(sampleRate));
}

double AudioSdlMixer::getSampleRate() {
    return static_cast<double>(sampleRate);
}
