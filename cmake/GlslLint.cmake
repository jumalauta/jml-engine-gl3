function(add_check_glsl_target TARGET_NAME VERT_SRC_LIST FRAG_SRC_LIST)

    # URL: https://github.com/KhronosGroup/glslang/releases/tag/master-tot
    find_program(GLSLANGVALIDATOR glslangValidator)
    if(NOT GLSLANGVALIDATOR)
        message(WARNING "glslangValidator not found")
    else()
        add_custom_target(${TARGET_NAME}
            COMMAND ${GLSLANGVALIDATOR} -S vert ${VERT_SRC_LIST}
            COMMAND ${GLSLANGVALIDATOR} -S frag ${FRAG_SRC_LIST}
            DEPENDS ${VERT_SRC_LIST} ${FRAG_SRC_LIST}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Running GlslLint")
    endif()

endfunction()
