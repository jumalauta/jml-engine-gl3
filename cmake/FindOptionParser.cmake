set(OPTIONPARSER_VERSION "1.7")
set(prefix "${PROJECT_SOURCE_DIR}/external/lib/optionparser-${OPTIONPARSER_VERSION}/src") 
if (prefix)
    if (NOT EXISTS ${prefix})
        message(FATAL_ERROR "OptionParser directory not found: " ${prefix})
    endif()
endif()

set(OptionParser_INCLUDE_DIRS "${prefix}")
