set(FontStash_VERSION "303564d")
set(relative_path "${EXT_LIB_ROOT}/fontstash-${FontStash_VERSION}")
set(prefix "${PROJECT_SOURCE_DIR}/${relative_path}") 
set(lib_path "${relative_path}/src")

if (prefix)
    if (NOT EXISTS ${prefix})
        message(FATAL_ERROR "Font-Stash directory not found: " ${prefix})
    endif()
endif()

set(FontStash_INCLUDE_DIRS "${lib_path}")
