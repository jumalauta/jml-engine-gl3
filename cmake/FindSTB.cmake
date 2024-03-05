set(prefix "${PROJECT_SOURCE_DIR}/external/lib/stb") 
if (prefix)
    if (NOT EXISTS ${prefix})
        message(FATAL_ERROR "STB directory not found: " ${prefix})
    endif()
endif()

set(STB_INCLUDE_DIRS "${prefix}")
