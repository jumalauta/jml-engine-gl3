#include "io/MidiManager.h"

#include "logger/logger.h"

MidiManager& MidiManager::getInstance() {
    static MidiManager midiManager;
    return midiManager;
}

MidiManager::MidiManager() {
}

MidiManager::~MidiManager() {
    exit();
}

std::vector<MidiController*> MidiManager::getMidiControllers() {
    return midiControllers;
}

MidiController* MidiManager::addMidiController() {
    MidiController* midiController = MidiController::newInstance();
    if (midiController == NULL) {
        loggerFatal("MIDI controller could not be created. Out of memory?");
        return NULL;
    }

    midiControllers.push_back(midiController);

    return midiController;
}

bool MidiManager::exit() {
    if (!midiControllers.empty()) {
        loggerDebug("Cleaning up MIDI %d controllers", midiControllers.size());
        for(MidiController* midiController : midiControllers) {
            midiController->disconnect();
            midiController->exit();
            delete midiController;
        }

        midiControllers.clear();
    }

    return true;
}
