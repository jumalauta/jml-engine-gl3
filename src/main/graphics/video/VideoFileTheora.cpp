#include "VideoFileTheora.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <math.h>

#include "graphics/Texture.h"
#include "time/SystemTime.h"
#include "time/Timer.h"
#include "logger/logger.h"
#include "graphics/Graphics.h"
#include "EnginePlayer.h"

#include <algorithm>
#include "GL/gl3w.h"

#ifdef __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

#include "theoraplay/theoraplay.h"

struct video_theora_frame_t {
    const THEORAPLAY_VideoFrame *video;
    const THEORAPLAY_AudioPacket *audio;
    long int fileCursor;
};

struct video_theora_t {
    THEORAPLAY_Decoder *decoder;
    struct video_theora_frame_t *currentFrame;
    SDL_AudioSpec audioSpec;
};

//static Uint32 baseticks = 0;

typedef struct AudioQueue
{
    const THEORAPLAY_AudioPacket *audio;
    int offset;
    struct AudioQueue *next;
} AudioQueue;

static volatile AudioQueue *audio_queue = NULL;
static volatile AudioQueue *audio_queue_tail = NULL;

static void SDLCALL audio_callback(void *userdata, Uint8 *stream, int len)
{

    if (userdata)
    {
        //DO NOTHING
    }
    

    // !!! FIXME: this should refuse to play if item->playms is in the future.
    //const Uint32 now = SystemTime::getTimeInMillis() - baseticks;
    Sint16 *dst = (Sint16 *) stream;

    while (audio_queue && (len > 0))
    {
        volatile AudioQueue *item = audio_queue;
        AudioQueue *next = item->next;
        const int channels = item->audio->channels;

        const float *src = item->audio->samples + (item->offset * channels);
        int cpy = (item->audio->frames - item->offset) * channels;
        int i;

        if (cpy > (int)(len / sizeof (Sint16)))
            cpy = len / sizeof (Sint16);

        for (i = 0; i < cpy; i++)
        {
            const float val = *(src++);
            if (val < -1.0f)
                *(dst++) = -32768;
            else if (val > 1.0f)
                *(dst++) = 32767;
            else
                *(dst++) = (Sint16) (val * 32767.0f);
        } // for

        item->offset += (cpy / channels);
        len -= cpy * sizeof (Sint16);

        if (item->offset >= item->audio->frames)
        {
            THEORAPLAY_freeAudio(item->audio);
            free((void *) item);
            audio_queue = next;
        } // if
    } // while

    if (!audio_queue)
        audio_queue_tail = NULL;

    if (len > 0)
        memset(dst, '\0', len);
} // audio_callback


static void queue_audio(const THEORAPLAY_AudioPacket *audio)
{
    AudioQueue *item = (AudioQueue *) malloc(sizeof (AudioQueue));
    assert(item);

    item->audio = audio;
    item->offset = 0;
    item->next = NULL;

    SDL_LockAudio();
    if (audio_queue_tail)
        audio_queue_tail->next = item;
    else
        audio_queue = item;
    audio_queue_tail = item;
    SDL_UnlockAudio();
} // queue_audio


#define CODEC_NULL 0
#define CODEC_THEORA 1


#define MAXFRAMES 0xFFFF

static bool timerIsAddTimeGracePeriod() {
    return false; //TODO: is timer grace period needed?
}

VideoFile* VideoFile::newInstance(std::string filePath) {
    VideoFile *videoFile = new VideoFileTheora(filePath);
    if (videoFile == NULL) {
        loggerFatal("Could not allocate memory for video file:'%s'", filePath.c_str());
        return NULL;
    }

    if (! videoFile->isSupported()) {
        delete videoFile;
        return NULL;
    }

    return videoFile;
}

#define VIDEO_STOPPED 0
#define VIDEO_PLAYING 1
#define VIDEO_PAUSED 2


VideoFileTheora::VideoFileTheora(std::string filePath) : VideoFile(filePath) {
    codec = NULL;
	freeVideoTheora();
}

VideoFileTheora::~VideoFileTheora() {
    if (texture != NULL) {
        loggerDebug("Deconstructing video and texture. file:'%s', texture:0x%p", getFilePath().c_str(), texture);
        delete texture;
    }

    freeVideoTheora();
}

bool VideoFileTheora::isSupported() {
    std::string fileExtension = getFileExtension();
    std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::tolower);
    if (fileExtension == "ogv" || fileExtension == "ogg") {
        return true;
    }

    return false;
}

bool VideoFileTheora::load(bool rollback) {
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

	freeVideoTheora();

    codecType = CODEC_THEORA;
    codec = (struct video_theora_t*)malloc(sizeof(struct video_theora_t));
    memset(codec, 0, sizeof(struct video_theora_t));

    if (codec == NULL) {
    	loggerFatal("Could not allocate memory for video. file:'%s'", getFilePath().c_str());
    	return false;
    }

    codec->currentFrame = NULL;
    codec->decoder = NULL;

    if (!loadVideoTheora())
    {
        loggerError("Could not load video '%s'!", getFilePath().c_str());
        return false;
    }

    return true;
}

bool VideoFileTheora::loadVideoTheoraFrame(double time)
{
    if (time > 0.0 && timerIsAddTimeGracePeriod())
    {
        return true;
    }

    if (codec->currentFrame != NULL && codec->currentFrame->video->playms/1000.0 > time+0.5)
    {
        THEORAPLAY_stopDecode(codec->decoder);
        codec->decoder = NULL;
    }

    if (codec->decoder == NULL)
    {
        if (codec->decoder)
        {
            THEORAPLAY_stopDecode(codec->decoder);
        }

        loggerDebug("Start decode again '%s'",getFilePath().c_str());
        codec->decoder = THEORAPLAY_startDecodeFile(getFilePath().c_str(), MAXFRAMES, THEORAPLAY_VIDFMT_RGB);
        if (!codec->decoder)
        {
            loggerError("Could not decode file! '%s'");
            return false;
        }

        //freeVideoFramesTheora(video);

        //THEORAPLAY_setDecodeTime(codec->decoder, 0.0);
    }

    //unsigned int frame = time*fps;
    THEORAPLAY_setDecodeTime(codec->decoder, time);

    //loadVideoTheoraFrame(0);

    //const unsigned int MAX_PLAY_TIME_MS = 30000;
    const unsigned int MAX_DECODE_WAIT_MS = 1000;

    //loggerDebug("Getting frame %d (%.2f) '%s'!", frame, frame/fps, getFilePath().c_str());

    //loggerDebug("=> Getting frame %d/%d!", frame, codec->framesSize);
    unsigned int loadStart = SystemTime::getTimeInMillis();
    const THEORAPLAY_VideoFrame *video_frame = NULL;
    
    int pauseWait = 0;
    while(1)
    {
        video_frame = THEORAPLAY_getVideo(codec->decoder);

        unsigned int wait_sum = SystemTime::getTimeInMillis()-loadStart;
        if (wait_sum > MAX_DECODE_WAIT_MS && pauseWait == 0)
        {
            pauseWait = 1;
            loggerInfo("Tried to decode video over %d ms. Slowness... video:%s", wait_sum, getFilePath().c_str());

            Timer& timer = EnginePlayer::getInstance().getTimer();
            if (!timer.isPause())
            {
                pauseWait = 2;
                timer.setTimeInSeconds(timer.getTimeInSeconds() -(wait_sum/1000.0));
                timer.pause(true);
            }
        }

        if (video_frame != NULL)
        {
            if (video_frame->playms/1000.0 >= time)
            {
                //loggerDebug("Using frame! time:%.3f, frameTime:%.3f",time,video_frame->playms/1000.0);
                break;
            }
            else
            {
                //loggerDebug("Skipping frame! time:%.3f, frameTime:%.3f",time,video_frame->playms/1000.0);
                THEORAPLAY_freeVideo(video_frame);    //discard frame => will not be used
            }
        }

        if (!THEORAPLAY_isDecoding(codec->decoder))
        {
            return true;
        }        
    }

    if (pauseWait == 2)
    {
        Timer& timer = EnginePlayer::getInstance().getTimer();
        if (timer.isPause())
        {
            timer.pause(false);
        }
    }

    assert (video_frame != NULL);

    struct video_theora_frame_t *frame = (struct video_theora_frame_t*)malloc(sizeof(struct video_theora_frame_t));
    assert(frame);
    frame->audio = NULL;
    frame->video = video_frame;
    frame->fileCursor = frame->video->filecursor;
    //add_to_array(struct video_theora_frame_t, codec->frames, frame);


    /*if (frame->playms >= MAX_PLAY_TIME_MS)
    {
        loggerWarning("Maximum video length is %.2f seconds. Video truncated to maximum length. video:%s", MAX_PLAY_TIME_MS/1000.0f, getFilePath().c_str());
        return 0;
    }*/

    while ((frame->audio = THEORAPLAY_getAudio(codec->decoder)) != NULL)
    {
        THEORAPLAY_freeAudio(frame->audio);    
        frame->audio = NULL;
    }

    //TODO: HOW TO APPROPRIATELY HANDLE AUDIO???
    //while ((frame->audio = THEORAPLAY_getAudio(codec->decoder)) != NULL)
    //{
    //    queue_audio(frame->audio);
    //}

    if (codec->currentFrame != NULL && codec->currentFrame != frame)
    {
        freeFrameTheora(&codec->currentFrame);
    }
    codec->currentFrame = frame;

    assert(codec->currentFrame->video != NULL);
    //loggerTrace("Loaded frame '%s'! decodedTime:%.3f, time:%.3f", getFilePath().c_str(), frame->video->playms/1000.0, time);

    return true;
}

bool VideoFileTheora::loadVideoTheora()
{
    loadVideoTheoraFrame(0.0);

    int isInitialized = THEORAPLAY_isInitialized(codec->decoder);
    int hasAudio = THEORAPLAY_hasAudioStream(codec->decoder);
    int hasVideo = THEORAPLAY_hasVideoStream(codec->decoder);

    /*if (THEORAPLAY_isDecoding(codec->decoder))
    {
        THEORAPLAY_stopDecode(codec->decoder);
    }*/

    if (!isInitialized || !hasVideo)
    {
        loggerError("Could not initialize video! '%s'");
        return false;
    }

    //realloc_to_actual_size(struct video_theora_frame_t, codec->frames);

    if (useAudio && codec->currentFrame->audio)
    {
        loggerDebug("Audio stuff...");
        memset(&codec->audioSpec, '\0', sizeof (SDL_AudioSpec));
        codec->audioSpec.freq = codec->currentFrame->audio->freq;
        codec->audioSpec.format = AUDIO_S16SYS;
        codec->audioSpec.channels = codec->currentFrame->audio->channels;
        codec->audioSpec.samples = 2048;
        codec->audioSpec.callback = audio_callback;
        if (SDL_OpenAudio(&codec->audioSpec, NULL) < 0)
        {
            loggerError("Audio open failed! error:'%s'", SDL_GetError());
            useAudio = false;
        }
        else
        {
            while (codec->currentFrame->audio)
            {
                queue_audio(codec->currentFrame->audio);
                codec->currentFrame->audio = THEORAPLAY_getAudio(codec->decoder);
            }
        }
    }

    width = codec->currentFrame->video->width;
    height = codec->currentFrame->video->height;
    fps = codec->currentFrame->video->fps;

    if (!videoRefreshFrame()) {
        return false;
    }

    loggerInfo("Loaded video '%s'! dimensions:%dx%d, fps:%.2f, audio:%s, video:%s", getFilePath().c_str(), width, height, fps,hasAudio?"true":"false", hasVideo?"true":"false");

    return true;
}

void VideoFileTheora::freeFrameTheora(struct video_theora_frame_t **frame)
{
    assert(*frame);

    if ((*frame)->video)
    {
        THEORAPLAY_freeVideo((*frame)->video);
        (*frame)->video = NULL;
    }
    if ((*frame)->audio)
    {
        THEORAPLAY_freeAudio((*frame)->audio);
        (*frame)->audio = NULL;
    }

    free(*frame);
}

bool VideoFileTheora::freeVideoTheora()
{
    if (codec != NULL) {
        //freeVideoFramesTheora(video);
        if (codec->currentFrame)
        {
            freeFrameTheora(&codec->currentFrame);
        }

        THEORAPLAY_stopDecode(codec->decoder);

        free(codec);
        codec = NULL;
    }

    width = height = frame = 0;
    speed = 1.0;
    fps = 0.0;
    paused = false;
    texture = NULL;
    codecType = CODEC_NULL;
    codec = NULL;
    useAudio = false;
    loop = false;
    startTime = 0.0f;
    pauseTime = 0.0f;
    length = 0.0f;
    currentFrame = 0;
    state = VIDEO_STOPPED;

    return true;
}

void VideoFileTheora::setLength(float length)
{
    this->length = length;
}

bool VideoFileTheora::videoRefreshFrame()
{
    assert(codecType == CODEC_THEORA);
    const THEORAPLAY_VideoFrame *videoFrame = codec->currentFrame->video;
    assert(videoFrame);

    // TODO: support varying image sizes?
    /*const unsigned int w = videoFrame->width;
    const unsigned int h = videoFrame->height;*/

    if (texture != NULL) {
        texture->update(videoFrame->pixels);
    } else {
        texture = Texture::newInstance();
        texture->setFormat(TextureFormat::RGB);
        if (texture->create(width, height, videoFrame->pixels) == false) {
            loggerError("Could not load image, error creating texture. file:'%s' width:%d, height:%d, texture:0x%p",
                getFilePath().c_str(), width, height, texture);

            return false;        
        }
    }

    return true;
}

void VideoFileTheora::setSpeed(double speed)
{
    if (speed < 0.0)
    {
        loggerError("Invalid value given! video:'%s', invalidSpeedValue:%f", getFilePath().c_str(), speed);
        return;
    }

    this->speed = speed;
}

void VideoFileTheora::setFps(double fps)
{
    if (fps < 0.0)
    {
        loggerError("Invalid value given! video:'%s', invalidFpsValue:%f", getFilePath().c_str(), fps);
        return;
    }

    this->fps = fps;
}

void VideoFileTheora::setLoop(bool loop)
{
    this->loop = loop;
}

void VideoFileTheora::play()
{
    if (useAudio)
    {
        loggerError("Video audio not supported currently! getFilePath().c_str():'%s'", getFilePath().c_str());
        assert(useAudio == 0);
        return;
    }

    state = VIDEO_PLAYING;

    loggerDebug("Video '%s' started playing! startTime:%.2f, length:%.2f, loop:%s, state:%d", getFilePath().c_str(), startTime, length, loop ? "true":"false", state);
}

void VideoFileTheora::setStartTime(float startTime)
{
    if (startTime < 0.0)
    {
        loggerError("Invalid value given! video:'%s', invalidStartTimeValue:%f", getFilePath().c_str(), startTime);
        return;
    }

    this->startTime = startTime;
}

void VideoFileTheora::setTime(float time)
{
    if (state != VIDEO_PLAYING)
    {
        loggerWarning("You can't rewind video '%s' when it's not playing!", getFilePath().c_str());
        return;
    }

    startTime = EnginePlayer::getInstance().getTimer().getTimeInSeconds() - time;
}

void VideoFileTheora::stop()
{
    state = VIDEO_STOPPED;
    startTime = 0.0f;
}

void VideoFileTheora::pause()
{
    if (state == VIDEO_PLAYING)
    {
        state = VIDEO_PAUSED;
        pauseTime = EnginePlayer::getInstance().getTimer().getTimeInSeconds();
    }
    else
    {
        state = VIDEO_PLAYING;
        startTime += EnginePlayer::getInstance().getTimer().getTimeInSeconds()-pauseTime;
        pauseTime = 0.0f;
    }
}

void VideoFileTheora::draw()
{
    assert(codecType == CODEC_THEORA);

    float runningTime = EnginePlayer::getInstance().getTimer().getTimeInSeconds()-startTime;
    if (runningTime < 0.0)
    {
        if (state == VIDEO_PLAYING && codec->decoder)
        {
            THEORAPLAY_stopDecode(codec->decoder);
            codec->decoder = NULL;
        }

        return;
    }

    if (loop && length > 0.0)
    {
        runningTime = fmodf(runningTime, length);
    }

    unsigned int frame_i = currentFrame;
    
    if (state == VIDEO_PLAYING)
    {
        frame_i = (unsigned int)(fps*runningTime*speed);
    }
    //loggerDebug("Running! %.2f frame:%d (%d), state:%d", runningTime, frame_i, currentFrame, state);
    /*if (frame_i >= codec->framesSize)
    {
        return;
    }*/

    if (currentFrame == frame_i)
    {
        return; //assume that frame has been buffered
    }
    currentFrame = frame_i;
    loadVideoTheoraFrame(runningTime*speed);

    //codec->currentFrame->video = codec->frames[currentFrame]->video;

    if (codec->currentFrame->video)
    {
        //loggerTrace("FRAME PLAY: %u, %u, %.2f, fps:%.2f, speed:%.2f", currentFrame, frame_i, runningTime, fps, speed);
        videoRefreshFrame();
    }

    if (codec->currentFrame->audio)
    {
        while ((codec->currentFrame->audio = THEORAPLAY_getAudio(codec->decoder)) != NULL)
            queue_audio(codec->currentFrame->audio);
    }
}
