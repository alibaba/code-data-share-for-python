cmake_minimum_required(VERSION 3.20)
project(pycds)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror=implicit-function-declaration")

# Only for IDE linting, skbuild will handle wheel dependencies.
if (NOT SKBUILD)
    #set(Python_ROOT_DIR "")
endif ()

find_package(Python 3.8 REQUIRED COMPONENTS Interpreter Development.Module)

add_compile_definitions(Py_BUILD_CORE)  # for sizeof(PyGC_Head)
add_compile_definitions(NEED_OPCODE_TABLES)  # for _PyOpcode_Caches and _PyOpcode_Deopt

python_add_library(_cds MODULE WITH_SOABI src/_cds/_cdsmodule.c src/_cds/lookup_table.c src/_cds/hashtable.c)
# clinic-generated files access core header.
target_include_directories(_cds PRIVATE "${Python_INCLUDE_DIRS}/internal")

# todo: if find cpython repo, run clinic
# currently we manually run `python3 ../cpython/Tools/clinic/clinic.py --make --srcdir src/_cds`

install(TARGETS _cds DESTINATION .)

install(FILES src/artifacts/pycds.pth DESTINATION .)
