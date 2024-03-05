#ifndef ENGINE_IO_MIDICONTROLLER_H_
#define ENGINE_IO_MIDICONTROLLER_H_

#include <string>
#include <vector>

// URL: https://www.midi.org/specifications-old/item/table-1-summary-of-midi-message
// URL: https://www.inspiredacoustics.com/en/MIDI_note_numbers_and_center_frequencies

enum class MidiEventType {
    UNKNOWN = -1,

    // events with channels
    NOTE_OFF = 128,
    NOTE_ON = 144,
    POLYPHONIC_AFTERTOUCH = 160,
    CONTROL_CHANGE = 176,
    PROGRAM_CHANGE = 192,
    CHANNEL_AFTERTOUCH = 208,
    PITCH_BEND_CHANGE = 224,

    // events without channels
    SYSTEM_EXCLUSIVE = 240,
    SYSTEM_EXCLUSIVE_EOX = 247,
    TIME_CODE_QTR_FRAME = 241,
    SONG_POSITION_POINTER = 242,
    SONG_SELECT = 243,
    // undefined 244
    // undefined 245
    TUNE_REQUEST = 246,
    TIMING_CLOCK = 248,
    // undefined 249
    START = 250,
    CONTINUE = 251,
    STOP = 252,
    // undefined 253
    ACTIVE_SENSING = 254,
    SYSTEM_RESET = 255
};

enum class MidiPortStatus {
    DISCONNECTED=0,
    CONNECTED=1
};

struct MidiPort {
    MidiPort();
    MidiPort(unsigned int number, std::string name);
    unsigned int number;
    std::string name;
    MidiPortStatus status;
};

struct MidiEvent {
    MidiEvent();
    std::string serialize();

    double demoTime;
    double deltaTime;
    MidiEventType type;
    int channel;
    std::vector<unsigned char> message;
};

class MidiController {
public:
    static MidiController* newInstance();
    MidiController();
    virtual ~MidiController() {};
    virtual bool init() = 0;
    virtual bool exit() = 0;
    virtual std::vector<MidiPort> listAvailablePorts() = 0;
    virtual bool connect(unsigned int portNumber) = 0;
    virtual bool disconnect() = 0;
    virtual void addEvent(const MidiEvent& event);
    virtual void addEvent( double deltaTime, std::vector< unsigned char > *message );
    virtual std::vector<MidiEvent> pollEvents();
protected:
    MidiPort port;
    std::vector<MidiEvent> events;
    size_t eventPollIterator;
};

#endif /* ENGINE_IO_MIDICONTROLLER_H_ */
