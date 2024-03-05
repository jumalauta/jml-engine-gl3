#include "MidiControllerRtMidi.h"

#include "logger/logger.h"

// URL: https://www.music.mcgill.ca/~gary/rtmidi/classRtMidiIn.html

static void rtMidiErrorCallback(RtMidiError::Type type, const std::string &errorText, void *userData) {
    loggerWarning("MIDI error occurred: '%s'", errorText.c_str());
}

static void rtMidiCallback( double deltaTime, std::vector< unsigned char > *message, void *userData )
{
    MidiController* midiController = reinterpret_cast<MidiController*>(userData);
    if (midiController == NULL) {
        loggerFatal("Configuration issue, midi controller should be defined");
        return;
    }

    midiController->addEvent(deltaTime, message);
}

MidiController* MidiController::newInstance() {
    MidiController *midiController = new MidiControllerRtMidi();
    if (midiController == NULL) {
        loggerFatal("Could not allocate memory for midi controller");
        return NULL;
    }

    return midiController;
}

MidiControllerRtMidi::MidiControllerRtMidi() {
    midiin = NULL;
}

MidiControllerRtMidi::~MidiControllerRtMidi() {
    exit();
}

bool MidiControllerRtMidi::init() {
    try {
        if (midiin == NULL) {
            midiin = new RtMidiIn();

            if (midiin == NULL) {
                loggerFatal("No memory to initialize RtMidiIn");
                return false;
            }
        }

        std::vector<RtMidi::Api> apis;
        RtMidi::getCompiledApi(apis);
        if (apis.empty()) {
            loggerError("Trying to init MIDI but no RtMidi API implementations available");
            return false;
        }

        bool hasProperApiImplementation = false;
        for(RtMidi::Api api : apis) {
            loggerTrace("RtMidi API available: '%s'", RtMidi::getApiName(api).c_str());
            if (api != RtMidi::Api::UNSPECIFIED && api != RtMidi::Api::RTMIDI_DUMMY) {
                hasProperApiImplementation = true;
            }
        }

        if (!hasProperApiImplementation) {
            loggerError("No proper RtMidi API implementation found... can't support MIDI");
            return false;
        }

        return true;

    } catch ( RtMidiError &error ) {
        loggerWarning("MIDI exception occurred: '%s'", error.what());
    }

    return false;
}

std::vector<MidiPort> MidiControllerRtMidi::listAvailablePorts() {

    std::vector<MidiPort> portList;

    try {
        if (midiin == NULL) {
            loggerFatal("Not initialized, must not call");
        }

        unsigned int nPorts = midiin->getPortCount();
        if ( nPorts == 0 ) {
            loggerTrace("No MIDI ports available!");
            return portList;
        }

        for ( unsigned int i=0; i<nPorts; i++ ) {
            std::string portName = midiin->getPortName(i);
            loggerTrace("Input port #%u: %s", i, portName.c_str());

            portList.push_back(MidiPort(i, portName));
        }
    } catch ( RtMidiError &error ) {
        portList.clear();
        loggerWarning("MIDI exception occurred: '%s'", error.what());
    }

    return portList;
}

bool MidiControllerRtMidi::connect(unsigned int portNumber = 0) {
    try {
        if (midiin == NULL) {
            loggerFatal("Not initialized, must not call");
            return false;
        }

        loggerDebug("Attempting to connect MIDI port %u", portNumber);

        port.number = portNumber;
        port.name = midiin->getPortName(port.number);

        midiin->openPort(port.number);

        /*if (midiin->isPortOpen() == false) {
            //this should not occur as RtMidiIn::openPort should throw an exception if opening fails
            loggerError("MIDI port not open!");
            return false;
        }*/

        port.status = MidiPortStatus::CONNECTED;

        loggerInfo("MIDI port open #%u: %s", port.number, port.name.c_str());

        // Don't ignore sysex, timing, or active sensing messages.
        bool midiSysex = false;
        bool midiTime = false;
        bool midiSense = false;
        midiin->ignoreTypes( midiSysex, midiTime, midiSense );

        // Set our callback function.  This should be done immediately after
        // opening the port to avoid having incoming messages written to the
        // queue.
        midiin->setCallback( &rtMidiCallback, this );
        midiin->setErrorCallback(&rtMidiErrorCallback, this);

        return true;

    } catch ( RtMidiError &error ) {
        loggerWarning("MIDI exception occurred: '%s'", error.what());
    }

    return false;
}

bool MidiControllerRtMidi::disconnect() {
    try {
        if (port.status == MidiPortStatus::CONNECTED) {
            midiin->closePort();
            port.status = MidiPortStatus::DISCONNECTED;

            loggerDebug("MIDI port closed #%u: %s", port.number, port.name.c_str());
        }

        return true;

    } catch ( RtMidiError &error ) {
        loggerWarning("MIDI exception occurred: '%s'", error.what());
    }

    return false;
}

bool MidiControllerRtMidi::exit() {
    if (midiin != NULL) {
        delete midiin;
        midiin = NULL;
    }

    return true;    
}
