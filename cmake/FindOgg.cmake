set(OGG_VERSION "1.3.1")
set(prefix "${PROJECT_SOURCE_DIR}/external/lib/libogg-${OGG_VERSION}") 
set(Ogg_INCLUDE_DIRS "${prefix}/include")

if (MSYS OR MINGW)
    if (prefix)
        if (NOT EXISTS ${prefix})
            message(FATAL_ERROR "OGG directory not found: " ${prefix})
        endif()
    endif()

    set(exec_prefix "${prefix}")
    set(libdir "${exec_prefix}/lib")
    set(Ogg_LIBDIR "${exec_prefix}/lib")
    set(Ogg_LIBRARIES "-logg")
    string(STRIP "${Ogg_LIBRARIES}" Ogg_LIBRARIES)
elseif (APPLE)
    set(Ogg_LIBRARIES "-framework Ogg")
else()
    PKG_SEARCH_MODULE(Ogg REQUIRED ogg)
    string(STRIP "${Ogg_LIBRARIES}" Ogg_LIBRARIES)
endif()