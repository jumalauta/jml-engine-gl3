
function(add_check_js_target TARGET_NAME SRC_LIST)

    find_program(ESLINT eslint)
    if(NOT ESLINT)
        message(FATAL_ERROR "eslint required (via npm)")
    endif()

    set(ESLINT_EXECUTABLE)

    add_custom_target(${TARGET_NAME}
        COMMAND ${ESLINT}
                --config ${CMAKE_SOURCE_DIR}/.eslintrc.js
                ${SRC_LIST}
        DEPENDS ${SRC_LIST}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running JsLint")

endfunction()
