cmake_minimum_required(VERSION 3.20)
project(pycds)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror=implicit-function-declaration")

#set(CMAKE_MODULE_PATH /Users/yyc/repo/py/install/lib/python3.12/site-packages/skbuild/resources/cmake)
#set(PYTHON_EXECUTABLE /Users/yyc/repo/py/install/bin/python3)
#set(PYTHON_INCLUDE_DIR /Users/yyc/repo/py/install/include/python3.12d)
#set(PYTHON_LIBRARY /Users/yyc/repo/py/install/lib/libpython3.12d.a)
set(CMAKE_BUILD_TYPE Debug)

find_package(PythonExtensions REQUIRED)

add_compile_definitions(Py_BUILD_CORE)  # for sizeof(PyGC_Head)
add_compile_definitions(NEED_OPCODE_TABLES)  # for _PyOpcode_Caches

add_library(_cds MODULE src/_cds/_cdsmodule.c src/_cds/lookup_table.c src/_cds/hashtable.c)
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
