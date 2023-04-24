/*[clinic input]
preserve
[clinic start generated code]*/

PyDoc_STRVAR(_cds__create_archive__doc__,
"_create_archive($module, /, archive)\n"
"--\n"
"\n");

#define _CDS__CREATE_ARCHIVE_METHODDEF    \
    {"_create_archive", (PyCFunction)(void(*)(void))_cds__create_archive, METH_FASTCALL|METH_KEYWORDS, _cds__create_archive__doc__},

static PyObject *
_cds__create_archive_impl(PyObject *module, const char *archive);

static PyObject *
_cds__create_archive(PyObject *module, PyObject *const *args, Py_ssize_t nargs, PyObject *kwnames)
{
    PyObject *return_value = NULL;
    static const char * const _keywords[] = {"archive", NULL};
    static _PyArg_Parser _parser = {NULL, _keywords, "_create_archive", 0};
    PyObject *argsbuf[1];
    const char *archive;

    args = _PyArg_UnpackKeywords(args, nargs, NULL, kwnames, &_parser, 1, 1, 0, argsbuf);
    if (!args) {
        goto exit;
    }
    if (!PyUnicode_Check(args[0])) {
        _PyArg_BadArgument("_create_archive", "argument 'archive'", "str", args[0]);
        goto exit;
    }
    Py_ssize_t archive_length;
    archive = PyUnicode_AsUTF8AndSize(args[0], &archive_length);
    if (archive == NULL) {
        goto exit;
    }
    if (strlen(archive) != (size_t)archive_length) {
        PyErr_SetString(PyExc_ValueError, "embedded null character");
        goto exit;
    }
    return_value = _cds__create_archive_impl(module, archive);

exit:
    return return_value;
}

PyDoc_STRVAR(_cds__load_archive__doc__,
"_load_archive($module, /, archive)\n"
"--\n"
"\n");

#define _CDS__LOAD_ARCHIVE_METHODDEF    \
    {"_load_archive", (PyCFunction)(void(*)(void))_cds__load_archive, METH_FASTCALL|METH_KEYWORDS, _cds__load_archive__doc__},

static PyObject *
_cds__load_archive_impl(PyObject *module, const char *archive);

static PyObject *
_cds__load_archive(PyObject *module, PyObject *const *args, Py_ssize_t nargs, PyObject *kwnames)
{
    PyObject *return_value = NULL;
    static const char * const _keywords[] = {"archive", NULL};
    static _PyArg_Parser _parser = {NULL, _keywords, "_load_archive", 0};
    PyObject *argsbuf[1];
    const char *archive;

    args = _PyArg_UnpackKeywords(args, nargs, NULL, kwnames, &_parser, 1, 1, 0, argsbuf);
    if (!args) {
        goto exit;
    }
    if (!PyUnicode_Check(args[0])) {
        _PyArg_BadArgument("_load_archive", "argument 'archive'", "str", args[0]);
        goto exit;
    }
    Py_ssize_t archive_length;
    archive = PyUnicode_AsUTF8AndSize(args[0], &archive_length);
    if (archive == NULL) {
        goto exit;
    }
    if (strlen(archive) != (size_t)archive_length) {
        PyErr_SetString(PyExc_ValueError, "embedded null character");
        goto exit;
    }
    return_value = _cds__load_archive_impl(module, archive);

exit:
    return return_value;
}

PyDoc_STRVAR(_cds__move_in__doc__,
"_move_in($module, /, op)\n"
"--\n"
"\n");

#define _CDS__MOVE_IN_METHODDEF    \
    {"_move_in", (PyCFunction)(void(*)(void))_cds__move_in, METH_FASTCALL|METH_KEYWORDS, _cds__move_in__doc__},

static PyObject *
_cds__move_in_impl(PyObject *module, PyObject *op);

static PyObject *
_cds__move_in(PyObject *module, PyObject *const *args, Py_ssize_t nargs, PyObject *kwnames)
{
    PyObject *return_value = NULL;
    static const char * const _keywords[] = {"op", NULL};
    static _PyArg_Parser _parser = {NULL, _keywords, "_move_in", 0};
    PyObject *argsbuf[1];
    PyObject *op;

    args = _PyArg_UnpackKeywords(args, nargs, NULL, kwnames, &_parser, 1, 1, 0, argsbuf);
    if (!args) {
        goto exit;
    }
    op = args[0];
    return_value = _cds__move_in_impl(module, op);

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
    {"_set_mode", (PyCFunction)(void(*)(void))_cds__set_mode, METH_FASTCALL|METH_KEYWORDS, _cds__set_mode__doc__},

static PyObject *
_cds__set_mode_impl(PyObject *module, int mode);

static PyObject *
_cds__set_mode(PyObject *module, PyObject *const *args, Py_ssize_t nargs, PyObject *kwnames)
{
    PyObject *return_value = NULL;
    static const char * const _keywords[] = {"mode", NULL};
    static _PyArg_Parser _parser = {NULL, _keywords, "_set_mode", 0};
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
    {"_set_verbose", (PyCFunction)(void(*)(void))_cds__set_verbose, METH_FASTCALL|METH_KEYWORDS, _cds__set_verbose__doc__},

static PyObject *
_cds__set_verbose_impl(PyObject *module, int mode);

static PyObject *
_cds__set_verbose(PyObject *module, PyObject *const *args, Py_ssize_t nargs, PyObject *kwnames)
{
    PyObject *return_value = NULL;
    static const char * const _keywords[] = {"mode", NULL};
    static _PyArg_Parser _parser = {NULL, _keywords, "_set_verbose", 0};
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
/*[clinic end generated code: output=df38bb0fb1b336cd input=a9049054013a1b77]*/
