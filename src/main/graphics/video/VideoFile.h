#ifndef ENGINE_GRAPHICS_VIDEO_VIDEOFILE_H_
#define ENGINE_GRAPHICS_VIDEO_VIDEOFILE_H_

#include "graphics/Image.h"

class VideoFile : public Image {
public:
    static VideoFile* newInstance(std::string filePath);
    virtual ~VideoFile();

    virtual void setSpeed(double speed) = 0;
    virtual void setFps(double fps) = 0;
    virtual void setLoop(bool loop) = 0;
    virtual void play() = 0;
    virtual void setStartTime(float startTime) = 0;
    virtual void setLength(float length) = 0;
    virtual void setTime(float time) = 0;
    virtual void stop() = 0;
    virtual void pause() = 0;
    virtual void draw() = 0;
protected:
    explicit VideoFile(std::string filePath);
};

#endif /*ENGINE_GRAPHICS_VIDEO_VIDEOFILE_H_*/
