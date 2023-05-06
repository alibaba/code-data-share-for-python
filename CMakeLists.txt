cmake_minimum_required(VERSION 3.20)
project(pycds)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror=implicit-function-declaration")

find_package(PythonExtensions REQUIRED)

add_compile_definitions(Py_BUILD_CORE)  # for sizeof(PyGC_Head)
add_compile_definitions(NEED_OPCODE_TABLES)  # for _PyOpcode_Caches and _PyOpcode_Deopt

add_library(_cds MODULE src/_cds/_cdsmodule.c src/_cds/lookup_table.c)
# clinic-generated files access core header.
target_include_directories(_cds PRIVATE "${PYTHON_INCLUDE_DIR}/internal")
python_extension_module(_cds)

# todo: if find cpython repo, run clinic
# currently we manually run `python3 ../cpython/Tools/clinic/clinic.py --make --srcdir src/_cds`

install(TARGETS _cds LIBRARY DESTINATION "${PYTHON_RELATIVE_SITE_PACKAGES_DIR}")

# by default site hooks is installed
if (NOT DEFINED ENV{DISABLE_SITE_HOOK})
    install(FILES src/artifacts/pycds.pth DESTINATION "${PYTHON_RELATIVE_SITE_PACKAGES_DIR}")
endif ()
