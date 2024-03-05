#ifndef ENGINE_IO_MIDIMANAGER_H_
#define ENGINE_IO_MIDIMANAGER_H_

#include <vector>
#include "io/MidiController.h"

class MidiManager {
public:
    static MidiManager& getInstance();
    ~MidiManager();

    MidiController* addMidiController();
    std::vector<MidiController*> getMidiControllers();

    bool exit();
private:
    MidiManager();
    std::vector<MidiController*> midiControllers;
};

#endif /* ENGINE_IO_MIDIMANAGER_H_ */
