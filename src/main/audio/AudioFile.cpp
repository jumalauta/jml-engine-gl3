#include "AudioFile.h"

#include "Settings.h"
#include "logger/logger.h"
#include "time/SystemTime.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <limits.h>

#include <algorithm>

static double clamp(double value, double low, double high) {
    if (value < low) {
        value = low;
    } else if (value > high) {
        value = high;
    }

    return value;
}

AudioFile::AudioFile(std::string filePath) : File(filePath) {
    channels = 2;
    sampleRate = 44100.0;
    totalTime = 0.0;
    pcmBigEndian = false;
    pcmSigned = true;
    pcmBitSize = 16;

    pcmData = NULL;
    pcmDataSize = 0;
    pcmDataDecodedSize = 0;

    duration = -1.0;

    setModifyGracePeriod(Settings::gui.largeFileModifyGracePeriod);
}

int AudioFile::getChannels() {
    return channels;
}

double AudioFile::getSampleRate() {
    return sampleRate;
}

double AudioFile::getTotalTime() {
    return totalTime;
}

bool AudioFile::isPcmBigEndian() {
    return pcmBigEndian;
}

bool AudioFile::isPcmSigned() {
    return pcmSigned;
}

unsigned char AudioFile::getPcmBitSize() {
    return pcmBitSize;
}

void* AudioFile::getPcmData() {
    return pcmData;
}

long AudioFile::getPcmDataSize() {
    return pcmDataSize;
}

long AudioFile::getPcmDataDecodedSize() {
    return pcmDataDecodedSize;
}

void AudioFile::setDuration(double duration) {
    this->duration = duration;
}

double AudioFile::getDuration() {
    return duration;
}

void AudioFile::calculateHistogram(float* histogramData, size_t histogramDataSize, float &minValue, float &maxValue) {
    if (histogramData == NULL || histogramDataSize == 0) {
        loggerError("Invalid histogram input parameters");
        return;
    }

    minValue = 0.0f;
    maxValue = 0.0f;

    memset(histogramData, 0, histogramDataSize * sizeof(float));

    const int bytes = pcmBitSize / 8;
    if (getPcmData() == NULL || getPcmDataDecodedSize() <= 0 || channels == 0 || bytes < 1) {
        loggerWarning("File not decoded, can't calculate histogram");
        return;
    }

    long pcmDataValueSize = getPcmDataDecodedSize() / bytes;
    if (static_cast<long>(histogramDataSize) >= pcmDataValueSize) {
        loggerError("Too many histogram values requested. histogramDataSize:%ld, pcmDataValueSize:%ld", histogramDataSize, pcmDataValueSize);
        return;
    }
    double maxSignedSize = static_cast<double>(pow(2, pcmBitSize - 1));

    const int histogramStride = 2;
    size_t bytesPerHistogramValue = getPcmDataDecodedSize() / histogramDataSize * histogramStride;

    size_t pcmDataOffset = 0;
    for (size_t i = 0; i < histogramDataSize; i += histogramStride) {

        double divisor = 0.0;
        double loudestPositiveValue = 0.0;
        double loudestNegativeValue = 0.0;

        // WARNING: Assuming little endian CPU architecture
        // TODO: Make endian-safe
        for (size_t refine = 0; refine < bytesPerHistogramValue; refine += bytes, divisor++) {
            unsigned char lsb = static_cast<unsigned char>(pcmData[pcmDataOffset + refine]);
            double refineValue = 0.0;
            if (bytes == 1) {
                refineValue += lsb;
            } else if (bytes == 2) {
                unsigned char msb = static_cast<unsigned char>(pcmData[pcmDataOffset + refine + 1]);
                refineValue += (static_cast<short>((lsb & 0xff) | (msb << 8)));
            } else {
                loggerWarning("Bitsize %d not supported", pcmBitSize);
                break;
            }

            // clamp shouldn't be needed, as we should know bit sizes and MAX values
            refineValue = clamp(static_cast<double>(refineValue * (1.0 / maxSignedSize)), -1.0, 1.0);
            if (refineValue > 0.0) {
                loudestPositiveValue += refineValue;
            } else {
                loudestNegativeValue += refineValue;
            }
        }

        loudestNegativeValue = loudestNegativeValue / divisor;
        loudestPositiveValue = loudestPositiveValue / divisor;

        histogramData[i] = static_cast<float>(loudestPositiveValue);
        if (histogramStride > 1) {
            histogramData[i + 1] = static_cast<float>(loudestNegativeValue);
        }

        minValue = std::min(static_cast<float>(loudestNegativeValue), minValue);
        maxValue = std::max(static_cast<float>(loudestPositiveValue), maxValue);

        pcmDataOffset += bytesPerHistogramValue;
        if (pcmDataOffset >= static_cast<size_t>(getPcmDataDecodedSize())) {
            break;
        }
    }
}
