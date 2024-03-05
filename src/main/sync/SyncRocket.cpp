#include "SyncRocket.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "tinyxml2.h"

#include "rocket-0.10.2/lib/sync.h"
// rocket private APIs
#include "rocket-0.10.2/lib/track.h"
#include "rocket-0.10.2/lib/device.h"

#include "logger/logger.h"
#include "time/Timer.h"
#include "io/File.h"
#include "Settings.h"

//as-is copy of rocket private function
static int find_track(struct sync_device *d, const char *name)
{
    int i;
    for (i = 0; i < (int)d->num_tracks; ++i)
        if (!strcmp(name, d->tracks[i]->name))
            return i;
    return -1; /* not found */
}
static int create_track(struct sync_device *d, const char *name)
{
    void *tmp;
    struct sync_track *t;
    assert(find_track(d, name) < 0);

    t = (struct sync_track *)malloc(sizeof(*t));
    if (!t)
        return -1;

    t->name = strdup(name);
    t->keys = NULL;
    t->num_keys = 0;

    tmp = (void*)realloc(d->tracks, sizeof(d->tracks[0]) * (d->num_tracks + 1));
    if (!tmp) {
        free(t);
        return -1;
    }

    d->tracks = (struct sync_track **)tmp;
    d->tracks[d->num_tracks++] = t;

    return (int)d->num_tracks - 1;
}

static const char *path_encode(const char *path) {
    // Do not convert paths to comply with POSIX encoding rules as how rockets wants it to be done
    return path;
}

bool SyncRocket::parseRocketXml(std::string filename)
{
    usingXml = false;
    if (!rocket) {
        // Code bug as initialization should be expected
        loggerFatal("GNU Rocket not initialized, will not parse");
        return false;
    }

    File rocketXmlFile = File(filename);
    if (!rocketXmlFile.isFile()) {
        loggerTrace("No rocket file exists: '%s'", rocketXmlFile.getFilePath().c_str());
        return false;
    }

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError ret = doc.LoadFile(rocketXmlFile.getFilePath().c_str());
    if (ret != tinyxml2::XML_SUCCESS) {
        loggerError("Could not load file '%s', error:%d", ret);
        return false;
    }

    tinyxml2::XMLNode* sync = doc.FirstChildElement("sync");
    if (!sync) {
        loggerError("Could not parse xml, missing sync element");
        return false;
    }

    tinyxml2::XMLElement* tracks = sync->FirstChildElement("tracks");
    if (!tracks) {
        loggerError("Could not parse xml, missing tracks element");
        return false;
    }

    int track_i = 0;
    for(tinyxml2::XMLElement* track = tracks->FirstChildElement("track"); track; track = track->NextSiblingElement("track")) {
        const char* trackName = track->Attribute("name");
        if (rocketEditor) {
            getSyncTrack(trackName);
            continue;
        }

        int idx = find_track(rocket, trackName);
        if (idx >= 0 || !track->FirstChildElement("key")) {
            continue;
        }

        idx = create_track(rocket, trackName);
        struct sync_track *t = rocket->tracks[idx];

        t->num_keys = 0;
        for(tinyxml2::XMLElement* key = track->FirstChildElement("key"); key; key = key->NextSiblingElement("key")) {
            t->num_keys++;
        }

        loggerTrace("Rocket sync added! track:'%s' (0x%p), keys:%d", trackName, t, t->num_keys);

        t->keys = (struct track_key*)malloc(sizeof(struct track_key) * t->num_keys);
        if (!t->keys) {
            loggerFatal("Could not allocate memory. track:'%s', keys:%d", trackName, t->num_keys);
            return false;
        }

        int i = 0;
        for(tinyxml2::XMLElement* key = track->FirstChildElement("key"); key; key = key->NextSiblingElement("key")) {
            struct track_key *k = t->keys + i;
            k->value = key->FloatAttribute("value");
            k->row = key->IntAttribute("row");
            k->type = (key_type)key->IntAttribute("interpolation");
            i++;
        }

        track_i++;
    }

    usingXml = true;
    loggerInfo("Loaded GNU Rocket file. file:'%s', tracks:%d", rocketXmlFile.getFilePath().c_str(), track_i);

    return true;
}

Sync& Sync::getInstance() {
    static SyncRocket sync = SyncRocket();
    return sync;
}

void SyncRocket::pauseCallBack(void *syncRocketPtr, int isPause) {
    SyncRocket *syncRocket = (SyncRocket*)syncRocketPtr;
    syncRocket->getTimer()->pause(isPause ? true : false);
}

void SyncRocket::setRowCallBack(void *syncRocketPtr, int row) {
    SyncRocket *syncRocket = (SyncRocket*)syncRocketPtr;
    syncRocket->getTimer()->setTimeInBeats(row / syncRocket->getRowsPerBeat());
}

int SyncRocket::isPlayingCallBack(void *syncRocketPtr) {
    SyncRocket *syncRocket = (SyncRocket*)syncRocketPtr;
    return (syncRocket->getTimer()->isPause() == true ? 0 : 1);
}

SyncRocket::SyncRocket() {
    rocket = NULL;
    setRowsPerBeat(8.0);
    currentRow = 0.0;
    rocketEditor = false;
    usingXml = false;
}

SyncRocket::~SyncRocket() {

}

double SyncRocket::getRowsPerBeat() {
    return rowsPerBeat;
}

void SyncRocket::setRowsPerBeat(double rowsPerBeat) {
    this->rowsPerBeat = rowsPerBeat;
}

void SyncRocket::update() {
    currentRow = getTimer()->getTimeInBeats() * rowsPerBeat;

    if (isRocketEditor()) {
        static struct sync_cb sync_rocket_callback = {
            pauseCallBack,
            setRowCallBack,
            isPlayingCallBack
        };

        if (sync_update(rocket, static_cast<int>(floor(currentRow)), &sync_rocket_callback, this)) {
            loggerWarning("Lost socket connection to GNU Rocket");
            connect();
        }
    }
}

const struct sync_track* SyncRocket::getSyncTrack(const char *trackName) {    
    return sync_get_track(rocket, trackName);
}

double SyncRocket::getSyncTrackCurrentValue(const void *ptr) {
    const struct sync_track *track = (const struct sync_track*)ptr;
    return sync_get_val(track, currentRow);
}

bool SyncRocket::containsVariable(const char *variableName) {
    if (!rocket) {
        return false;
    }

    if (find_track(rocket, variableName) == -1) {
        return false;
    }

    return true;
}

double SyncRocket::getVariableCurrentValue(const char *variableName) {
    return getSyncTrackCurrentValue(getSyncTrack(variableName));
}

bool SyncRocket::init(Timer *timer) {
    return init(timer, true);
}

bool SyncRocket::init(Timer *timer, bool allowConnection) {
    if (timer == NULL) {
        loggerFatal("Timer not defined!");
        return false;
    }

    setTimer(timer);

    File syncDirectory = File("sync");
    if (!syncDirectory.isDirectory()) {
        if (Settings::gui.tool) {
            loggerInfo("Sync disabled. Sync directory not found: '%s'", syncDirectory.getFilePath().c_str());
        }

        return false;
    }
    syncPrefixPath = std::string(syncDirectory.getFilePath() + "/sync");

    if (!rocket) {
        rocket = sync_create_device(syncPrefixPath.c_str());
        if (!rocket) {
            loggerError("Failed to initialize GNU Rocket sync device.");
            return false;
        }

        sync_set_path_encode_cb(rocket, (const char *(*)(const char *path))path_encode);
    }
    
    if (Settings::gui.tool && allowConnection)
    {
        connect();
        update();
    }

    if (!parseRocketXml(syncDirectory.getFilePath() + std::string("/") + Settings::demo.rocketXmlFile)) {
        // Fall-back / legacy support for the track files.
        // Main problem with *.track files are that
        // 1) GNU Rocket does not care about endianess - i.e. big endian vs. little endian cross-dev does not work without conversion
        // 2) it's harder to do quick hand edits to track files vs. one big xml
        // 3) xml files are extendable in case needed
        std::vector<File> syncFiles = syncDirectory.list();
        int track_i = 0;
        for(File file : syncFiles) {
            if (file.getFileExtension() == "track") {
                std::string variableName = file.getFilePath().substr(syncPrefixPath.length() + 1);
                std::size_t fileExtensionIndex = variableName.find_last_of(".");
                if (fileExtensionIndex != std::string::npos) {
                    variableName = variableName.substr(0, fileExtensionIndex);
                }

                // Pre create sync track data, so that it can be auto-bind into shaders
                getSyncTrack(variableName.c_str());
                track_i++;
            }
        }

        loggerInfo("Loaded GNU Rocket tracks. directory:'%s', tracks:%d", syncDirectory.getFilePath().c_str(), track_i);
    }


    return true;
}

void SyncRocket::connect() {
    if (sync_tcp_connect(rocket, Settings::gui.gnuRocketHost.c_str(), Settings::gui.gnuRocketPort)) {
        loggerInfo("Could not connect to GNU Rocket. server:'%s:%d'", Settings::gui.gnuRocketHost.c_str(), Settings::gui.gnuRocketPort);
        setRocketEditor(false);
    } else {
        loggerInfo("Connected to GNU Rocket. server:'%s:%d'", Settings::gui.gnuRocketHost.c_str(), Settings::gui.gnuRocketPort);
        setRocketEditor(true);
    }
}

void SyncRocket::setRocketEditor(bool rocketEditor) {
    this->rocketEditor = rocketEditor;
}

bool SyncRocket::isRocketEditor() {
    return rocketEditor;
}

void SyncRocket::save() {
    if (!usingXml) {
        if (sync_save_tracks(rocket) == 0) {
            loggerInfo("Saved GNU Rocket tracks in '%s'", syncPrefixPath.c_str());
        } else {
            loggerError("Could not save one or more GNU Rocket tracks in '%s'!", syncPrefixPath.c_str());
        }
    } else {
        loggerTrace("Not saving *.track files as rocket XML is in use");
    }
}

bool SyncRocket::exit() {
    if (rocket) {
        if (isRocketEditor()) {
            save();
            setRocketEditor(false);
        }

        sync_destroy_device(rocket);
        rocket = NULL;
    }

    return true;
}
