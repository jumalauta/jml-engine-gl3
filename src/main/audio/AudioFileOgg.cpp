#include "AudioFileOgg.h"
#include "Audio.h"

#include "logger/logger.h"
#include "time/SystemTime.h"

#include <algorithm>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#define DEFAULT_BUFFER_SIZE 4096

AudioFile* AudioFile::newInstance(std::string filePath) {
    AudioFile *audioFile = new AudioFileOgg(filePath);
    if (audioFile == NULL) {
        loggerFatal("Could not allocate memory for audio file:'%s'", filePath.c_str());
        return NULL;
    }

    if (! audioFile->isSupported()) {
        delete audioFile;
        return NULL;
    }

    return audioFile;
}

AudioFileOgg::AudioFileOgg(std::string filePath) : AudioFile(filePath) {
    pcmData = NULL;
    pcmDataDecodedSize = pcmDataSize = 0L;
}

AudioFileOgg::~AudioFileOgg() {
    if (pcmData != NULL) {
        delete [] pcmData;
        pcmData = NULL;
        pcmDataDecodedSize = pcmDataSize = 0L;
    }
}

bool AudioFileOgg::isSupported() {
    std::string fileExtension = getFileExtension();
    std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::tolower);
    if (fileExtension == "ogg") {
        return true;
    }

    return false;
}

bool AudioFileOgg::load() {
    loadLastModified = lastModified();

    if (!isFile()) {
        loggerError("Not a file. file:'%s'", getFilePath().c_str());
        return false;
    }

    if (!isSupported()) {
        loggerError("File type not supported. file:'%s'", getFilePath().c_str());
        return false;
    }

    if (!loadRaw()) {
        loggerError("Could not load file. file:'%s'", getFilePath().c_str());
        return false;
    }

    bool audioPlaying = false;
    Audio& audio = Audio::getInstance();
    double playTime = 0.0;
    //TODO: Better logic needed when there's more than one audio file playing concurrently
    if (audio.getAudioFile() == this) {
        playTime = audio.getTimeInSeconds();
        audio.stop();
        audioPlaying = true;
    }

    if (!decode()) {
        return false;
    }

    if (audioPlaying) {
        audio.load(getFilePath().c_str());
        audio.play(getFilePath().c_str());
        audio.setPosition(playTime);
    }

    loggerInfo("Loaded audio. file:'%s', length:%.2f, channels:%d, sampleRate:%.0f",
        getFilePath().c_str(), totalTime, channels, sampleRate);

    return true;
}

// TODO: Make class File internal methods
static size_t dataCursor = 0;

static size_t read_func(void *ptr, size_t size, size_t nmemb, void *datasource) {
    AudioFile *file = static_cast<AudioFile*>(datasource);

    size_t readLength = size * nmemb;
    if (dataCursor + readLength > file->length()) {
        readLength = file->length() - dataCursor;
    }

    if (readLength <= 0) {
        return 0;
    }

    memcpy(ptr, file->getData() + dataCursor, readLength);
    dataCursor += readLength;

    return readLength;
}

static int seek_func(void *datasource, ogg_int64_t offset, int whence) {
    AudioFile *file = static_cast<AudioFile*>(datasource);

    size_t dataCursorNew = 0;
    switch (whence) {
        case SEEK_SET:
            dataCursorNew = offset;
            break;
        case SEEK_CUR:
            dataCursorNew = dataCursor + offset;
            break;
        case SEEK_END:
            dataCursorNew = file->length() - 1;
            break;
        default:
            return -1;
    }

    if (dataCursorNew >= file->length()) {
        return -1;
    }

    dataCursor = dataCursorNew;

    return 0;
}

static long tell_func(void *datasource) {
    AudioFile *file = static_cast<AudioFile*>(datasource);
    return static_cast<long>(dataCursor);
}

bool AudioFileOgg::decode() {
    uint64_t loadStart = SystemTime::getTimeInMillis();

    ov_callbacks callbacks;
    memset(&callbacks, 0, sizeof(callbacks));
    callbacks.read_func = read_func;
    callbacks.seek_func = seek_func;
    callbacks.close_func = NULL;
    callbacks.tell_func = tell_func;

    OggVorbis_File vorbisFile;

    dataCursor = 0;
    int ret = ov_open_callbacks(this, &vorbisFile, NULL, 0, callbacks);
    if (ret < 0) {
        const char *message = "";

        switch(ret) {
            // A read from media returned an error.
            case OV_EREAD:
                message = "read error";
                break;
            // Bitstream does not contain any Vorbis data.
            case OV_ENOTVORBIS:
                message = "file is not recognized as ogg vorbis file";
                break;
            // Vorbis version mismatch.
            case OV_EVERSION:
                message = "vorbis version mismatch";
                break;
            // Invalid Vorbis bitstream header.
            case OV_EBADHEADER:
                message = "corrupt or invalid stream header";
                break;
            // Internal logic fault; indicates a bug or heap/stack corruption.
            case OV_EFAULT:
                message = "internal logic fault";
                break;
            default:
                message = "unknown error";
                break;
        }

        loggerError("Error opening ogg vorbis file, %s. file:'%s'", message, getFilePath().c_str());
        ov_clear(&vorbisFile);

        return false;
    }

    // ref: https://www.xiph.org/vorbis/doc/Vorbis_I_spec.html#x1-810004.3.9
    // ref: https://wiki.libsdl.org/SDL_AudioSpec
    // Channel mapping needed to reorder the audio channels for surround
    vorbis_info *vorbisInfo = ov_info(&vorbisFile, -1);
    if (vorbisInfo == NULL) {
        loggerError("Could not read ogg vorbis file info header. file:'%s'", getFilePath().c_str());
        ov_clear(&vorbisFile);
        return false;
    }
    channels = vorbisInfo->channels;
    sampleRate = static_cast<double>(vorbisInfo->rate);
    totalTime = ov_time_total(&vorbisFile, -1);

    if (pcmData != NULL) {
        delete [] pcmData;
        pcmData = NULL;
        pcmDataDecodedSize = pcmDataSize = 0L;
    }

    int USE_BIG_ENDIAN = pcmBigEndian ? 1 : 0;
    int USE_16BIT_SAMPLES = 1;
    switch(pcmBitSize) {
        default:
            loggerError("Defined bitsize(%d) not supported in ogg decoding. file:'%s'", pcmBitSize, getFilePath().c_str());
            return false;
        case 8:
            USE_16BIT_SAMPLES = 1;
            break;
        case 16:
            USE_16BIT_SAMPLES = 2;
            break;
    }
    const int USE_SIGNED_DATA = pcmSigned ? 1 : 0;

    pcmDataSize = ov_pcm_total(&vorbisFile, -1) * (pcmBitSize / 8) * channels;
    pcmData = new char[pcmDataSize];
    if (pcmData == NULL) {
        loggerFatal("Could not allocate memory for audio. file:'%s'", getFilePath().c_str());
        ov_clear(&vorbisFile);
        return false;
    }

    int currentSection = 0;
    while (pcmDataDecodedSize < pcmDataSize) {
        int oldSection = currentSection;
        long readRet=ov_read(&vorbisFile,
            pcmData+pcmDataDecodedSize,DEFAULT_BUFFER_SIZE,USE_BIG_ENDIAN,USE_16BIT_SAMPLES,USE_SIGNED_DATA,&currentSection);
        if (readRet > 0) {
            pcmDataDecodedSize += readRet;

            if (oldSection != currentSection) {
                vorbis_info *vorbisInfoNew = ov_info(&vorbisFile, -1);

                bool formatChanged = false;
                if (vorbisInfo->channels != vorbisInfoNew->channels) {
                    loggerError("Amount of channels (%d -> %d) has changed in the ogg. This is not supported, please re-encode the audio! rawPosition:%ld/%ld, file:'%s'",
                        vorbisInfo->channels, vorbisInfoNew->channels, pcmDataDecodedSize, pcmDataSize, getFilePath().c_str());
                    formatChanged = true;
                }
                if (vorbisInfo->rate != vorbisInfoNew->rate) {
                    loggerError("Frequency (%ld -> %ld) has changed in the ogg. This is not supported, please re-encode the audio! rawPosition:%ld/%ld, file:'%s'",
                        vorbisInfo->channels, vorbisInfoNew->channels, pcmDataDecodedSize, pcmDataSize, getFilePath().c_str());
                    formatChanged = true;
                }

                if (formatChanged) {
                    // FIXME: reconvert audio instead of failing?
                    // possible ref: SDL_BuildAudioCVT & SDL_ConvertAudio
                    ov_clear(&vorbisFile);
                    return false;
                }
            }
        } else if (readRet == 0) {
            // end-of-file
            break;
        } else {
            // error occurred in reading

            switch(readRet) {
                // indicates that an invalid stream section was supplied to libvorbisfile, or the requested link is corrupt.
                case OV_EBADLINK:
                    loggerError("Could not decode ogg, corrupt or invalid stream section. errorCode: %d, file:'%s'", ret, getFilePath().c_str());
                    break;
                // indicates there was an interruption in the data. (one of: garbage between pages, loss of sync followed by recapture, or a corrupt page)
                case OV_HOLE:
                    loggerError("Could not decode ogg, interruption in data. errorCode: %d, file:'%s'", ret, getFilePath().c_str());
                    break;
                default:
                    loggerError("Could not decode ogg. errorCode: %d, file:'%s'", ret, getFilePath().c_str());
                    break;
            }

            ov_clear(&vorbisFile);
            return false;
        }
    }

    ov_clear(&vorbisFile);

    loggerDebug("Music decoded in %u ms. file:'%s'", SystemTime::getTimeInMillis() - loadStart, getFilePath().c_str());
        
    return true;
}
