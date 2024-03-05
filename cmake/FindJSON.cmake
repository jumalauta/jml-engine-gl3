# https://github.com/nlohmann/json/

set(JSON_VERSION "3.9.1")
set(prefix "${PROJECT_SOURCE_DIR}/external/lib/json-${JSON_VERSION}") 
if (prefix)
    if (NOT EXISTS ${prefix})
        message(FATAL_ERROR "JSON directory not found: " ${prefix})
    endif()
endif()

set(JSON_INCLUDE_DIRS "${prefix}")
