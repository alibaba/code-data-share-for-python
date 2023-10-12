cmake_minimum_required(VERSION 3.20)
project(pycds)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror=implicit-function-declaration")

# Only for IDE linting, skbuild will handle wheel dependencies.
if (NOT SKBUILD)
    # set(PYTHON_ROOT "")
    if (NOT SKBUILD_ASSETS)
        FILE(GLOB SKBUILD_ASSETS RELATIVE ${PYTHON_ROOT} "${PYTHON_ROOT}/lib/python*/site-packages/skbuild/resources/cmake")
    endif ()
    if (NOT PYTHON_ROOT OR NOT SKBUILD_ASSETS)
        message(FATAL_ERROR "Need to specify `PYTHON_ROOT` for the targeting python interpreter and `SKBUILD_ASSETS` for scikit-build cmake files (can be ignored if the installed in the target python).")
    endif ()
    set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PYTHON_ROOT}/${SKBUILD_ASSETS}")

    FILE(GLOB include RELATIVE ${PYTHON_ROOT} ${PYTHON_ROOT}/include/python*)
    FILE(GLOB libpython RELATIVE ${PYTHON_ROOT} ${PYTHON_ROOT}/lib/libpython*)

    set(PYTHON_EXECUTABLE "${PYTHON_ROOT}/bin/python3")
    set(PYTHON_INCLUDE_DIR "${PYTHON_ROOT}/${include}")
    set(PYTHON_LIBRARY "${PYTHON_ROOT}/${libpython}")
endif ()

find_package(Python 3.8 REQUIRED COMPONENTS Interpreter Development.Module)

add_compile_definitions(Py_BUILD_CORE)  # for sizeof(PyGC_Head)
add_compile_definitions(NEED_OPCODE_TABLES)  # for _PyOpcode_Caches and _PyOpcode_Deopt

python_add_library(_cds MODULE WITH_SOABI src/_cds/_cdsmodule.c src/_cds/lookup_table.c src/_cds/hashtable.c)
# clinic-generated files access core header.
target_include_directories(_cds PRIVATE "${PYTHON_INCLUDE_DIR}/internal")

# todo: if find cpython repo, run clinic
# currently we manually run `python3 ../cpython/Tools/clinic/clinic.py --make --srcdir src/_cds`

install(TARGETS _cds DESTINATION .)

install(FILES src/artifacts/pycds.pth DESTINATION .)
