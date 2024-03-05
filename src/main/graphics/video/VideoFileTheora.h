#ifndef ENGINE_GRAPHICS_VIDEO_VIDEOFILETHEORA_H_
#define ENGINE_GRAPHICS_VIDEO_VIDEOFILETHEORA_H_

#include "VideoFile.h"

class Texture;
struct video_theora_frame_t;
struct video_theora_t;


class VideoFileTheora : public VideoFile {
public:
    explicit VideoFileTheora(std::string filePath);
    ~VideoFileTheora();
    bool load(bool rollback=false);
    bool isSupported();

    void setSpeed(double speed);
    void setFps(double fps);
    void setLoop(bool loop);
    void play();
    void setStartTime(float startTime);
    void setTime(float time);
    void setLength(float length);
    void stop();
    void pause();
    void draw();

private:
    bool loadVideoTheoraFrame(double time);
    bool loadVideoTheora();
    void freeFrameTheora(struct video_theora_frame_t **frame);
    bool freeVideoTheora();
    bool videoRefreshFrame();

    unsigned int frame;
    float startTime;
    float pauseTime;
    float length;
    int state;
    double fps;
    double speed;
    unsigned int currentFrame;
    bool paused;
    bool useAudio;
    bool loop;
    Texture *frameTexture;
    int codecType;
    struct video_theora_t *codec;
};

#endif /*ENGINE_GRAPHICS_VIDEO_VIDEOFILETHEORA_H_*/
