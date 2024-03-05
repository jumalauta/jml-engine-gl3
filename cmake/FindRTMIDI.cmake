set(RTMIDI_VERSION "4.0.0")

set(prefix "${PROJECT_SOURCE_DIR}/external/lib/rtmidi-${RTMIDI_VERSION}")
if (prefix)
    if (NOT EXISTS ${prefix})
        message(FATAL_ERROR "RtMidi directory not found: " ${prefix})
    endif()
endif()

set(RTMIDI_INCLUDE_DIRS "${prefix}")
set(RTMIDI_SOURCE_FILES
    ${prefix}/RtMidi.cpp
    ${prefix}/RtMidi.h
)

if (MSYS OR MINGW)
    set_source_files_properties(${RTMIDI_SOURCE_FILES} PROPERTIES COMPILE_DEFINITIONS "__WINDOWS_MM__")
    set(RTMIDI_LIBRARIES "-lwinmm")
elseif (APPLE)
    set_source_files_properties(${RTMIDI_SOURCE_FILES} PROPERTIES COMPILE_DEFINITIONS "__MACOSX_CORE__")
    set(RTMIDI_LIBRARIES "-framework CoreMIDI -framework CoreAudio -framework CoreFoundation")
else()
    set_source_files_properties(${RTMIDI_SOURCE_FILES} PROPERTIES COMPILE_DEFINITIONS "__UNIX_JACK__")
    set(RTMIDI_LIBRARIES "-ljack")
endif()


