set(EASY_PROGRAM_VERSION_MAJOR 1)
set(EASY_PROGRAM_VERSION_MINOR 3)
set(EASY_PROGRAM_VERSION_PATCH 0)

set(EasyProfiler_VERSION "${EASY_PROGRAM_VERSION_MAJOR}.${EASY_PROGRAM_VERSION_MINOR}.${EASY_PROGRAM_VERSION_PATCH}")
set(relative_path "${EXT_LIB_ROOT}/easy_profiler-${EasyProfiler_VERSION}/easy_profiler_core")
set(prefix "${PROJECT_SOURCE_DIR}/${relative_path}") 
set(lib_path "${relative_path}")

if (prefix)
    if (NOT EXISTS ${prefix})
        message(FATAL_ERROR "EasyProfiler directory not found: " ${prefix})
    endif()
endif()

#####################################################################
# Checking c++11 thread_local support
if (NOT WIN32)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.8")
            set(NO_CXX11_THREAD_LOCAL_SUPPORT TRUE)
        endif ()
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "3.3")
            set(NO_CXX11_THREAD_LOCAL_SUPPORT TRUE)
        endif ()
    endif ()
    set(NO_CXX11_THREAD_LOCAL_SUPPORT TRUE)
    # TODO: Check thread_local keyword support for other compilers for Unix
    
    if (NO_CXX11_THREAD_LOCAL_SUPPORT)
        message(WARNING "  Your compiler does not support C++11 thread_local feature.")
        message(WARNING "  Without C++11 thread_local feature you may face to possible memory leak or application crash if using implicit thread registration and using EASY_THREAD instead of EASY_THREAD_SCOPE.")
    endif ()
endif ()
#####################################################################


#set(CMAKE_PREFIX_PATH "${prefix}/lib/cmake/easy_profiler")
#find_package(easy_profiler REQUIRED)


add_definitions(
    -DEASY_PROFILER_VERSION_MAJOR=${EASY_PROGRAM_VERSION_MAJOR}
    -DEASY_PROFILER_VERSION_MINOR=${EASY_PROGRAM_VERSION_MINOR}
    -DEASY_PROFILER_VERSION_PATCH=${EASY_PROGRAM_VERSION_PATCH}
)

###########################################################
# EasyProfiler options:
set(EASY_OPTION_IMPLICIT_THREAD_REGISTER_TEXT "Enable new threads registration when collecting context switch events")
set(EASY_DEFAULT_PORT                  28077  CACHE STRING "Default listening port")
set(EASY_OPTION_LISTEN                 OFF    CACHE BOOL   "Enable automatic startListen on startup")
set(EASY_OPTION_PROFILE_SELF           OFF    CACHE BOOL   "Enable self profiling (measure time for internal storage expand)")
set(EASY_OPTION_PROFILE_SELF_BLOCKS_ON OFF    CACHE BOOL   "Storage expand default status (profiler::ON or profiler::OFF)")
set(EASY_OPTION_LOG                    OFF    CACHE BOOL   "Print errors to stderr")
set(EASY_OPTION_PREDEFINED_COLORS      ON     CACHE BOOL   "Use predefined set of colors (see profiler_colors.h). If you want to use your own colors palette you can turn this option OFF")
set(BUILD_SHARED_LIBS                  ON     CACHE BOOL   "Build easy_profiler as shared library.")
if (WIN32)
    set(EASY_OPTION_IMPLICIT_THREAD_REGISTRATION ON CACHE BOOL ${EASY_OPTION_IMPLICIT_THREAD_REGISTER_TEXT})
    set(EASY_OPTION_EVENT_TRACING                ON CACHE BOOL "Enable event tracing by default")
    set(EASY_OPTION_LOW_PRIORITY_EVENT_TRACING   ON CACHE BOOL "Set low priority for event tracing thread")
else ()
    if (NO_CXX11_THREAD_LOCAL_SUPPORT)
        set(EASY_OPTION_IMPLICIT_THREAD_REGISTRATION OFF CACHE BOOL ${EASY_OPTION_IMPLICIT_THREAD_REGISTER_TEXT})
        set(EASY_OPTION_REMOVE_EMPTY_UNGUARDED_THREADS OFF CACHE BOOL "Enable easy_profiler to remove empty unguarded threads. This fixes potential memory leak on Unix systems, but may lead to an application crash! This is used when C++11 thread_local is unavailable.")
        
        add_definitions(-DEASY_THREAD_LOCAL=__thread)
    else ()
        set(EASY_OPTION_IMPLICIT_THREAD_REGISTRATION ON CACHE BOOL ${EASY_OPTION_IMPLICIT_THREAD_REGISTER_TEXT})
    endif ()
endif ()
set(BUILD_WITH_CHRONO_STEADY_CLOCK OFF CACHE BOOL "Use std::chrono::steady_clock as a timer" )
set(BUILD_WITH_CHRONO_HIGH_RESOLUTION_CLOCK OFF CACHE BOOL "Use std::chrono::high_resolution_clock as a timer")
# End EasyProfiler options.
###########################################################

add_definitions(-DEASY_DEFAULT_PORT=${EASY_DEFAULT_PORT})
if(EASY_OPTION_LISTEN)
 add_definitions(-DEASY_OPTION_START_LISTEN_ON_STARTUP=1)
else()
 add_definitions(-DEASY_OPTION_START_LISTEN_ON_STARTUP=0)
endif(EASY_OPTION_LISTEN)

if(EASY_OPTION_PROFILE_SELF)
 add_definitions(-DEASY_OPTION_MEASURE_STORAGE_EXPAND=1)
 if(EASY_OPTION_PROFILE_SELF_BLOCKS_ON)
  add_definitions(-DEASY_OPTION_STORAGE_EXPAND_BLOCKS_ON=true)
 else()
  add_definitions(-DEASY_OPTION_STORAGE_EXPAND_BLOCKS_ON=false)
 endif(EASY_OPTION_PROFILE_SELF_BLOCKS_ON)
else()
 add_definitions(-DEASY_OPTION_MEASURE_STORAGE_EXPAND=0)
endif(EASY_OPTION_PROFILE_SELF)

if(WIN32)
 if(EASY_OPTION_EVENT_TRACING)
  add_definitions(-DEASY_OPTION_EVENT_TRACING_ENABLED=true)
 else()
  add_definitions(-DEASY_OPTION_EVENT_TRACING_ENABLED=false)
 endif(EASY_OPTION_EVENT_TRACING)
 if(EASY_OPTION_LOW_PRIORITY_EVENT_TRACING)
  add_definitions(-DEASY_OPTION_LOW_PRIORITY_EVENT_TRACING=true)
 else()
  add_definitions(-DEASY_OPTION_LOW_PRIORITY_EVENT_TRACING=false)
 endif(EASY_OPTION_LOW_PRIORITY_EVENT_TRACING)
endif(WIN32)

if(EASY_OPTION_LOG)
 add_definitions(-DEASY_OPTION_LOG_ENABLED=1)
else()
 add_definitions(-DEASY_OPTION_LOG_ENABLED=0)
endif(EASY_OPTION_LOG)

if(EASY_OPTION_PREDEFINED_COLORS)
 add_definitions(-DEASY_OPTION_BUILTIN_COLORS=1)
else()
 add_definitions(-DEASY_OPTION_BUILTIN_COLORS=0)
endif(EASY_OPTION_PREDEFINED_COLORS)

add_definitions(
    -D_BUILD_PROFILER
    -DBUILD_WITH_EASY_PROFILER
    #-DEASY_PROFILER_API_DISABLED # uncomment this to disable profiler api only (you will have to rebuild only easy_profiler)
)

add_definitions(
    -D_WIN32_WINNT=0x0600
    -DSTRSAFE_NO_DEPRECATE
)

set(CPP_FILES
    ./${lib_path}/block.cpp
    ./${lib_path}/profile_manager.cpp
    ./${lib_path}/reader.cpp
    ./${lib_path}/thread_storage.cpp
    ./${lib_path}/event_trace_win.cpp
    ./${lib_path}/easy_socket.cpp
    ./${lib_path}/nonscoped_block.cpp
)

set(EasyProfiler_SOURCE_FILES
    ${CPP_FILES}
)


if (CMAKE_COMPILER_IS_GNUCC)
    set(FLAGS "-Wno-reorder -Wno-conversion-null -Wno-unknown-pragmas -Wno-missing-field-initializers")
    set_source_files_properties(${EasyProfiler_SOURCE_FILES} PROPERTIES COMPILE_FLAGS "${FLAGS}")
endif()

add_definitions(-DBUILD_WITH_EASY_PROFILER)
set(EasyProfiler_INCLUDE_DIRS "${prefix}/include")
#set(EasyProfiler_LIBDIR "${prefix}/bin")
#set(EasyProfiler_LIBRARIES "-L${EasyProfiler_LIBDIR} -Wl,-Bstatic -l:libeasy_profiler.a -Wl,-Bdynamic -lws2_32 -lpsapi")

if (MINGW)
    set(EasyProfiler_LIBRARIES "-lws2_32 -lpsapi")
endif()

if(UNIX)
    set(PLATFORM_LIBS ${PLATFORM_LIBS} pthread)
endif(UNIX)

