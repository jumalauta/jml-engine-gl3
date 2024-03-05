#include "AudioSdl.h"
#include "AudioFile.h"

#include "Settings.h"

#include "logger/logger.h"
#include "io/File.h"
#include "io/MemoryManager.h"
#include "time/SystemTime.h"

#include <math.h>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <algorithm>

#include <complex>
#include <iostream>
#include <valarray>
#include <vector>

#include "graphics/Texture.h"
Texture *fftTexture = NULL;
static float* fftData = NULL;
static float* fftDataRing = NULL;
static unsigned int fftDataRingIterator = 0;

static std::mutex mutex;

struct StreamSampleFft {
    StreamSampleFft() {
        samplePosition = 0;
        fftDataRow = NULL;
    }

    ~StreamSampleFft() {
    }

    void setFftDataRow(float *fftData) {
        samplePosition = dynamic_cast<AudioSdl&>(Audio::getInstance()).audioStreams[0].getSamplePosition();
        fftDataRow = NULL;

        if (fftData == NULL) {
            loggerFatal("FFT data must not be initialized with NULL");
            return;
        }

        fftDataRow = new float[Settings::demo.fft.size];

        memcpy(static_cast<void*>(fftDataRow), static_cast<void*>(fftData), Settings::demo.fft.size * sizeof(float));
    }

    void clean() {
        if (fftDataRow != NULL) {
            delete [] fftDataRow;
        }

        samplePosition = 0;
        fftDataRow = NULL;
    }

    unsigned int samplePosition;
    float *fftDataRow;
};

static std::vector<StreamSampleFft> fftDataHistory = std::vector<StreamSampleFft>(); 

static void fft(std::valarray<std::complex<double>>& data)
{
    const size_t dataLength = data.size();
    const size_t dataLengthHalf = dataLength / 2;

    if (dataLength <= 1) {
        return;
    }
 
    std::valarray<std::complex<double>> dataEven = data[std::slice(0, dataLengthHalf, 2)];
    fft(dataEven);
    std::valarray<std::complex<double>> dataOdd = data[std::slice(1, dataLengthHalf, 2)];
    fft(dataOdd);
 
    for (size_t i = 0; i < dataLengthHalf; i++)
    {
        std::complex<double> p = std::polar(1.0, -2 * M_PI * i / dataLength) * dataOdd[i];
        data[i] = dataEven[i] + p;
        data[i+dataLengthHalf] = dataEven[i] - p;
    }
}
 
static double clamp(double value, double low, double high) {
    if (value < low) {
        value = low;
    } else if (value > high) {
        value = high;
    }

    return value;
}

static void fftDataInit();

static int processFft(AudioSdl *audio, Uint8 *outputStream, int outputStreamLength)
{
    //ref: https://www.gaussianwaves.com/2015/11/interpreting-fft-results-complex-dft-frequency-bins-and-fftshift/
    //ref: https://stackoverflow.com/questions/25624548/fft-real-imaginary-abs-parts-interpretation

    const int nSamples = Settings::audio.samples;
    int sampleStride = outputStreamLength/nSamples;

    double freqResolution = audio->getSampleRate() / nSamples;
    std::vector<std::complex<double>> x(nSamples);

    const int pcmBitSize = 16;
    const double maxSignedSize = static_cast<double>(pow(2, pcmBitSize - 1));
    for(int oi=0, i=0; i<nSamples; i++,oi+=sampleStride) {
        unsigned char lsb = static_cast<unsigned char>(outputStream[oi]);
        unsigned char msb = static_cast<unsigned char>(outputStream[oi + 1]);
        double refineValue = (static_cast<short>((lsb & 0xff) | (msb << 8)));

        // clamp shouldn't be needed, as we should know bit sizes and MAX values
        refineValue = clamp(static_cast<double>(refineValue * (1.0 / maxSignedSize)), -1.0, 1.0);

        x[i] = std::complex<double>(refineValue);
    }

    std::valarray<std::complex<double>> data(x.data(), nSamples);
 
    // forward fft
    fft(data);
 
    fftDataInit();

    unsigned int fftDataStride = nSamples/Settings::demo.fft.size;
    for (unsigned int i=0;i<Settings::demo.fft.size;i++) {
        float fftValue = 0.0f;
        for(unsigned int j=i*fftDataStride;j<i*fftDataStride+fftDataStride;j++) {
            fftValue += sqrt(pow(data[j].real(), 2.0) + pow(data[j].imag(), 2.0)); 
        }

        unsigned int ffti = fftDataRingIterator + i;
        fftDataRing[ffti] = (float)clamp(fftValue / (float)fftDataStride / Settings::demo.fft.divisor, Settings::demo.fft.clipMin, Settings::demo.fft.clipMax);
    } 

    if (Settings::gui.tool) {
        StreamSampleFft ssFft;
        ssFft.setFftDataRow(fftDataRing + fftDataRingIterator);
        fftDataHistory.push_back(ssFft);
    }

    fftDataRingIterator += Settings::demo.fft.size;
    if (fftDataRingIterator >= Settings::demo.fft.size * Settings::demo.fft.history) {
        fftDataRingIterator = 0;
    }

    return 0;
}

static void fftDataClear() {
    if (fftData != NULL) {
        memset(static_cast<void*>(fftData), 0, Settings::demo.fft.size * Settings::demo.fft.history * sizeof(float));
        memset(static_cast<void*>(fftDataRing), 0, Settings::demo.fft.size * Settings::demo.fft.history * sizeof(float));

        for (StreamSampleFft ssFft : fftDataHistory) {
            ssFft.clean();
        }
        fftDataHistory.clear();


    }
}

static void fftDataInit() {
    if (fftData == NULL) {
        fftData = new float[Settings::demo.fft.size * Settings::demo.fft.history];
        fftDataRing = new float[Settings::demo.fft.size * Settings::demo.fft.history];

        fftDataClear();
    }
}

static void fftDataDeinit() {
    if (fftData != NULL) {
        fftDataClear();

        delete [] fftData;
        fftData = NULL;

        delete [] fftDataRing;
        fftDataRing = NULL;
    }
}

void fftTextureInit() {
    if (fftTexture == NULL) {
        fftDataInit();

        fftTexture = Texture::newInstance();
        fftTexture->setFormat(TextureFormat::RED);
        fftTexture->setDataType(TextureDataType::FLOAT);
        fftTexture->setTargetType(TextureTargetType::TEXTURE_2D);
        if (fftTexture->create(Settings::demo.fft.size, Settings::demo.fft.history)) {
            loggerDebug("FFT texture created");
        }
    }
}

double getFftDataHistoryBufferTime() {
    if (fftDataHistory.empty()) {
        return 0.0;
    }

    Audio& audio = Audio::getInstance();
    AudioSdl* audioSdl = dynamic_cast<AudioSdl*>(&audio);
    double approximatePosition = fftDataHistory[fftDataHistory.size() - 1].samplePosition / audioSdl->getSampleRate() / static_cast<double>(audioSdl->requestAudioOutSpec.channels);

    return approximatePosition;
}

static void fftDataSamplePosition(unsigned int approximateSamplePosition) {
    if (fftDataHistory.empty()) {
        //nothing to rewind
        return;
    }

    Audio& audio = Audio::getInstance();
    AudioSdl* audioSdl = dynamic_cast<AudioSdl*>(&audio);

    bool currentlyPaused = audio.isPaused();
    if (!currentlyPaused) {
        audio.pause(true);
    }

    int i = fftDataHistory.size() - 1;
    bool found = false;
    for (; i >= 0; i--) {
        StreamSampleFft ssFft = fftDataHistory[i];

        if (abs(static_cast<int>(approximateSamplePosition - ssFft.samplePosition)) <= Settings::audio.samples * audioSdl->requestAudioOutSpec.channels) {
            found = true;
            break;
        }
    }

    if (!found) {
        double approximatePosition = approximateSamplePosition / audioSdl->getSampleRate() / static_cast<double>(audioSdl->requestAudioOutSpec.channels);

        loggerWarning("No FFT data found. size:%d, approximatePosition:%.3f, approximateSamplePosition:%u, minSamplePosition:%u, maxSamplePosition:%u",
            fftDataHistory.size(),
            approximatePosition,
            approximateSamplePosition,
            fftDataHistory[0].samplePosition,
            fftDataHistory[fftDataHistory.size() - 1].samplePosition
        );

        if (!currentlyPaused) {
            audio.pause(false);
        }

        return;
    }

    memset(static_cast<void*>(fftDataRing), 0, Settings::demo.fft.size * Settings::demo.fft.history * sizeof(float));
    fftDataRingIterator = 0;

    for (int fftDataRingI = Settings::demo.fft.size * Settings::demo.fft.history - Settings::demo.fft.size; i >= 0 && fftDataRingI >= 0; i--, fftDataRingI -= Settings::demo.fft.size) {
        StreamSampleFft ssFft = fftDataHistory[i];

        memcpy(static_cast<void*>(fftDataRing + fftDataRingI), static_cast<void*>(ssFft.fftDataRow), Settings::demo.fft.size * sizeof(float));
    }

    if (!currentlyPaused) {
        audio.pause(false);
    }
}

void fftDataSamplePosition(double position) {
    Audio* audio = &Audio::getInstance();
    AudioSdl* audioSdl = dynamic_cast<AudioSdl*>(audio);
    unsigned int approximateSamplePosition = static_cast<unsigned int>(position * audioSdl->getSampleRate() * audioSdl->requestAudioOutSpec.channels);
    fftDataSamplePosition(approximateSamplePosition);
}

void fftTextureUpdate() {
    if (fftTexture) {

        // copy the ring buffer to texture data.
        int length = (Settings::demo.fft.size * Settings::demo.fft.history) - fftDataRingIterator;

        memcpy(static_cast<void*>(fftData), static_cast<void*>(fftDataRing + fftDataRingIterator), length * sizeof(float));
        if (fftDataRingIterator > 0) {
            memcpy(static_cast<void*>(fftData + length), static_cast<void*>(fftDataRing), fftDataRingIterator * sizeof(float));
        }

        // fftData[x * y]
        // goes to texture with coordinates (x) 0.0 - 1.0, (y) 0.0 - 1.0
        // 0.0 - 1.0, 1.0 = latest fft data
        // 0.0 - 1.0, 0.0 = oldest fft data
        // update frequency depends in the sample size etc

        fftTexture->update(fftData);
    }
}

void fftTextureDeinit() {
    if (fftTexture != NULL) {
        fftDataDeinit();

        fftTexture->free();
        fftTexture = NULL;
    }
}


static const int BYTES_PER_SAMPLE = 2;


AudioStream::AudioStream() {
    audioFile = NULL;
    audioBufferSize = 0;
    audioBufferDecodedSize = 0;
    audioBuffer = NULL;
    samplePosition = 0;
    loop = false;
    duration = -1.0;
    audioCallbackTime = 0;
}
AudioStream::~AudioStream() {
}

double AudioStream::getTimeInSeconds() {
    double audioCallbackTimeAdjustment = 0;
    
    AudioSdl* audio = static_cast<AudioSdl*>(&Audio::getInstance());
    if (!audio->isPaused()) {
        audioCallbackTimeAdjustment = (SystemTime::getTimeInMillis() - audioCallbackTime) / 1000.0;
    }

    return std::max((samplePosition) / audio->getSampleRate() / static_cast<double>(audio->requestAudioOutSpec.channels) + audioCallbackTimeAdjustment, 0.0);
}

bool AudioStream::isEnd() {
    if (duration < 0.0) {
        return false;
    }
    if (getTimeInSeconds() >= duration) {
        return true;
    }

    return false;
}

void AudioStream::setDuration(double duration) {
    this->duration = duration;
}

void AudioStream::incrementSamplePosition(unsigned int length) {
     samplePosition += length;

    if (getAudioBufferCurrentPosition() == NULL) {
        resetSamplePosition();
    }   
}
void AudioStream::resetSamplePosition() {
    samplePosition = 0;
}
unsigned int AudioStream::getSamplePosition() {
    return samplePosition;
}
unsigned int AudioStream::getAudioBufferDecodedSize() {
    if (audioFile != NULL) {
        return audioFile->getPcmDataDecodedSize();
    }

    return audioBufferDecodedSize;
}
const void * AudioStream::getAudioBufferCurrentPosition() {
    if (audioBuffer == NULL || getSamplePosition() * BYTES_PER_SAMPLE >= getAudioBufferDecodedSize()) {
        if (getAudioBufferDecodedSize() < audioBufferSize) {
            loggerWarning("Audio decoder lagging!");
            // TODO: Implement some sort of buffering strategy?
            return NULL;
        } else {
            return NULL;
        }
    }

    return &audioBuffer[getSamplePosition()];
}


static std::condition_variable initConditionVariable;

#define DECODE_THREAD_SLEEP_DEFAULT 1
Audio& Audio::getInstance() {
    static AudioSdl audio = AudioSdl();
    return audio;
}


AudioSdl::AudioSdl() {
    SDL_memset(&requestAudioOutSpec, 0, sizeof(requestAudioOutSpec));
    SDL_memset(&requestAudioInSpec, 0, sizeof(requestAudioInSpec));
    SDL_memset(&deviceOut, 0, sizeof(deviceOut));
    SDL_memset(&deviceIn, 0, sizeof(deviceIn));
    //audioFile = NULL;
    //samplePosition = 0;
    //audioBufferSize = 0;
    //audioBufferDecodedSize = 0;
    //audioBuffer = NULL;
    //synth = false;
    running = false;
    stopping = false;
    paused = false;
    initialized = false;
    decodeThreadSleep = DECODE_THREAD_SLEEP_DEFAULT;
}

AudioSdl::~AudioSdl() {
}

double AudioSdl::getTimeInSeconds() {
    return audioStreams[0].getTimeInSeconds();
}

/*void AudioSdl::resetSamplePosition() {
    for(AudioStream audioStream : audioStreams) {
        audioStream.resetSamplePosition();
    }
}

void AudioSdl::incrementSamplePosition(unsigned int length) {
    for(AudioStream audioStream : audioStreams) {
        audioStream.incrementSamplePosition(length);
    }
}
unsigned int AudioSdl::getSamplePosition() {
    return audioStreams[0].samplePosition;
}

unsigned int AudioSdl::getAudioBufferDecodedSize() {
    return audioStreams[0].getAudioBufferDecodedSize();
}

const void *AudioSdl::getAudioBufferCurrentPosition() {
    return audioStreams[0].getAudioBufferCurrentPosition();
}*/

void AudioSdl::audioCallback(void *userData, Uint8 *outputStream, int outputStreamLength)
{
    AudioSdl* audio = static_cast<AudioSdl*>(userData);
    

    uint64_t audioCallbackTime = SystemTime::getTimeInMillis();

    int copyLength = outputStreamLength;

    SDL_memset(outputStream, 0, copyLength);

    if (audio->isPaused()) {
        return;
    }


    if (!audio->audioStreams.empty()) {
        std::lock_guard<std::mutex> lock(mutex);
        for(auto it = audio->audioStreams.begin(); it != audio->audioStreams.end(); ) {
            AudioStream& audioStream = *it;

            if (audioStream.isEnd()) {
                if (audioStream.loop) {
                    loggerError("Duration and loop not supported");
                }

                audio->audioStreams.erase(it);
                continue;
            }

            audioStream.audioCallbackTime = audioCallbackTime;

            const void *audioBufferPointer = audioStream.getAudioBufferCurrentPosition();

            unsigned int audioBufferEndIndex = audioStream.getSamplePosition() * BYTES_PER_SAMPLE + copyLength;
            if (audioBufferEndIndex < audioStream.getAudioBufferDecodedSize()) {
                // mixer needed: https://wiki.libsdl.org/SDL_MixAudio
                //memcpy(outputStream, audioBufferPointer, copyLength);
                SDL_MixAudioFormat(outputStream, static_cast<const Uint8*>(audioBufferPointer), audio->requestAudioOutSpec.format, copyLength, static_cast<int>(SDL_MIX_MAXVOLUME * Settings::audio.captureMixVolume));

                audioStream.incrementSamplePosition(copyLength / BYTES_PER_SAMPLE);
            } else {
                if (audioStream.loop) {
                    copyLength = audioStream.getAudioBufferDecodedSize() - audioStream.getSamplePosition() * BYTES_PER_SAMPLE;
                    //memcpy(outputStream, audioBufferPointer, copyLength);
                    SDL_MixAudioFormat(outputStream, static_cast<const Uint8*>(audioBufferPointer), audio->requestAudioOutSpec.format, copyLength, static_cast<int>(SDL_MIX_MAXVOLUME * Settings::audio.captureMixVolume));
                    audioStream.incrementSamplePosition(copyLength / BYTES_PER_SAMPLE);

                    int reminderCopyLength = outputStreamLength - copyLength;
                    audioStream.resetSamplePosition();
                    if (reminderCopyLength > 0) {
                        const void *restartedAudioBufferPointer = audioStream.getAudioBufferCurrentPosition();
                        //memcpy(outputStream+copyLength, restartedAudioBufferPointer, reminderCopyLength);
                        SDL_MixAudioFormat(outputStream+copyLength, static_cast<const Uint8*>(restartedAudioBufferPointer), audio->requestAudioOutSpec.format, reminderCopyLength, static_cast<int>(SDL_MIX_MAXVOLUME * Settings::audio.captureMixVolume));

                        audioStream.incrementSamplePosition(reminderCopyLength / BYTES_PER_SAMPLE);
                    }
                } else {
                    // Sample ended, remove stream
                    audio->audioStreams.erase(it);
                    continue;  
                }
            }

            ++it;
        }
    }


    if (audio->deviceIn != 0) {
        // Capture audio to output
        Uint8 *captureStream = new Uint8[outputStreamLength];

        int dequeuedLength = static_cast<int>(SDL_DequeueAudio(audio->deviceIn, captureStream, static_cast<unsigned int>(outputStreamLength)));
        if (outputStreamLength > dequeuedLength) {
            SDL_memset(captureStream + dequeuedLength, 0, outputStreamLength - dequeuedLength);
        }

        SDL_MixAudioFormat(outputStream, captureStream, audio->requestAudioOutSpec.format, outputStreamLength, static_cast<int>(SDL_MIX_MAXVOLUME * Settings::audio.captureMixVolume));
        //memcpy(outputStream, captureStream, outputStreamLength);

        delete [] captureStream;
    }

    if (Settings::demo.fft.enable) {
        processFft(audio, outputStream, copyLength);
    }

    if (Settings::audio.mute) {
        SDL_memset(outputStream, 0, copyLength);
    }
}

/*
void AudioSdl::audioDecode() {
    running = true;
    stopping = false;
    initialized = false;
    uint64_t decodeStart = SystemTime::getTimeInMillis();

    while(!stopping) {
        SystemTime::sleepInMillis(decodeThreadSleep);

        decodeFrame();

        if (audioBufferDecodedSize >= audioBufferSize) {
            stopping = true;
        }

        const uint64_t DECODE_BUFFERING_TIME = 2000;
        if (!initialized && (stopping || SystemTime::getTimeInMillis() - decodeStart > DECODE_BUFFERING_TIME)) {
            initialized = true;
        }
    }

    loggerDebug("Audio decoding ended after %u ms", SystemTime::getTimeInMillis() - decodeStart);
    running = false;
}

void AudioSdl::decodeFrame()
{
    const int AMPLITUDE = 28000;

    unsigned int currentDecodePosition = audioBufferDecodedSize;

    for(unsigned int i = currentDecodePosition;
        i < audioBufferSize ;
        i += audioSpec.channels, audioBufferDecodedSize += audioSpec.channels)
    {
        double time = audioBufferDecodedSize/getSampleRate()/audioSpec.channels;
        Sint16 value = (Sint16)(AMPLITUDE * sin(2.0f * M_PI * 440.0f * time));

        switch(audioSpec.channels) {
            case 2: // stereo
                // left channel
                audioBuffer[i] = value;

                // right channel
                audioBuffer[i + 1] = value;
                break;

            case 1: // mono
            default:
                audioBuffer[i] = value;
                break;
        }
    }
}
*/

bool AudioSdl::init() {
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        loggerError("SDL_mixer audio could not initialize! SDL Error: %s", SDL_GetError());
        return false;
    }

    return true;
}

bool AudioSdl::exit() {
    stop();

    //while(running);

    SDL_QuitSubSystem(SDL_INIT_AUDIO);

    return true;
}

void AudioSdl::pause(bool audioPause) {
    if (audioPause) {
        paused = true;
        if (deviceOut != 0) {
            SDL_PauseAudioDevice(deviceOut, 1);
        }
        if (deviceIn != 0) {
            SDL_PauseAudioDevice(deviceIn, 1);
        }

        decodeThreadSleep = 100; // slow down frame decoding. save battery
    } else {
        decodeThreadSleep = DECODE_THREAD_SLEEP_DEFAULT;
        paused = false;
        if (deviceOut != 0) {
            SDL_PauseAudioDevice(deviceOut, 0);
        }
        if (deviceIn != 0) {
            SDL_PauseAudioDevice(deviceIn, 0);
        }
    }

    //loggerTrace("Audio pause: %s", paused ? "true" : "false");
}

void AudioSdl::stop() {
    std::lock_guard<std::mutex> lock(mutex);

    if (deviceOut != 0) {
        SDL_PauseAudioDevice(deviceOut, 1);
    }
    if (deviceIn != 0) {
        SDL_PauseAudioDevice(deviceIn, 1);
    }

    closeAudioDevice();

    audioStreams.clear();
}

bool AudioSdl::closeAudioDevice() {
    initialized = false;
    stopping = true;
    if (deviceOut != 0) {
        SDL_CloseAudioDevice(deviceOut);
        deviceOut = 0;
    }
    if (deviceIn != 0) {
        SDL_CloseAudioDevice(deviceIn);
        deviceIn = 0;
    }

    /*if (synth) {
        if (audioBuffer) {
            free(audioBuffer);

            audioBuffer = NULL;
            audioBufferSize = 0;
        }
    }*/

    setPosition(0.0);

    return true;
}

std::vector<std::string> AudioSdl::getOutputDevices() {
    std::vector<std::string> devices;

    int count = SDL_GetNumAudioDevices(0);
    for (int i = 0; i < count; i++) {
        devices.push_back(std::string(SDL_GetAudioDeviceName(i, 0)));
    }

    return devices;
}

std::string AudioSdl::getFormat(SDL_AudioFormat format) {
    std::string formatString = "{raw:"+std::to_string((int)(format));
    formatString += ",bits:"+std::to_string((SDL_AUDIO_BITSIZE(format)));
    formatString += ",float:"+std::string((SDL_AUDIO_ISFLOAT(format)?"true":"false"));
    formatString += ",bigEndian:"+std::string((SDL_AUDIO_ISBIGENDIAN(format)?"true":"false"));
    formatString += ",signed:"+std::string((SDL_AUDIO_ISSIGNED(format)?"true":"false"));
    formatString += "}";
    return formatString;
}

void AudioSdl::setAudioSpecFromFile(SDL_AudioSpec& fileAudioSpec, AudioFile* audioFile) {
    fileAudioSpec.freq = audioFile->getSampleRate();
    
    // https://wiki.libsdl.org/SDL_AudioFormat
    // In practice we try to aim for AUDIO_S16LSB, but audio file can dictate it
    fileAudioSpec.format = audioFile->getPcmBitSize();
    if (audioFile->isPcmSigned()) {
        fileAudioSpec.format |= SDL_AUDIO_MASK_SIGNED;
    }
    if (audioFile->isPcmBigEndian()) {
        fileAudioSpec.format |= SDL_AUDIO_MASK_ENDIAN;
    }

    // channels specifies the number of output channels. As of SDL 2.0, supported values are 1 (mono), 2 (stereo), 4 (quad), and 6 (5.1).
    // Channel data is interleaved.
    //   Stereo samples are stored in left/right ordering.
    //   Quad is stored in front-left/front-right/rear-left/rear-right order.
    //   5.1 is stored in front-left/front-right/center/low-freq/rear-left/rear-right ordering ("low-freq" is the ".1" speaker). 
    fileAudioSpec.channels = audioFile->getChannels();
}

bool AudioSdl::openAudioDevice() {
    closeAudioDevice();

    AudioFile *audioFile = getAudioFile();
    if (audioFile == NULL) {
        loggerError("Audio file must not be NULL, is it loaded?");
        return false;
    }
    if (! audioFile->isLoaded()) {
        loggerError("Audio file not loaded. file:'%s'", audioFile->getFilePath().c_str());
        return false;
    }

    std::vector<std::string> audioDevices = AudioSdl::getOutputDevices();
    for (std::string audioDevice : audioDevices) {
        loggerTrace("Non-capturing audio device: '%s'", audioDevice.c_str());
    }

    if (Settings::audio.capture) {
        int count = SDL_GetNumAudioDevices(1);
        for (int i = 0; i < count; i++) {
            loggerTrace("Capturing audio device %02d: %s", i, SDL_GetAudioDeviceName(i, 1));
        }
    }

    // https://wiki.libsdl.org/SDL_AudioSpec
    SDL_AudioSpec audioSpec;
    SDL_memset(&audioSpec, 0, sizeof(audioSpec));
    SDL_memset(&requestAudioOutSpec, 0, sizeof(requestAudioOutSpec));

    setAudioSpecFromFile(requestAudioOutSpec, audioFile);

    requestAudioOutSpec.samples = Settings::audio.samples;
    requestAudioOutSpec.callback = audioCallback;
    requestAudioOutSpec.userdata = this;

    int isCapture = 0;
    const char *deviceName = SDL_GetAudioDeviceName(0, isCapture);
    if (Settings::audio.device != "") {
        deviceName = Settings::audio.device.c_str();
    }

    // If your application can only handle one specific data format, pass a zero for allowed_changes and let SDL transparently handle any differences. 
    const int allowedChanges = 0;
    deviceOut = SDL_OpenAudioDevice(deviceName, isCapture, &requestAudioOutSpec, &audioSpec, allowedChanges);

    if (deviceOut == 0) {
        loggerError("Could not initialize SDL Audio for device '%s': %s\n", deviceName, SDL_GetError());
        return false;
    }

    // Compare requested and received audio formats
    // We don't allow changes, SDL should silently convert audioSpecs to requested: https://wiki.libsdl.org/SDL_OpenAudioDevice
    if (requestAudioOutSpec.format != audioSpec.format) {
        closeAudioDevice();
        loggerError("Could not initialize audio. Unsupported format: %s. requestedFormat: %s",
            getFormat(audioSpec.format).c_str(), getFormat(requestAudioOutSpec.format).c_str());
        return false;
    }
    if (requestAudioOutSpec.freq != audioSpec.freq) {
        closeAudioDevice();
        loggerError("Could not initialize audio. Unsupported frequency: %d. requestedFrequency: %d",
            audioSpec.freq, requestAudioOutSpec.freq);
        return false;
    }
    if (requestAudioOutSpec.channels != audioSpec.channels) {
        closeAudioDevice();
        loggerError("Could not initialize audio. Unsupported amount of channels: %d. requestedChannels: %d",
            audioSpec.channels, requestAudioOutSpec.channels);
        return false;
    }
    if (requestAudioOutSpec.callback != audioSpec.callback) {
        closeAudioDevice();
        loggerError("Could not initialize audio. Incorrect callback! requested: 0x%p. received: 0x%p",
            audioSpec.callback, requestAudioOutSpec.callback);
        return false;
    }
    if (requestAudioOutSpec.userdata != audioSpec.userdata) {
        closeAudioDevice();
        loggerError("Could not initialize audio. Incorrect callback! requested: 0x%p. received: 0x%p",
            audioSpec.userdata, requestAudioOutSpec.userdata);
        return false;
    }

    if (requestAudioOutSpec.samples != audioSpec.samples) {
        loggerWarning("Did not receive requested sample amount. received: %d. requested: %d",
            audioSpec.samples, requestAudioOutSpec.samples);

        if (Settings::audio.timeSource && audioSpec.samples > requestAudioOutSpec.samples) {
            // We want relatively small sample frame size so that timer clock increments would stay small
            // hence audio<->timer synch will be removed is frame is too large
            loggerDebug("Too large requested/received sample discrepancy, can't synchronize timer to audio");
            Settings::audio.timeSource = false;
        }
    }

    /*if (synth) {
        audioBufferSize = getSampleRate() * audioSpec.channels * Settings::demo.length;
        audioBuffer = (Sint16*)malloc(sizeof(Sint16) * audioBufferSize);
        if (audioBuffer == NULL) {
            loggerFatal("Could not allocate enough memory for audio buffer");
            return false;
        }
    }*/

    loggerDebug("Output audio initialized successfully! device:'%s', rate:%d, format:%s, channels:%d, samples:%u",
        deviceName, audioSpec.freq, getFormat(audioSpec.format).c_str(), audioSpec.channels, audioSpec.samples);

    if (Settings::audio.capture) {
        SDL_memset(&audioSpec, 0, sizeof(audioSpec));
        SDL_memset(&requestAudioInSpec, 0, sizeof(requestAudioInSpec));
        requestAudioInSpec.freq = requestAudioOutSpec.freq;
        requestAudioInSpec.format = requestAudioOutSpec.format;
        requestAudioInSpec.samples = requestAudioOutSpec.samples;
        requestAudioInSpec.callback = NULL;
        requestAudioInSpec.userdata = this;

        isCapture = 1;
        const char *deviceInName = SDL_GetAudioDeviceName(0, isCapture);
        deviceIn = SDL_OpenAudioDevice(deviceInName, isCapture, &requestAudioInSpec, &audioSpec, allowedChanges);
        if (deviceIn == 0) {
            loggerError("Could not initialize SDL Audio for capturing: %s\n", SDL_GetError());
            return false;
        }
        loggerDebug("Input/capture audio initialized successfully! device:'%s', rate:%d, format:%d, channels:%d, samples:%u",
            deviceInName, audioSpec.freq, audioSpec.format, audioSpec.channels, audioSpec.samples);
    }

    return true;
}

bool AudioSdl::load(const char *filename) {
    /*if (!strcmp(filename, "synth")) {
        synth = true;
    } else {*/

    AudioFile *audioFile = MemoryManager<AudioFile>::getInstance().getResource(std::string(filename));
    if (audioFile == NULL || !audioFile->exists()) {
        // consider fatal, as audio is very important
        loggerFatal("File not found: '%s'", filename);
        return false;
    }

    if (!audioFile->isLoaded()) {
        if (!audioFile->load()) {
            loggerFatal("Could not load audio: '%s'", audioFile->getFilePath().c_str());
            return false;
        }

        if (deviceOut != 0) {
            SDL_AudioSpec fileAudioSpec;
            SDL_memset(&fileAudioSpec, 0, sizeof(fileAudioSpec));
            setAudioSpecFromFile(fileAudioSpec, audioFile);

            if (requestAudioOutSpec.freq != fileAudioSpec.freq
                || requestAudioOutSpec.format != fileAudioSpec.format
                || requestAudioOutSpec.channels != fileAudioSpec.channels) {
                loggerError("Output audio setup is incompatible with loaded file! samples:%u, originalRate:%d, originalFormat:%s, originalChannels:%d, newFile:%s, newRate:%d, newFormat:%s, newChannels:%d",
                    requestAudioOutSpec.samples, requestAudioOutSpec.freq, getFormat(requestAudioOutSpec.format).c_str(), requestAudioOutSpec.channels,
                    audioFile->getFilePath().c_str(), fileAudioSpec.freq, getFormat(fileAudioSpec.format).c_str(), fileAudioSpec.channels
                );
            }
        }
    }

    /*if (synth) {
        running = true;
        std::thread decodeThread(&AudioSdl::audioDecode, this);
        decodeThread.detach();

        //wait that music has buffered enough
        while(running && !initialized) {
            SystemTime::sleepInMillis(1);
        }
    }*/

    return true;
}

bool AudioSdl::play(const char *filename) {
    AudioStream audioStream;

    audioStream.audioFile = MemoryManager<AudioFile>::getInstance().getResource(std::string(filename));
    if (audioStream.audioFile == NULL || !audioStream.audioFile->exists()) {
        // consider fatal, as audio is very important
        loggerFatal("File not found: '%s'", filename);
        return false;
    }

    if (!audioStream.audioFile->isLoaded()) {
        loggerError("Audio file not loaded");
        return false;
    }

    audioStream.audioBuffer = static_cast<Sint16*>(audioStream.audioFile->getPcmData());
    audioStream.audioBufferSize = audioStream.audioFile->getPcmDataSize();
    audioStream.audioBufferDecodedSize = audioStream.getAudioBufferDecodedSize();
    audioStream.loop = Settings::demo.songLoop;
    audioStream.setDuration(audioStream.audioFile->getDuration());
    audioStream.audioCallbackTime = SystemTime::getTimeInMillis();

    std::lock_guard<std::mutex> lock(mutex);

    /*if (deviceOut != 0 && !audioStreams.empty()) {
        // try to make audio playing sound more real-time
        double adjustPosition = (SystemTime::getTimeInMillis() - audioStreams[0].audioCallbackTime) / 1000.0;
        audioStream.samplePosition = static_cast<unsigned int>(adjustPosition * getSampleRate() * requestAudioOutSpec.channels);
    }*/

    audioStreams.push_back(audioStream);

    if (audioStreams.size() == 1) {
        if (!openAudioDevice()) {
            return false;
        }
        if (deviceOut == 0 && deviceIn == 0) {
            loggerWarning("There are no audio devices to play");
            return false;
        }

        if (deviceOut != 0) {
            SDL_PauseAudioDevice(deviceOut, 0);
        }
        if (deviceIn != 0) {
            SDL_PauseAudioDevice(deviceIn, 0);
        }

        setPosition(0.0);

        if (Settings::demo.length < 0.0 && audioStream.audioFile->getTotalTime() > 0.0) {
            Settings::demo.length = audioStream.audioFile->getTotalTime();
        }
    }

    return true;
}

void AudioSdl::setDuration(const char *filename, double duration) {
    AudioFile *audioFile = MemoryManager<AudioFile>::getInstance().getResource(std::string(filename));
    if (audioFile == NULL) {
        // consider fatal, as audio is very important
        loggerWarning("File not found: '%s'", filename);
        return;
    }

    audioFile->setDuration(duration);
}

void AudioSdl::setPosition(double position) {    
    bool currentlyPaused = isPaused();
    if (!currentlyPaused) {
        pause(true);
    }

    auto it = audioStreams.begin();
    while (it != audioStreams.end())
    {
        AudioStream& audioStream = *it;

        audioStream.samplePosition = static_cast<unsigned int>(position * getSampleRate() * requestAudioOutSpec.channels);
        if (audioStream.loop) {
            audioStream.samplePosition = audioStream.samplePosition % (audioStream.getAudioBufferDecodedSize() / BYTES_PER_SAMPLE);
        }

        //TODO. refactor! assuming first track is the song...
        if (it == audioStreams.begin()) {
            if (audioStream.samplePosition == 0 || fabs(getFftDataHistoryBufferTime()-position) >= 0.5) {
                loggerTrace("Clearing FFT history! position:%.3f, bufferMaxPosition:%.3f", position, getFftDataHistoryBufferTime());
                fftDataClear();
            }

            fftDataSamplePosition(audioStream.samplePosition);
        }

        ++it;
    }

    if (!currentlyPaused) {
        pause(false);
    }
}

double AudioSdl::getSampleRate() {
    return static_cast<double>(requestAudioOutSpec.freq);
}

bool AudioSdl::isPaused() {
    return paused;
}

AudioFile* AudioSdl::getAudioFile() {
    if (audioStreams.empty()) { return NULL; }
    return audioStreams[0].audioFile;
}
