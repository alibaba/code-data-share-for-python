/*[clinic input]
preserve
[clinic start generated code]*/

#if defined(Py_BUILD_CORE) && !defined(Py_BUILD_CORE_MODULE)
#  include "pycore_gc.h"            // PyGC_Head
#  include "pycore_runtime.h"       // _Py_ID()
#endif


PyDoc_STRVAR(_cds__create_archive__doc__,
"_create_archive($module, /, a)\n"
"--\n"
"\n");

#define _CDS__CREATE_ARCHIVE_METHODDEF    \
    {"_create_archive", _PyCFunction_CAST(_cds__create_archive), METH_FASTCALL|METH_KEYWORDS, _cds__create_archive__doc__},

static PyObject *
_cds__create_archive_impl(PyObject *module, const char *a);

static PyObject *
_cds__create_archive(PyObject *module, PyObject *const *args, Py_ssize_t nargs, PyObject *kwnames)
{
    PyObject *return_value = NULL;
    #if defined(Py_BUILD_CORE) && !defined(Py_BUILD_CORE_MODULE)

    #define NUM_KEYWORDS 1
    static struct {
        PyGC_Head _this_is_not_used;
        PyObject_VAR_HEAD
        PyObject *ob_item[NUM_KEYWORDS];
    } _kwtuple = {
        .ob_base = PyVarObject_HEAD_INIT(&PyTuple_Type, NUM_KEYWORDS)
        .ob_item = { &_Py_ID(a), },
    };
    #undef NUM_KEYWORDS
    #define KWTUPLE (&_kwtuple.ob_base.ob_base)

    #else  // !Py_BUILD_CORE
    #  define KWTUPLE NULL
    #endif  // !Py_BUILD_CORE

    static const char * const _keywords[] = {"a", NULL};
    static _PyArg_Parser _parser = {
        .keywords = _keywords,
        .fname = "_create_archive",
        .kwtuple = KWTUPLE,
    };
    #undef KWTUPLE
    PyObject *argsbuf[1];
    const char *a;

    args = _PyArg_UnpackKeywords(args, nargs, NULL, kwnames, &_parser, 1, 1, 0, argsbuf);
    if (!args) {
        goto exit;
    }
    if (!PyUnicode_Check(args[0])) {
        _PyArg_BadArgument("_create_archive", "argument 'a'", "str", args[0]);
        goto exit;
    }
    Py_ssize_t a_length;
    a = PyUnicode_AsUTF8AndSize(args[0], &a_length);
    if (a == NULL) {
        goto exit;
    }
    if (strlen(a) != (size_t)a_length) {
        PyErr_SetString(PyExc_ValueError, "embedded null character");
        goto exit;
    }
    return_value = _cds__create_archive_impl(module, a);

exit:
    return return_value;
}

PyDoc_STRVAR(_cds__load_archive__doc__,
"_load_archive($module, /, a)\n"
"--\n"
"\n");

#define _CDS__LOAD_ARCHIVE_METHODDEF    \
    {"_load_archive", _PyCFunction_CAST(_cds__load_archive), METH_FASTCALL|METH_KEYWORDS, _cds__load_archive__doc__},

static PyObject *
_cds__load_archive_impl(PyObject *module, const char *a);

static PyObject *
_cds__load_archive(PyObject *module, PyObject *const *args, Py_ssize_t nargs, PyObject *kwnames)
{
    PyObject *return_value = NULL;
    #if defined(Py_BUILD_CORE) && !defined(Py_BUILD_CORE_MODULE)

    #define NUM_KEYWORDS 1
    static struct {
        PyGC_Head _this_is_not_used;
        PyObject_VAR_HEAD
        PyObject *ob_item[NUM_KEYWORDS];
    } _kwtuple = {
        .ob_base = PyVarObject_HEAD_INIT(&PyTuple_Type, NUM_KEYWORDS)
        .ob_item = { &_Py_ID(a), },
    };
    #undef NUM_KEYWORDS
    #define KWTUPLE (&_kwtuple.ob_base.ob_base)

    #else  // !Py_BUILD_CORE
    #  define KWTUPLE NULL
    #endif  // !Py_BUILD_CORE

    static const char * const _keywords[] = {"a", NULL};
    static _PyArg_Parser _parser = {
        .keywords = _keywords,
        .fname = "_load_archive",
        .kwtuple = KWTUPLE,
    };
    #undef KWTUPLE
    PyObject *argsbuf[1];
    const char *a;

    args = _PyArg_UnpackKeywords(args, nargs, NULL, kwnames, &_parser, 1, 1, 0, argsbuf);
    if (!args) {
        goto exit;
    }
    if (!PyUnicode_Check(args[0])) {
        _PyArg_BadArgument("_load_archive", "argument 'a'", "str", args[0]);
        goto exit;
    }
    Py_ssize_t a_length;
    a = PyUnicode_AsUTF8AndSize(args[0], &a_length);
    if (a == NULL) {
        goto exit;
    }
    if (strlen(a) != (size_t)a_length) {
        PyErr_SetString(PyExc_ValueError, "embedded null character");
        goto exit;
    }
    return_value = _cds__load_archive_impl(module, a);

exit:
    return return_value;
}

PyDoc_STRVAR(_cds__move_in__doc__,
"_move_in($module, /, obj)\n"
"--\n"
"\n");

#define _CDS__MOVE_IN_METHODDEF    \
    {"_move_in", _PyCFunction_CAST(_cds__move_in), METH_FASTCALL|METH_KEYWORDS, _cds__move_in__doc__},

static PyObject *
_cds__move_in_impl(PyObject *module, PyObject *obj);

static PyObject *
_cds__move_in(PyObject *module, PyObject *const *args, Py_ssize_t nargs, PyObject *kwnames)
{
    PyObject *return_value = NULL;
    #if defined(Py_BUILD_CORE) && !defined(Py_BUILD_CORE_MODULE)

    #define NUM_KEYWORDS 1
    static struct {
        PyGC_Head _this_is_not_used;
        PyObject_VAR_HEAD
        PyObject *ob_item[NUM_KEYWORDS];
    } _kwtuple = {
        .ob_base = PyVarObject_HEAD_INIT(&PyTuple_Type, NUM_KEYWORDS)
        .ob_item = { &_Py_ID(obj), },
    };
    #undef NUM_KEYWORDS
    #define KWTUPLE (&_kwtuple.ob_base.ob_base)

    #else  // !Py_BUILD_CORE
    #  define KWTUPLE NULL
    #endif  // !Py_BUILD_CORE

    static const char * const _keywords[] = {"obj", NULL};
    static _PyArg_Parser _parser = {
        .keywords = _keywords,
        .fname = "_move_in",
        .kwtuple = KWTUPLE,
    };
    #undef KWTUPLE
    PyObject *argsbuf[1];
    PyObject *obj;

    args = _PyArg_UnpackKeywords(args, nargs, NULL, kwnames, &_parser, 1, 1, 0, argsbuf);
    if (!args) {
        goto exit;
    }
    obj = args[0];
    return_value = _cds__move_in_impl(module, obj);

exit:
    return return_value;
}

PyDoc_STRVAR(_cds__get_obj__doc__,
"_get_obj($module, /)\n"
"--\n"
"\n");

#define _CDS__GET_OBJ_METHODDEF    \
    {"_get_obj", (PyCFunction)_cds__get_obj, METH_NOARGS, _cds__get_obj__doc__},

static PyObject *
_cds__get_obj_impl(PyObject *module);

static PyObject *
_cds__get_obj(PyObject *module, PyObject *Py_UNUSED(ignored))
{
    return _cds__get_obj_impl(module);
}

PyDoc_STRVAR(_cds__set_mode__doc__,
"_set_mode($module, /, mode)\n"
"--\n"
"\n"
"Set mode for only tracer and manual mode, from cds module.\n"
"\n"
"Other mode will be set by extension module.");

#define _CDS__SET_MODE_METHODDEF    \
    {"_set_mode", _PyCFunction_CAST(_cds__set_mode), METH_FASTCALL|METH_KEYWORDS, _cds__set_mode__doc__},

static PyObject *
_cds__set_mode_impl(PyObject *module, int mode);

static PyObject *
_cds__set_mode(PyObject *module, PyObject *const *args, Py_ssize_t nargs, PyObject *kwnames)
{
    PyObject *return_value = NULL;
    #if defined(Py_BUILD_CORE) && !defined(Py_BUILD_CORE_MODULE)

    #define NUM_KEYWORDS 1
    static struct {
        PyGC_Head _this_is_not_used;
        PyObject_VAR_HEAD
        PyObject *ob_item[NUM_KEYWORDS];
    } _kwtuple = {
        .ob_base = PyVarObject_HEAD_INIT(&PyTuple_Type, NUM_KEYWORDS)
        .ob_item = { &_Py_ID(mode), },
    };
    #undef NUM_KEYWORDS
    #define KWTUPLE (&_kwtuple.ob_base.ob_base)

    #else  // !Py_BUILD_CORE
    #  define KWTUPLE NULL
    #endif  // !Py_BUILD_CORE

    static const char * const _keywords[] = {"mode", NULL};
    static _PyArg_Parser _parser = {
        .keywords = _keywords,
        .fname = "_set_mode",
        .kwtuple = KWTUPLE,
    };
    #undef KWTUPLE
    PyObject *argsbuf[1];
    int mode;

    args = _PyArg_UnpackKeywords(args, nargs, NULL, kwnames, &_parser, 1, 1, 0, argsbuf);
    if (!args) {
        goto exit;
    }
    mode = _PyLong_AsInt(args[0]);
    if (mode == -1 && PyErr_Occurred()) {
        goto exit;
    }
    return_value = _cds__set_mode_impl(module, mode);

exit:
    return return_value;
}

PyDoc_STRVAR(_cds__get_initialized__doc__,
"_get_initialized($module, /)\n"
"--\n"
"\n"
"Get if cds module is already initialized.");

#define _CDS__GET_INITIALIZED_METHODDEF    \
    {"_get_initialized", (PyCFunction)_cds__get_initialized, METH_NOARGS, _cds__get_initialized__doc__},

static int
_cds__get_initialized_impl(PyObject *module);

static PyObject *
_cds__get_initialized(PyObject *module, PyObject *Py_UNUSED(ignored))
{
    PyObject *return_value = NULL;
    int _return_value;

    _return_value = _cds__get_initialized_impl(module);
    if ((_return_value == -1) && PyErr_Occurred()) {
        goto exit;
    }
    return_value = PyBool_FromLong((long)_return_value);

exit:
    return return_value;
}

PyDoc_STRVAR(_cds__set_verbose__doc__,
"_set_verbose($module, /, mode)\n"
"--\n"
"\n"
"Set verbosity and return old verbosity.");

#define _CDS__SET_VERBOSE_METHODDEF    \
    {"_set_verbose", _PyCFunction_CAST(_cds__set_verbose), METH_FASTCALL|METH_KEYWORDS, _cds__set_verbose__doc__},

static PyObject *
_cds__set_verbose_impl(PyObject *module, int mode);

static PyObject *
_cds__set_verbose(PyObject *module, PyObject *const *args, Py_ssize_t nargs, PyObject *kwnames)
{
    PyObject *return_value = NULL;
    #if defined(Py_BUILD_CORE) && !defined(Py_BUILD_CORE_MODULE)

    #define NUM_KEYWORDS 1
    static struct {
        PyGC_Head _this_is_not_used;
        PyObject_VAR_HEAD
        PyObject *ob_item[NUM_KEYWORDS];
    } _kwtuple = {
        .ob_base = PyVarObject_HEAD_INIT(&PyTuple_Type, NUM_KEYWORDS)
        .ob_item = { &_Py_ID(mode), },
    };
    #undef NUM_KEYWORDS
    #define KWTUPLE (&_kwtuple.ob_base.ob_base)

    #else  // !Py_BUILD_CORE
    #  define KWTUPLE NULL
    #endif  // !Py_BUILD_CORE

    static const char * const _keywords[] = {"mode", NULL};
    static _PyArg_Parser _parser = {
        .keywords = _keywords,
        .fname = "_set_verbose",
        .kwtuple = KWTUPLE,
    };
    #undef KWTUPLE
    PyObject *argsbuf[1];
    int mode;

    args = _PyArg_UnpackKeywords(args, nargs, NULL, kwnames, &_parser, 1, 1, 0, argsbuf);
    if (!args) {
        goto exit;
    }
    mode = _PyLong_AsInt(args[0]);
    if (mode == -1 && PyErr_Occurred()) {
        goto exit;
    }
    return_value = _cds__set_verbose_impl(module, mode);

exit:
    return return_value;
}
/*[clinic end generated code: output=b052111ddd3acec0 input=a9049054013a1b77]*/
