set(relative_path "${EXT_LIB_ROOT}/theoraplay")
set(prefix "${PROJECT_SOURCE_DIR}/${relative_path}") 
set(lib_path "${relative_path}")

if (prefix)
    if (NOT EXISTS ${prefix})
        message(FATAL_ERROR "THEORAPLAY directory not found: " ${prefix})
    endif()
endif()

set(THEORAPLAY_INCLUDE_DIRS "${lib_path}")
set(THEORAPLAY_SOURCE_FILES
    ./${lib_path}/theoraplay.c
)

# Image pixels need to be flipped vertically
set(FLAGS "-DTHEORAPLAY_FLIP_VERTICALLY")
set_source_files_properties(${THEORAPLAY_SOURCE_FILES} PROPERTIES COMPILE_FLAGS "${FLAGS}")
