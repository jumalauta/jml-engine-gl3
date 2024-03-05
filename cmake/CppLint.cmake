function(add_check_cpp_target TARGET_NAME SRC_LIST)

    find_program(CPPLINT cpplint)
    if(NOT CPPLINT)
        find_package(PythonInterp)

        if(NOT PYTHONINTERP_FOUND)
            message(FATAL_ERROR "Python not found")
        endif()

        if(NOT PYTHON_VERSION_MAJOR VERSION_EQUAL 3)
            #CppLint silently fails with python 3... awkward
            #gjslint also depending on Python 2
            message(FATAL_ERROR "Python version 3 required, found version " ${PYTHON_VERSION_STRING})
        endif()

        set(CPPLINT "${PYTHON_EXECUTABLE}"
                                "-mcpplint")
    endif()

    set(CPPLINT_FILTERS ${CPPLINT_FILTERS}-legal/copyright,)
    set(CPPLINT_FILTERS ${CPPLINT_FILTERS}-build/include_subdir,)
    set(CPPLINT_FILTERS ${CPPLINT_FILTERS}-build/include,)
    set(CPPLINT_FILTERS ${CPPLINT_FILTERS}-build/header_guard,)
    set(CPPLINT_FILTERS ${CPPLINT_FILTERS}-build/c++11,)
    set(CPPLINT_FILTERS ${CPPLINT_FILTERS}-whitespace/indent,)

    add_custom_target(${TARGET_NAME}
        COMMAND ${CPPLINT}
                "--filter=${CPPLINT_FILTERS}"
                "--counting=detailed"
                "--extensions=cpp,hpp,h"
                "--linelength=150"
                ${SRC_LIST}
        DEPENDS ${SRC_LIST}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running CppLint")

endfunction()
