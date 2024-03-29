if (MSYS OR MINGW)
    set(prefix "${PROJECT_SOURCE_DIR}/external/lib/mingw/SDL2_mixer-2.0.4/x86_64-w64-mingw32") 
    if (prefix)
        if (NOT EXISTS ${prefix})
            message(FATAL_ERROR "SDL2 Mixer directory not found: " ${prefix})
        endif()
    endif()

    set(exec_prefix "${prefix}")
    set(libdir "${exec_prefix}/lib")
    #set(SDL2_PREFIX "/usr/local/cross-tools/i686-w64-mingw32")
    #set(SDL2_EXEC_PREFIX "/usr/local/cross-tools/i686-w64-mingw32")
    set(SDL2_MIXER_LIBDIR "${exec_prefix}/lib")
    set(SDL2_MIXER_INCLUDE_DIRS "${prefix}/include/SDL2")
    set(SDL2_MIXER_LIBRARIES "-lSDL2_mixer")
    string(STRIP "${SDL2_MIXER_LIBRARIES}" SDL2_MIXER_LIBRARIES)
elseif (APPLE)
    FIND_PATH(SDL2_MIXER_INCLUDE_DIRS SDL2_mixer/SDL_mixer.h)
    if (NOT EXISTS ${SDL2_MIXER_INCLUDE_DIRS})
        message(FATAL_ERROR "SDL2_mixer framework not found")
    endif()

    set(SDL2_MIXER_LIBRARIES "-framework SDL2_mixer")
else()
    PKG_SEARCH_MODULE(SDL2_MIXER REQUIRED SDL2_mixer>=2.0.0)

    string(STRIP "${SDL2_MIXER_LIBRARIES}" SDL2_MIXER_LIBRARIES)
endif()
