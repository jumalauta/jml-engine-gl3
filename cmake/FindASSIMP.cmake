# Lighterweight Assimp manual building
# git clone https://github.com/assimp/assimp.git && cd assimp && mkdir build && cd build && cmake -DASSIMP_BUILD_ZLIB=ON -DASSIMP_NO_EXPORT=ON -DASSIMP_BUILD_ASSIMP_TOOLS=OFF -DASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT=OFF -DASSIMP_BUILD_3DS_IMPORTER=ON -DASSIMP_BUILD_COLLADA_IMPORTER=ON -DASSIMP_BUILD_OBJ_IMPORTER=ON -DASSIMP_BUILD_TESTS=OFF .. && make

if (MSYS OR MINGW)
    set(ASSIMP_VERSION "4.0.1")
    set(prefix "${PROJECT_SOURCE_DIR}/external/lib/mingw/assimp-${ASSIMP_VERSION}") 
    if (prefix)
        if (NOT EXISTS ${prefix})
            message(FATAL_ERROR "ASSIMP directory not found: " ${prefix})
        endif()
    endif()

    set(exec_prefix "${prefix}")
    set(libdir "${exec_prefix}/lib")
    set(ASSIMP_LIBDIR "${libdir}")
    set(ASSIMP_INCLUDE_DIRS "${prefix}/include")
    set(ASSIMP_LIBRARIES "-lassimp")
    string(STRIP "${ASSIMP_LIBRARIES}" ASSIMP_LIBRARIES)
elseif (APPLE)
    FIND_PATH(ASSIMP_INCLUDE_DIRS assimp/mesh.h)
    if (NOT EXISTS ${ASSIMP_INCLUDE_DIRS})
        message(FATAL_ERROR "assimp framework not found")
    endif()

    set(ASSIMP_LIBRARIES "-framework assimp")
else()
    set(ASSIMP_LIBRARIES "-lassimp")
    string(STRIP "${ASSIMP_LIBRARIES}" ASSIMP_LIBRARIES)
endif()
