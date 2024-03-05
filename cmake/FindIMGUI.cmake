set(IMGUI_VERSION "1.83")
set(prefix "${PROJECT_SOURCE_DIR}/external/lib/imgui-${IMGUI_VERSION}") 
if (prefix)
    if (NOT EXISTS ${prefix})
        message(FATAL_ERROR "IMGUI directory not found: " ${prefix})
    endif()
endif()

set(IMGUI_INCLUDE_DIRS "${prefix}")
set(IMGUI_SOURCE_FILES
    ./external/lib/imgui-${IMGUI_VERSION}/imgui.cpp
    ./external/lib/imgui-${IMGUI_VERSION}/imgui_widgets.cpp
    ./external/lib/imgui-${IMGUI_VERSION}/imgui_draw.cpp
    ./external/lib/imgui-${IMGUI_VERSION}/imgui_tables.cpp
    ./external/lib/imgui-${IMGUI_VERSION}/imgui_demo.cpp
)

if (CMAKE_COMPILER_IS_GNUCXX)
    #set(FLAGS "-Wno-maybe-uninitialized")

    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "6.0.0")
        #set(FLAGS "${FLAGS} -Wno-misleading-indentation")
    endif()

    set_source_files_properties(${IMGUI_SOURCE_FILES} PROPERTIES COMPILE_FLAGS "${FLAGS}")
endif()

if (MINGW)
    set(IMGUI_LIBRARIES "-limm32")
endif()
