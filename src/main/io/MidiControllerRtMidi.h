#ifndef ENGINE_IO_MIDICONTROLLERRTMIDI_H_
#define ENGINE_IO_MIDICONTROLLERRTMIDI_H_

#include "MidiController.h"

#include "RtMidi.h"

class MidiControllerRtMidi : public MidiController {
public:
    MidiControllerRtMidi();
    ~MidiControllerRtMidi();
    bool init();
    bool exit();
    std::vector<MidiPort> listAvailablePorts();
    bool connect(unsigned int portNumber);
    bool disconnect();
private:
    RtMidiIn *midiin;
};

#endif /* ENGINE_IO_MIDICONTROLLERRTMIDI_H_ */
