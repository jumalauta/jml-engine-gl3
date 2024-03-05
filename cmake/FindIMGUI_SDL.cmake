set(prefix "${PROJECT_SOURCE_DIR}/external/lib/imgui-${IMGUI_VERSION}/backends") 
if (prefix)
    if (NOT EXISTS ${prefix})
        message(FATAL_ERROR "IMGUI SDL OpengGL 3 directory not found: " ${prefix})
    endif()
endif()

set(IMGUI_SDL_INCLUDE_DIRS "${prefix}")
set(IMGUI_SDL_SOURCE_FILES
    ./external/lib/imgui-${IMGUI_VERSION}/backends/imgui_impl_sdl.cpp
    ./external/lib/imgui-${IMGUI_VERSION}/backends/imgui_impl_opengl3.cpp
)
