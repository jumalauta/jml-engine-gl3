#include "MidiController.h"

#include "logger/logger.h"
#include "time/Timer.h"
#include "EnginePlayer.h"
#include "Settings.h"

#include <sstream>
#include <limits>

MidiPort::MidiPort() {
    number = 0;
    name = "";
    status = MidiPortStatus::DISCONNECTED;
}

MidiPort::MidiPort(unsigned int number, std::string name) {
    MidiPort();

    this->number = number;
    this->name = name;
}

static void to_json(nlohmann::json& j, const MidiEventType& type) {
    JSON_MARSHAL_ENUM(MidiEventType, type);
}

// Some issues occurred with JSON parsing when there are too many decimals in use
// XXX. maybe an issue in Duktape? need to repro the case
static double truncateDoubleMaxPrecision(double originalValue) {
    std::stringstream sstream;
    sstream.precision(std::numeric_limits<double>::digits10);
    sstream << originalValue;

    double newValue = 0.0;
    sstream >> newValue;

    return newValue;
}

static void to_json(nlohmann::json& j, const MidiEvent& event) {
    j = nlohmann::json::object();
    j["demoTime"] = event.demoTime;
    j["deltaTime"] = truncateDoubleMaxPrecision(event.deltaTime);
    j["type"] = event.type;
    j["channel"] = event.channel;
    j["message"] = event.message;
}

static void from_json(const nlohmann::json& j, MidiEvent& event) {
    JSON_UNMARSHAL_VAR(event, double, demoTime);
    JSON_UNMARSHAL_VAR(event, double, deltaTime);
    JSON_UNMARSHAL_ENUM(event, MidiEventType, type);
    JSON_UNMARSHAL_VAR(event, int, channel);
    JSON_UNMARSHAL_VAR(event, std::vector<unsigned char>, message);
}

MidiEvent::MidiEvent() {
    demoTime = 0.0;
    deltaTime = 0.0;
    type = MidiEventType::UNKNOWN;
    channel = -1;
}

std::string MidiEvent::serialize() {
    nlohmann::json jsonObject = *this;
    return jsonObject.dump();
}

void MidiController::addEvent( double deltaTime, std::vector< unsigned char > *message )
{
    if (message->size() > 0) {
        EnginePlayer& enginePlayer = EnginePlayer::getInstance();
        Timer& timer = enginePlayer.getTimer();

        MidiEvent event;
        event.demoTime = timer.getTimeInSeconds();
        event.deltaTime = deltaTime;
        event.message = *message;

        unsigned int statusByte = static_cast<unsigned int>(event.message[0]);
        unsigned int eventType = statusByte;
        if (eventType < static_cast<unsigned int>(MidiEventType::SYSTEM_EXCLUSIVE)) {
            eventType = eventType >> 4 << 4; // disregard 4 least significant bits
            int channel = 1 + statusByte - eventType;
            event.channel = channel;
        }

        MidiEventType type;
        switch (eventType) {
            case static_cast<unsigned int>(MidiEventType::NOTE_OFF):
                type = MidiEventType::NOTE_OFF;
                break;
            case static_cast<unsigned int>(MidiEventType::NOTE_ON):
                type = MidiEventType::NOTE_ON;
                break;
            case static_cast<unsigned int>(MidiEventType::POLYPHONIC_AFTERTOUCH):
                type = MidiEventType::POLYPHONIC_AFTERTOUCH;
                break;
            case static_cast<unsigned int>(MidiEventType::CONTROL_CHANGE):
                type = MidiEventType::CONTROL_CHANGE;
                break;
            case static_cast<unsigned int>(MidiEventType::PROGRAM_CHANGE):
                type = MidiEventType::PROGRAM_CHANGE;
                break;
            case static_cast<unsigned int>(MidiEventType::CHANNEL_AFTERTOUCH):
                type = MidiEventType::CHANNEL_AFTERTOUCH;
                break;
            case static_cast<unsigned int>(MidiEventType::PITCH_BEND_CHANGE):
                type = MidiEventType::PITCH_BEND_CHANGE;
                break;
            case static_cast<unsigned int>(MidiEventType::SYSTEM_EXCLUSIVE):
                type = MidiEventType::SYSTEM_EXCLUSIVE;
                break;
            case static_cast<unsigned int>(MidiEventType::SYSTEM_EXCLUSIVE_EOX):
                type = MidiEventType::SYSTEM_EXCLUSIVE_EOX;
                break;
            case static_cast<unsigned int>(MidiEventType::TIME_CODE_QTR_FRAME):
                type = MidiEventType::TIME_CODE_QTR_FRAME;
                break;
            case static_cast<unsigned int>(MidiEventType::SONG_POSITION_POINTER):
                type = MidiEventType::SONG_POSITION_POINTER;
                break;
            case static_cast<unsigned int>(MidiEventType::SONG_SELECT):
                type = MidiEventType::SONG_SELECT;
                break;
            case static_cast<unsigned int>(MidiEventType::TUNE_REQUEST):
                type = MidiEventType::TUNE_REQUEST;
                break;
            case static_cast<unsigned int>(MidiEventType::TIMING_CLOCK):
                type = MidiEventType::TIMING_CLOCK;
                break;
            case static_cast<unsigned int>(MidiEventType::START):
                type = MidiEventType::START;
                break;
            case static_cast<unsigned int>(MidiEventType::CONTINUE):
                type = MidiEventType::CONTINUE;
                break;
            case static_cast<unsigned int>(MidiEventType::STOP):
                type = MidiEventType::STOP;
                break;
            case static_cast<unsigned int>(MidiEventType::ACTIVE_SENSING):
                type = MidiEventType::ACTIVE_SENSING;
                break;
            case static_cast<unsigned int>(MidiEventType::SYSTEM_RESET):
                type = MidiEventType::SYSTEM_RESET;
                break;
            default:
                type = MidiEventType::UNKNOWN;
                break;
        }

        event.type = type;

        loggerDebug("MIDI Event: %s", event.serialize().c_str());
        /*if (timer.isPause()) {
            loggerDebug("MIDI event omitted, demo is in pause");
            return;
        }*/

        addEvent(event);
    } else {
        loggerTrace("Empty MIDI event received");
    }
}

MidiController::MidiController() {
    eventPollIterator = 0;
}

void MidiController::addEvent(const MidiEvent& event) {
    events.push_back(event);
}

std::vector<MidiEvent> MidiController::pollEvents() {
    std::vector<MidiEvent> polledEvents;

    EnginePlayer& enginePlayer = EnginePlayer::getInstance();
    Timer& timer = enginePlayer.getTimer();

    for(; eventPollIterator < events.size(); eventPollIterator++) {
        MidiEvent event = events[eventPollIterator];

        if (event.demoTime > timer.getTimeInSeconds()) {
            // assuming that events are ordered correctly, we should stop polling
            break;
        }

        polledEvents.push_back(event);
    }

    return polledEvents;
}
