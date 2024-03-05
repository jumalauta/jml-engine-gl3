#ifndef ENGINE_AUDIO_AUDIOFILEOGG_H_
#define ENGINE_AUDIO_AUDIOFILEOGG_H_

#include "AudioFile.h"

#include <string>

class AudioFileOgg : public AudioFile {
public:
    explicit AudioFileOgg(std::string filePath);
    ~AudioFileOgg();
    bool load();
    bool isSupported();
private:
    bool decode();
};

#endif /*ENGINE_AUDIO_AUDIOFILEOGG_H_*/
