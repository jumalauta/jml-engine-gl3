set(DUKTAPE_VERSION "2.6.0")
set(relative_path "${EXT_LIB_ROOT}/duktape-${DUKTAPE_VERSION}")
set(prefix "${PROJECT_SOURCE_DIR}/${relative_path}") 
set(lib_path "${relative_path}/src")

if (prefix)
    if (NOT EXISTS ${prefix})
        message(FATAL_ERROR "DUKTAPE directory not found: " ${prefix})
    endif()
endif()

set(DUKTAPE_INCLUDE_DIRS "${lib_path}")
set(DUKTAPE_SOURCE_FILES
    ./${lib_path}/duktape.h
    ./${lib_path}/duk_config.h
    ./${lib_path}/duktape.c
)

if (CMAKE_COMPILER_IS_GNUCC)
    set(FLAGS "-Wno-unused-function -DDUK_USE_DEBUGGER_SUPPORT -DDUK_USE_INTERRUPT_COUNTER")
    set_source_files_properties(${DUKTAPE_SOURCE_FILES} PROPERTIES COMPILE_FLAGS "${FLAGS}")
endif()
