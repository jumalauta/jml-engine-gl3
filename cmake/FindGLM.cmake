set(GLM_VERSION "0.9.9.8")
set(prefix "${PROJECT_SOURCE_DIR}/external/lib/glm-${GLM_VERSION}") 
if (prefix)
    if (NOT EXISTS ${prefix})
        message(FATAL_ERROR "GLM directory not found: " ${prefix})
    endif()
endif()

set(GLM_INCLUDE_DIRS "${prefix}")
