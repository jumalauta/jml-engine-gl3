project(engine)
cmake_minimum_required(VERSION 3.5)

# Builds must be done in <source root>/build directory
if (NOT "${CMAKE_SOURCE_DIR}/build" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "Build can only be invoked in '${CMAKE_SOURCE_DIR}/build' directory but it was attempted in '${CMAKE_BINARY_DIR}'. Example: mkdir build; cd build; cmake ..")
endif()

#cmake -G "MSYS Makefiles" .. && make

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release ... FORCE)
endif()

set(ENGINE_VERSION_MAJOR 0)
set(ENGINE_VERSION_MINOR 0)
set(ENGINE_VERSION_PATCH 1)

set(INT_SRC_ROOT "src/main")
set(EXT_LIB_ROOT "external/lib")
include_directories(${INT_SRC_ROOT})
include_directories(${EXT_LIB_ROOT})

# Check latest commit hash for version/debug information
set(ENGINE_LATEST_COMMIT 0)
EXECUTE_PROCESS(
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    OUTPUT_VARIABLE ENGINE_LATEST_COMMIT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

configure_file (
    "${CMAKE_SOURCE_DIR}/${INT_SRC_ROOT}/version.h.in"
    "${CMAKE_SOURCE_DIR}/${INT_SRC_ROOT}/version.h"
)

# Determine compiler
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUCC OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(IS_GCC_COMPATIBLE 1)
    if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "5.0.0")
        message(FATAL_ERROR "GCC version '${CMAKE_CXX_COMPILER_VERSION}' not supported. Higher needed for C++11 features.")
    endif()

else()
    message(STATUS "Not GCC compatible!")
    set(IS_GCC_COMPATIBLE 0)
endif()

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

if (APPLE)
    set (BUNDLE ON)
endif()

set (CXX_STANDARD_REQUIRED ON)
set (CXX_STANDARD 11)
set (C_STANDARD_REQUIRED ON)
set (C_STANDARD 99)

if(IS_GCC_COMPATIBLE)
    set(OPTIMIZATION_FLAGS "-O2 -DNDEBUG -s")
    # cmake -DCMAKE_BUILD_TYPE=Debug for debug builds
    set(DEBUG_FLAGS "-O0 -ggdb")

    if (APPLE)
        set(FRAMEWORKS "-F/System/Library/Frameworks -F/Library/Frameworks")
    endif (APPLE)

    # TODO: -Wno-unused-parameter & -Wno-unused-variable only during critical dev phase - cleanup later!
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=c++11 -Wno-unused-parameter -Wno-unused-variable ${FRAMEWORKS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -std=c99 ${FRAMEWORKS}")

    set(CMAKE_CXX_FLAGS_RELEASE "${OPTIMIZATION_FLAGS}")
    set(CMAKE_C_FLAGS_RELEASE "${OPTIMIZATION_FLAGS}")

    set(CMAKE_CXX_FLAGS_DEBUG "${DEBUG_FLAGS}")
    set(CMAKE_C_FLAGS_DEBUG "${DEBUG_FLAGS}")

endif()

# Some thread arguments are compile time
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)

SET(MAIN_CXX_SRC_FILES
    "${INT_SRC_ROOT}/main.cpp"
    "${INT_SRC_ROOT}/version.h"
    "${INT_SRC_ROOT}/Settings.cpp"
    "${INT_SRC_ROOT}/Settings.h"
    "${INT_SRC_ROOT}/EnginePlayer.cpp"
    "${INT_SRC_ROOT}/EnginePlayer.h"
    "${INT_SRC_ROOT}/io/File.cpp"
    "${INT_SRC_ROOT}/io/File.h"
    "${INT_SRC_ROOT}/io/ManagedMemoryObject.cpp"
    "${INT_SRC_ROOT}/io/ManagedMemoryObject.h"
    "${INT_SRC_ROOT}/io/MemoryManager.cpp"
    "${INT_SRC_ROOT}/io/MemoryManager.h"
    "${INT_SRC_ROOT}/io/LibraryLoader.cpp"
    "${INT_SRC_ROOT}/io/LibraryLoader.h"
    "${INT_SRC_ROOT}/io/FileRefreshManager.cpp"
    "${INT_SRC_ROOT}/io/FileRefreshManager.h"
    "${INT_SRC_ROOT}/io/NetworkManager.cpp"
    "${INT_SRC_ROOT}/io/NetworkManager.h"
    "${INT_SRC_ROOT}/io/Curl.cpp"
    "${INT_SRC_ROOT}/io/Curl.h"
    "${INT_SRC_ROOT}/io/EmbeddedResourceManager.cpp"
    "${INT_SRC_ROOT}/io/EmbeddedResourceManager.h"
    "${INT_SRC_ROOT}/io/EmbeddedResource.cpp"
    "${INT_SRC_ROOT}/io/EmbeddedResource.h"
    "${INT_SRC_ROOT}/io/Socket.cpp"
    "${INT_SRC_ROOT}/io/Socket.h"
    "${INT_SRC_ROOT}/io/MidiManager.h"
    "${INT_SRC_ROOT}/io/MidiManager.cpp"
    "${INT_SRC_ROOT}/io/MidiController.h"
    "${INT_SRC_ROOT}/io/MidiController.cpp"
    "${INT_SRC_ROOT}/io/MidiControllerRtMidi.h"
    "${INT_SRC_ROOT}/io/MidiControllerRtMidi.cpp"
    "${INT_SRC_ROOT}/script/Script.h"
    "${INT_SRC_ROOT}/script/Script.cpp"
    "${INT_SRC_ROOT}/script/ScriptEngine.h"
    "${INT_SRC_ROOT}/script/ScriptEngineDuktape.h"
    "${INT_SRC_ROOT}/script/ScriptEngineDuktape.cpp"
    "${INT_SRC_ROOT}/script/ScriptDuktape.h"
    "${INT_SRC_ROOT}/script/ScriptDuktape.cpp"
    "${INT_SRC_ROOT}/logger/logger.cpp"
    "${INT_SRC_ROOT}/logger/logger.h"
    "${INT_SRC_ROOT}/sync/Sync.cpp"
    "${INT_SRC_ROOT}/sync/Sync.h"
    "${INT_SRC_ROOT}/sync/SyncRocket.cpp"
    "${INT_SRC_ROOT}/sync/SyncRocket.h"
    "${INT_SRC_ROOT}/ui/GuiSdl.cpp"
    "${INT_SRC_ROOT}/ui/GuiSdl.h"
    "${INT_SRC_ROOT}/ui/Input.cpp"
    "${INT_SRC_ROOT}/ui/Input.h"
    "${INT_SRC_ROOT}/ui/InputSdl.cpp"
    "${INT_SRC_ROOT}/ui/InputSdl.h"
    "${INT_SRC_ROOT}/ui/Window.cpp"
    "${INT_SRC_ROOT}/ui/Window.h"
    "${INT_SRC_ROOT}/ui/WindowSdl.cpp"
    "${INT_SRC_ROOT}/ui/WindowSdl.h"
    "${INT_SRC_ROOT}/ui/menu/Menu.cpp"
    "${INT_SRC_ROOT}/ui/menu/Menu.h"
    "${INT_SRC_ROOT}/ui/menu/MenuImGui.cpp"
    "${INT_SRC_ROOT}/ui/menu/MenuImGui.h"
    #"${INT_SRC_ROOT}/audio/AudioSdlMixer.cpp"
    #"${INT_SRC_ROOT}/audio/AudioSdlMixer.h"
    "${INT_SRC_ROOT}/audio/AudioSdl.cpp"
    "${INT_SRC_ROOT}/audio/AudioSdl.h"
    "${INT_SRC_ROOT}/audio/AudioFile.cpp"
    "${INT_SRC_ROOT}/audio/AudioFile.h"
    "${INT_SRC_ROOT}/audio/AudioFileOgg.cpp"
    "${INT_SRC_ROOT}/audio/AudioFileOgg.h"
    "${INT_SRC_ROOT}/audio/Playlist.cpp"
    "${INT_SRC_ROOT}/audio/Playlist.h"
    "${INT_SRC_ROOT}/math/MathUtils.h"
    "${INT_SRC_ROOT}/math/TransformationMatrix.h"
    "${INT_SRC_ROOT}/math/TransformationMatrixGlm.h"
    "${INT_SRC_ROOT}/math/TransformationMatrixGlm.cpp"
    "${INT_SRC_ROOT}/graphics/datatypes.h"
    "${INT_SRC_ROOT}/graphics/datatypes.cpp"
    "${INT_SRC_ROOT}/graphics/Graphics.h"
    "${INT_SRC_ROOT}/graphics/GraphicsOpenGl.cpp"
    "${INT_SRC_ROOT}/graphics/GraphicsOpenGl.h"
    "${INT_SRC_ROOT}/graphics/Shadow.h"
    "${INT_SRC_ROOT}/graphics/Shadow.cpp"
    "${INT_SRC_ROOT}/graphics/Camera.h"
    "${INT_SRC_ROOT}/graphics/Camera.cpp"
    "${INT_SRC_ROOT}/graphics/Light.h"
    "${INT_SRC_ROOT}/graphics/Light.cpp"
    "${INT_SRC_ROOT}/graphics/LightManager.h"
    "${INT_SRC_ROOT}/graphics/LightManager.cpp"
    "${INT_SRC_ROOT}/graphics/Texture.h"
    "${INT_SRC_ROOT}/graphics/Texture.cpp"
    "${INT_SRC_ROOT}/graphics/TextureOpenGl.h"
    "${INT_SRC_ROOT}/graphics/TextureOpenGl.cpp"
    "${INT_SRC_ROOT}/graphics/Image.h"
    "${INT_SRC_ROOT}/graphics/Image.cpp"
    "${INT_SRC_ROOT}/graphics/ImageStb.cpp"
    "${INT_SRC_ROOT}/graphics/ImageStb.h"
    "${INT_SRC_ROOT}/graphics/Text.h"
    "${INT_SRC_ROOT}/graphics/Text.cpp"
    "${INT_SRC_ROOT}/graphics/Font.h"
    "${INT_SRC_ROOT}/graphics/Font.cpp"
    "${INT_SRC_ROOT}/graphics/FontFontStash.h"
    "${INT_SRC_ROOT}/graphics/FontFontStash.cpp"
    "${INT_SRC_ROOT}/graphics/Fbo.h"
    "${INT_SRC_ROOT}/graphics/Fbo.cpp"
    "${INT_SRC_ROOT}/graphics/FboOpenGl.h"
    "${INT_SRC_ROOT}/graphics/FboOpenGl.cpp"
    "${INT_SRC_ROOT}/graphics/Shader.h"
    "${INT_SRC_ROOT}/graphics/Shader.cpp"
    "${INT_SRC_ROOT}/graphics/ShaderOpenGl.cpp"
    "${INT_SRC_ROOT}/graphics/ShaderOpenGl.h"
    "${INT_SRC_ROOT}/graphics/ShaderProgram.h"
    "${INT_SRC_ROOT}/graphics/ShaderProgram.cpp"
    "${INT_SRC_ROOT}/graphics/ShaderProgramOpenGl.cpp"
    "${INT_SRC_ROOT}/graphics/ShaderProgramOpenGl.h"
    "${INT_SRC_ROOT}/graphics/ShaderVariable.h"
    "${INT_SRC_ROOT}/graphics/ShaderVariable.cpp"
    "${INT_SRC_ROOT}/graphics/ShaderVariableOpenGl.h"
    "${INT_SRC_ROOT}/graphics/ShaderVariableOpenGl.cpp"
    "${INT_SRC_ROOT}/graphics/model/TexturedQuad.h"
    "${INT_SRC_ROOT}/graphics/model/TexturedQuad.cpp"
    "${INT_SRC_ROOT}/graphics/model/Model.cpp"
    "${INT_SRC_ROOT}/graphics/model/Model.h"
    "${INT_SRC_ROOT}/graphics/model/ModelAssimp.cpp"
    "${INT_SRC_ROOT}/graphics/model/ModelAssimp.h"
    "${INT_SRC_ROOT}/graphics/model/Mesh.cpp"
    "${INT_SRC_ROOT}/graphics/model/Mesh.h"
    "${INT_SRC_ROOT}/graphics/model/Material.cpp"
    "${INT_SRC_ROOT}/graphics/model/Material.h"
    "${INT_SRC_ROOT}/graphics/video/VideoFile.cpp"
    "${INT_SRC_ROOT}/graphics/video/VideoFile.h"
    "${INT_SRC_ROOT}/graphics/video/VideoFileTheora.cpp"
    "${INT_SRC_ROOT}/graphics/video/VideoFileTheora.h"
    "${INT_SRC_ROOT}/time/Date.cpp"
    "${INT_SRC_ROOT}/time/Date.h"
    "${INT_SRC_ROOT}/time/TimeFormatter.cpp"
    "${INT_SRC_ROOT}/time/TimeFormatter.h"
    "${INT_SRC_ROOT}/time/SystemTime.cpp"
    "${INT_SRC_ROOT}/time/SystemTime.h"
    "${INT_SRC_ROOT}/time/Timer.cpp"
    "${INT_SRC_ROOT}/time/Timer.h"
    "${INT_SRC_ROOT}/time/Fps.cpp"
    "${INT_SRC_ROOT}/time/Fps.h"
)

#Stb generates a lot of stuff
set_source_files_properties("${INT_SRC_ROOT}/graphics/ImageStb.cpp" PROPERTIES COMPILE_FLAGS "-Wno-unused-function -Wno-misleading-indentation")
set_source_files_properties("${INT_SRC_ROOT}/graphics/FontFontStash.cpp" PROPERTIES COMPILE_FLAGS "-Wno-unused-function")

SOURCE_GROUP(Main FILES ${MAIN_CXX_SRC_FILES})

set(INT_CXX_SRC_FILES
    ${MAIN_CXX_SRC_FILES}
)

set(INT_JS_SRC_FILES
    "${INT_SRC_ROOT}/player/polyfill.js"
    "${INT_SRC_ROOT}/player/Core.js"
    "${INT_SRC_ROOT}/player/Settings.js"
    "${INT_SRC_ROOT}/player/Utils.js"
    "${INT_SRC_ROOT}/player/Effect.js"
    "${INT_SRC_ROOT}/player/Shader.js"
    "${INT_SRC_ROOT}/player/Sync.js"
    "${INT_SRC_ROOT}/player/Scene.js"
    "${INT_SRC_ROOT}/player/Audio.js"
    "${INT_SRC_ROOT}/player/Loader.js"
    "${INT_SRC_ROOT}/player/Player.js"
    "${INT_SRC_ROOT}/player/Graphics.js"
    "${INT_SRC_ROOT}/player/Video.js"
    "${INT_SRC_ROOT}/player/Image.js"
    "${INT_SRC_ROOT}/player/Text.js"
    "${INT_SRC_ROOT}/player/Fbo.js"
    "${INT_SRC_ROOT}/player/Model.js"
    "${INT_SRC_ROOT}/player/Mesh.js"
    "${INT_SRC_ROOT}/player/Light.js"
    "${INT_SRC_ROOT}/player/Camera.js"
    "${INT_SRC_ROOT}/player/Menu.js"
    "${INT_SRC_ROOT}/player/Input.js"
    "${INT_SRC_ROOT}/player/Socket.js"
    "${INT_SRC_ROOT}/math/CatmullRomSpline.js"
)

set(INT_GLSL_VERT_SRC_FILES
    "${INT_SRC_ROOT}/graphics/default.vs"
    "${INT_SRC_ROOT}/graphics/defaultPlain.vs"
)

set(INT_GLSL_FRAG_SRC_FILES
    "${INT_SRC_ROOT}/graphics/default.fs"
    "${INT_SRC_ROOT}/graphics/defaultPlain.fs"
    "${INT_SRC_ROOT}/graphics/progressBar.fs"
)

MACRO(ADD_MODULE name packages)
    OPTION(MODULE_${name} "include module ${name}" TRUE)
    IF(MODULE_${name})
        add_definitions(${${lib}_DEFINITIONS})
        LIST(APPEND ASSIMP_LOADER_SRCS ${ARGN})
        SET(ASSIMP_IMPORTERS_ENABLED "${ASSIMP_IMPORTERS_ENABLED} ${name}")
        SET(${name}_SRCS ${ARGN})
        SOURCE_GROUP(${name} FILES ${ARGN})

        handle_external_package(packages)
    ENDIF()
ENDMACRO()

INCLUDE(FindPkgConfig)

function(handle_external_package)
    message(STATUS "??? ${ARGN}")
    foreach(lib ${ARGN})
        message(STATUS "Handling ${lib}")
        find_package(${lib} REQUIRED)
        if (NOT ${${lib}_FOUND})
            message(FATAL_ERROR ${lib} " not found")
        endif()

        include_directories(${${lib}_INCLUDE_DIRS})
        add_definitions(${${lib}_DEFINITIONS})

        list(APPEND LIBS "${${lib}_LIBRARIES}")
        list(APPEND LIBDIRS "${${lib}_LIBDIR}")
        list(APPEND EXT_CXX_SRC_FILES "${${lib}_SOURCE_FILES}")
        list(APPEND EXT_JS_SRC_FILES "${${lib}_JS_SOURCE_FILES}")

        set(LIBS "${LIBS}" PARENT_SCOPE)
        set(LIBDIRS "${LIBDIRS}" PARENT_SCOPE)
        set(EXT_CXX_SRC_FILES "${EXT_CXX_SRC_FILES}" PARENT_SCOPE)
        set(EXT_JS_SRC_FILES "${EXT_JS_SRC_FILES}" PARENT_SCOPE)

        SOURCE_GROUP(${lib} FILES ${EXT_CXX_SRC_FILES} ${EXT_JS_SRC_FILES})
    endforeach(lib)
endfunction()

handle_external_package(EasyProfiler OptionParser STB DUKTAPE GL3W GLM JSON FontStash ASSIMP ROCKET SDL2 SDL2_MIXER TINYXML2 IMGUI IMGUI_SDL RTMIDI Ogg Vorbis Theora THEORAPLAY Curl)

# Code analysis
include(GlslLint)
add_check_glsl_target(check_glsl "${INT_GLSL_VERT_SRC_FILES}" "${INT_GLSL_FRAG_SRC_FILES}")
include(CppLint)
add_check_cpp_target(check_cpp "${INT_CXX_SRC_FILES}")
include(JsLint)
add_check_js_target(check_js "${INT_JS_SRC_FILES}")
add_custom_target(check
    DEPENDS check_glsl check_cpp check_js
    COMMENT "Checking sources")

list(APPEND ALL_CXX_SRC_FILES ${EXT_CXX_SRC_FILES} ${INT_CXX_SRC_FILES})
message(STATUS "Sources: " ${ALL_CXX_SRC_FILES})
message(STATUS "Libdirs: " ${LIBDIRS})
message(STATUS "Libs: " ${LIBS})
message(STATUS "Build type:" ${CMAKE_BUILD_TYPE})

if(IS_GCC_COMPATIBLE)
    if (WIN32)
        #In Windows we're mainly interested about baking the std libs to the executable
        #Funkiness warning!
        #-static-libgcc needed for static linking of libgcc_s_seh
        #-Wl,-Bstatic -lstdc++ needed for static linking of libwinpthreads...
        list(APPEND LIBS "-static-libgcc -static-libstdc++")
    elseif (NOT APPLE)
        list(APPEND LIBS "-lgcc -lstdc++")
        # pthreads needed at least by Linux build
        find_package(Threads REQUIRED)
        list(APPEND LIBS Threads::Threads)
    endif()
endif()

set(CMAKE_EXE_LINKER_FLAGS    "-Wl,--as-needed ${CMAKE_EXE_LINKER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--as-needed ${CMAKE_SHARED_LINKER_FLAGS}")
set(LIBS "-Wl,--as-needed ${LIBS}")


link_directories(${LIBDIRS})
add_executable(${PROJECT_NAME} ${ALL_CXX_SRC_FILES})
target_link_libraries(${PROJECT_NAME} ${LIBS})
add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_SOURCE_DIR}/release
)

function(create_embedded_source_resources)
    set(output_file "${INT_SRC_ROOT}/io/EmbeddedResources.h")
    string(TIMESTAMP generation_time "%Y-%m-%d %H:%M")
    file(WRITE ${output_file} "// THIS FILE IS AUTOMATICALLY GENERATED ${generation_time}\n")

    foreach(input_file_path ${ARGN})
        string(REGEX MATCH "([^/]+)$" file_name ${input_file_path})
        string(REGEX REPLACE "\\.| |-" "_" variable_name ${file_name})
        set(generated_file_path "_embedded/${file_name}")

        # Read raw file data and determine length
        file(READ ${input_file_path} filedata HEX)
        string(LENGTH "${filedata}" file_size)
        math(EXPR file_size "${file_size} / 2")

        # Convert to c-style hex string
        string(REGEX REPLACE "([0-9a-f][0-9a-f])" "\\\\x\\1" filedata ${filedata})

        file(APPEND ${output_file} "\n// ${input_file_path}\n")
        file(APPEND ${output_file} "const struct {\n\tconst char *filePath;\n\tconst char *data;\n\tsize_t length;\n} ${variable_name} = {\n\t")
        file(APPEND ${output_file} "\"${generated_file_path}\",\n\t\"${filedata}\",\n\t${file_size}\n};\n")

        list(APPEND resources "${variable_name}")
    endforeach()

    file(APPEND ${output_file} "\nvoid EmbeddedResourceManager::createEmbeddedResources() {\n")
    foreach(variable_name ${resources})
        file(APPEND ${output_file} "\taddResource(${variable_name}.filePath, reinterpret_cast<const unsigned char*>(${variable_name}.data), ${variable_name}.length);\n")
    endforeach()
    file(APPEND ${output_file} "}\n")
endfunction()

create_embedded_source_resources(
    ${INT_JS_SRC_FILES}
    "${INT_SRC_ROOT}/../../README.md"
    "${INT_SRC_ROOT}/graphics/defaultWhite.png"
    "${INT_SRC_ROOT}/graphics/defaultTransparent.png"
    ${INT_GLSL_VERT_SRC_FILES}
    ${INT_GLSL_FRAG_SRC_FILES}
    "${INT_SRC_ROOT}/graphics/shadertoyBootstrap.fs.part")
