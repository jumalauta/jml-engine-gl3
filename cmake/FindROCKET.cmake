set(relative_path "${EXT_LIB_ROOT}/rocket-0.10.2")
set(prefix "${PROJECT_SOURCE_DIR}/${relative_path}") 
set(lib_path "${relative_path}/lib")

if (prefix)
    if (NOT EXISTS ${prefix})
        message(FATAL_ERROR "ROCKET directory not found: " ${prefix})
    endif()
endif()

set(ROCKET_INCLUDE_DIRS "${lib_path}")
set(ROCKET_SOURCE_FILES
    ./${lib_path}/device.c
    ./${lib_path}/track.c
)

if (CMAKE_COMPILER_IS_GNUCC)
    # FIXME: Rocket version is based currently on: https://github.com/mrautio/rocket/tree/feature/allow-non-portable-characters-in-file-paths
    # ...Hoping for merge to actual master
    set(FLAGS "-Wno-sign-compare")
    set_source_files_properties(${ROCKET_SOURCE_FILES} PROPERTIES COMPILE_FLAGS "${FLAGS}")
endif()

OPTION(ROCKET_EDITOR_SUPPORT "build GNU Rocket with interactive editor support" TRUE)

if (ROCKET_EDITOR_SUPPORT)
    if (MINGW)
        set(ROCKET_LIBRARIES "-lWs2_32")
    endif()
else()
    add_definitions(-DSYNC_PLAYER)
endif()
