#ifndef ENGINE_AUDIO_AUDIOFILE_H_
#define ENGINE_AUDIO_AUDIOFILE_H_

#include "io/File.h"

#include <string>

class AudioFile : public File {
public:
    virtual ~AudioFile() {}
    virtual bool load() = 0;
    virtual bool isSupported() = 0;
    static AudioFile* newInstance(std::string filePath);
    virtual int getChannels();
    virtual double getSampleRate();
    virtual double getTotalTime();
    virtual bool isPcmBigEndian();
    virtual bool isPcmSigned();
    virtual unsigned char getPcmBitSize();
    virtual void calculateHistogram(float* histogramData, size_t histogramDataSize, float &minValue, float &maxValue);
    virtual void setDuration(double duration);
    virtual double getDuration();

    virtual void* getPcmData();
    virtual long getPcmDataSize();
    virtual long getPcmDataDecodedSize();
protected:
    explicit AudioFile(std::string filePath);
    char* pcmData;
    long pcmDataSize;
    long pcmDataDecodedSize;

    int channels;
    double sampleRate;
    double totalTime;
    double duration;
    bool pcmBigEndian;
    bool pcmSigned;
    unsigned char pcmBitSize;
};

#endif /*ENGINE_AUDIO_AUDIOFILE_H_*/
