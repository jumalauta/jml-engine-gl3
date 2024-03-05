#ifndef ENGINE_SYNC_SYNCROCKET_H_
#define ENGINE_SYNC_SYNCROCKET_H_

#include "sync/Sync.h"

#include <string>

struct sync_device;
struct sync_track;

class SyncRocket : public Sync {
public:
    SyncRocket();
    ~SyncRocket();
    bool containsVariable(const char *variableName);
    double getVariableCurrentValue(const char *variableName);
    void update();
    bool exit();
    bool init(Timer *timer);
    bool init(Timer *timer, bool allowConnection);

    double getRowsPerBeat();
    void setRowsPerBeat(double rowsPerBeat);

    void setRocketEditor(bool rocketEditor);
    bool isRocketEditor();
    const struct sync_track* getSyncTrack(const char *trackName);
    double getSyncTrackCurrentValue(const void *ptr);
private:
    struct sync_device *rocket;
    double rowsPerBeat;
    double currentRow;
    bool rocketEditor;
    bool usingXml;
    std::string syncPrefixPath;

    static void pauseCallBack(void *syncRocketPtr, int isPause);
    static void setRowCallBack(void *syncRocketPtr, int row);
    static int isPlayingCallBack(void *syncRocketPtr);

    bool parseRocketXml(std::string filename);
    void connect();

    void save();
};

#endif /*ENGINE_SYNC_SYNCROCKET_H_*/
