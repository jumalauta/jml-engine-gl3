set(GL3W_VERSION "7729692")

set(prefix "${PROJECT_SOURCE_DIR}/external/lib/gl3w-${GL3W_VERSION}") 
if (prefix)
    if (NOT EXISTS ${prefix})
        message(FATAL_ERROR "GL3W directory not found: " ${prefix})
    endif()
endif()

set(GL3W_INCLUDE_DIRS "${prefix}/include")
#if source file is missing go to gl3w directory and run: python3 gl3w_gen.py
set(GL3W_SOURCE_FILES
    ${PROJECT_SOURCE_DIR}/external/lib/gl3w-${GL3W_VERSION}/src/gl3w.c
)

if (MSYS OR MINGW)
    set(GL3W_LIBRARIES "-lopengl32")
elseif (APPLE)
    set(GL3W_LIBRARIES "-framework OpenGL")
else()
    PKG_SEARCH_MODULE(OPENGL required opengl)
    set(GL3W_LIBRARIES "${OPENGL_LIBRARIES} ${CMAKE_DL_LIBS}")
    string(STRIP "${GL3W_LIBRARIES}" GL3W_LIBRARIES)
endif()

