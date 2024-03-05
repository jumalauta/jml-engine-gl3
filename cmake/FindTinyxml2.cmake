set(TINYXML2_VERSION "9.0.0")
set(prefix "${PROJECT_SOURCE_DIR}/external/lib/tinyxml2-${TINYXML2_VERSION}") 
if (prefix)
    if (NOT EXISTS ${prefix})
        message(FATAL_ERROR "tinyxml2 directory not found: " ${prefix})
    endif()
endif()

set(TINYXML2_INCLUDE_DIRS "${prefix}")
set(TINYXML2_SOURCE_FILES
    ./external/lib/tinyxml2-${TINYXML2_VERSION}/tinyxml2.cpp
    ./external/lib/tinyxml2-${TINYXML2_VERSION}/tinyxml2.h
)
