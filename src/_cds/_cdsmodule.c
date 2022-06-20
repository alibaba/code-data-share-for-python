#include "_cdsmodule.h"

#include <stddef.h>
#include <sys/fcntl.h>
#include <sys/mman.h>

#ifdef MAP_POPULATE
#define M_POPULATE MAP_POPULATE
#else
#define M_POPULATE 0
#endif

/*[clinic input]
module _cds
[clinic start generated code]*/
/*[clinic end generated code: output=da39a3ee5e6b4b0d input=4970e54eebac52d1]*/

// clang-format off
static PyMethodDef CDSMethods[] = {
    _CDS__CREATE_ARCHIVE_METHODDEF
    _CDS__LOAD_ARCHIVE_METHODDEF
    _CDS__MOVE_IN_METHODDEF
    _CDS__GET_OBJ_METHODDEF
    _CDS__SET_MODE_METHODDEF
    _CDS__GET_INITIALIZED_METHODDEF
    _CDS__SET_VERBOSE_METHODDEF
    {NULL, NULL, 0, NULL},
};

static struct PyModuleDef _cdsmodule = {
    PyModuleDef_HEAD_INIT,
    "_cds",
    NULL,
    -1,
    CDSMethods
};

static PyStructSequence_Field flags_fields[] = {
    {"mode",                "PYCDSMODE"},
    {"verbose",             "PYCDSVERBOSE"},
    {0}
};

static PyStructSequence_Desc flags_desc = {
    "_cds.flags",
    NULL,
    flags_fields,
    2
};

// todo: not implemented yet
//PyMemAllocatorEx CDSAllocator = {&cds_status,
//    PyCDS_AllocatorMalloc,
//    PyCDS_AllocatorCalloc,
//    PyCDS_AllocatorRealloc,
//    PyCDS_AllocatorFree
//};
// clang-format on

static PyTypeObject FlagsType;
static PyObject *CDSException;

#define set_cds_exception_from_format(...)                \
    do {                                                  \
        PyObject *_s = PyUnicode_FromFormat(__VA_ARGS__); \
        PyErr_SetObject(CDSException, _s);                \
        Py_DECREF(_s);                                    \
    } while (0)

struct CDSStatus cds_status;

PyMODINIT_FUNC
PyInit__cds(void)
{
    PyObject *m;
    int res;

    m = PyModule_Create(&_cdsmodule);
    if (m == NULL)
        return NULL;

    if (FlagsType.tp_name == 0) {
        res = PyStructSequence_InitType2(&FlagsType, &flags_desc);
        assert(res >= 0);
    }

    cds_status.flags = Py_NewRef(PyStructSequence_New(&FlagsType));

    Py_XDECREF(PyStructSequence_GET_ITEM(cds_status.flags, 0));
    PyStructSequence_SET_ITEM(cds_status.flags, 0, PyLong_FromLong(0));
    Py_XDECREF(PyStructSequence_GET_ITEM(cds_status.flags, 1));
    PyStructSequence_SET_ITEM(cds_status.flags, 1, PyLong_FromLong(0));

    PyObject *d = PyModule_GetDict(m);

    CDSException =
        PyErr_NewException("_cds.CDSException", PyExc_RuntimeError, NULL);
    Py_INCREF(CDSException);
    res = PyDict_SetItemString(d, "CDSException", CDSException);
    if (res < 0) {
        PyErr_SetString(PyExc_RuntimeError, "init failed of _cds module.");
        return NULL;
    }

    res = PyDict_SetItemString(d, "flags", cds_status.flags);
    Py_DECREF(cds_status.flags);
    if (res < 0) {
        PyErr_SetString(CDSException, "init failed of _cds.flags.");
        return NULL;
    }
    return m;
}

/*[clinic input]
_cds._create_archive

    archive: str

[clinic start generated code]*/

static PyObject *
_cds__create_archive_impl(PyObject *module, const char *archive)
/*[clinic end generated code: output=3cfd54cfec16be94 input=374154f19d6a118a]*/
{
    if (PyCDS_SetInitializedWithMode(CDS_MODE_DUMP_ARCHIVE) == NULL) {
        return NULL;
    }
    if (PyCDS_CreateArchive(archive) == NULL) {
        return NULL;
    }
    return Py_NewRef(Py_None);
}

/*[clinic input]
_cds._load_archive

    archive: str

[clinic start generated code]*/

static PyObject *
_cds__load_archive_impl(PyObject *module, const char *archive)
/*[clinic end generated code: output=5838740b6db38d96 input=94f6b2c07d205bf5]*/
{
    if (PyCDS_SetInitializedWithMode(CDS_MODE_SHARE) == NULL) {
        return NULL;
    }
    if (PyCDS_LoadArchive(archive) == NULL) {
        return NULL;
    }
    return Py_NewRef(Py_None);
}

/*[clinic input]
_cds._move_in

    op: object

[clinic start generated code]*/

static PyObject *
_cds__move_in_impl(PyObject *module, PyObject *op)
/*[clinic end generated code: output=a5166107eeb0f42e input=87375f3d38d58acb]*/
{
    if ((cds_status.mode & CDS_MODE_DUMP_ARCHIVE) != CDS_MODE_DUMP_ARCHIVE) {
        return NULL;
    }
    else if (cds_status.archive_fd == 0) {
        PyErr_SetString(
            CDSException,
            "move in already finished or archive file not opened.");
        return NULL;
    }
    PyCDS_InitMoveIn();

    PYCDS_MOVEIN_IMPL(op, &cds_status.archive_header->obj);

    PyCDS_FinalizeMoveIn();

    ftruncate(cds_status.archive_fd, cds_status.archive_header->used);
    close(cds_status.archive_fd);
    cds_status.archive_fd = 0;
    if (cds_status.traverse_error) {
        return NULL;
    }

    return Py_NewRef(Py_None);
}

/*[clinic input]
_cds._get_obj

[clinic start generated code]*/

static PyObject *
_cds__get_obj_impl(PyObject *module)
/*[clinic end generated code: output=720ed26737aac9da input=57fd8d7756484d04]*/
{
    if (cds_status.archive_header->obj == NULL) {
        PyErr_SetString(CDSException, "No object in heap.");
        return NULL;
    }
    else {
        return Py_NewRef(cds_status.archive_header->obj);
    }
}

/*[clinic input]
_cds._set_mode

    mode: int

Set mode for only tracer and manual mode, from cds module.

Other mode will be set by extension module.
[clinic start generated code]*/

static PyObject *
_cds__set_mode_impl(PyObject *module, int mode)
/*[clinic end generated code: output=ba43df118acf9262 input=d774e66ec2dea0be]*/
{
    if (mode != CDS_MODE_DISABLED && mode != CDS_MODE_DUMP_LIST &&
        mode != CDS_MODE_DUMP_ARCHIVE && mode != CDS_MODE_MANUALLY) {
        set_cds_exception_from_format(
            "invalid mode: %d, only disabled (%d), tracer (%d), dumper (%d) "
            "and manual mode (%d) can be set with _cds._set_mode.",
            mode, CDS_MODE_DISABLED, CDS_MODE_DUMP_LIST, CDS_MODE_DUMP_ARCHIVE,
            CDS_MODE_MANUALLY);
        return NULL;
    }
    return PyCDS_SetInitializedWithMode(mode);
}

/*[clinic input]
_cds._get_initialized -> bool

Get if cds module is already initialized.
[clinic start generated code]*/

static int
_cds__get_initialized_impl(PyObject *module)
/*[clinic end generated code: output=6c0313e5323f4559 input=88a1b1ce00be4d85]*/
{
    return cds_status.initialized;
}

/*[clinic input]
_cds._set_verbose

    mode: int

Set verbosity and return old verbosity.
[clinic start generated code]*/

static PyObject *
_cds__set_verbose_impl(PyObject *module, int mode)
/*[clinic end generated code: output=596ccbc0b7f73deb input=40bf9611d44eec2e]*/
{
    return PyCDS_SetVerbose(mode);
}

void *
PyCDS_CreateArchive(const char *archive)
{
    if (cds_status.archive != NULL || cds_status.archive_fd != 0 ||
        cds_status.archive_header != NULL) {
        PyErr_SetString(CDSException, "archive already initialized.");
        return NULL;
    }

    cds_status.archive = archive;
    cds_status.archive_fd = open(archive, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (cds_status.archive_fd < 0) {
        PyErr_SetString(CDSException, "create mmap file failed.");
        return NULL;
    }
    ftruncate(cds_status.archive_fd, CDS_MAX_IMG_SIZE);
    void *shm =
        mmap(CDS_REQUESTING_ADDR, CDS_MAX_IMG_SIZE, PROT_READ | PROT_WRITE,
             MAP_SHARED, cds_status.archive_fd, 0);
    if (shm == MAP_FAILED) {
        PyErr_SetString(CDSException, "mmap failed.");
        return NULL;
    }
    cds_status.archive_header = (struct CDSArchiveHeader *)shm;
    cds_status.archive_header->mapped_addr = shm;
    cds_status.archive_header->none_addr = Py_None;
    cds_status.archive_header->true_addr = Py_True;
    cds_status.archive_header->false_addr = Py_False;
    cds_status.archive_header->ellipsis_addr = Py_Ellipsis;
    cds_status.archive_header->used =
        ALIEN_TO(sizeof(struct CDSArchiveHeader), 8);
    cds_status.archive_header->all_string_ref = NULL;
    return shm;
}

void *
PyCDS_Malloc(size_t size)
{
    cds_status.move_in_ctx->n_alloc++;
    if (!size)
        size = 1;
    // Extra memory for (internal) PyGC_Head,
    // otherwise previous object will be overwritten by GC.
    // Objects in CPython are not compact.
    cds_status.archive_header->used += sizeof(PyGC_Head);
    size_t size_aligned = ALIEN_TO(size, 8);
    void *res =
        ((char *)cds_status.archive_header) + cds_status.archive_header->used;
    if ((cds_status.archive_header->used += size_aligned) > CDS_MAX_IMG_SIZE) {
        cds_status.archive_header->used -= sizeof(PyGC_Head);
        cds_status.archive_header->used -= size_aligned;
        return NULL;
    }
    PyCDS_Verbose(2, "Malloc: [%p, %p)", res, res + size_aligned);
    return res;
}

void
PyCDS_Free(void *p)
{
    // do nothing for now.
    assert(PyCDS_InHeap(p));
    PyCDS_Verbose(2, "Free: %p", p);
}
bool
PyCDS_InHeap(void *p)
{
    if (cds_status.archive_header) {
        if (p > cds_status.archive_header->mapped_addr &&
            p < cds_status.archive_header->mapped_addr +
                    cds_status.archive_header->used)
            return true;
    }
    return false;
}

void *
PyCDS_LoadArchive(const char *archive)
{
    if (cds_status.archive_header != NULL) {
        PyErr_SetString(CDSException, "archive already loaded.");
        return NULL;
    }

    PyCDS_Verbose(1, "opening archive %s", archive);

    cds_status.archive = archive;
    cds_status.archive_fd = open(archive, O_RDWR);
    if (cds_status.archive_fd < 0) {
        PyErr_SetString(CDSException, "open mmap file failed.");
        goto fail;
    }

    struct CDSArchiveHeader h;
    if (read(cds_status.archive_fd, &h, sizeof(h)) != sizeof(h)) {
        PyErr_SetString(CDSException, "read archive header failed.");
        goto fail;
    }

    PyCDS_Verbose(2, "requesting %p...", h.mapped_addr);
    size_t aligned_size = ALIEN_TO(h.used, 4096);
    void *shm =
        mmap(h.mapped_addr, aligned_size, PROT_READ | PROT_WRITE,
             MAP_PRIVATE | MAP_FIXED | M_POPULATE, cds_status.archive_fd, 0);
    if (shm == MAP_FAILED) {
        PyErr_SetString(CDSException, "mmap failed.");
        goto fail;
    }
    else if (shm != h.mapped_addr) {
        PyErr_SetString(CDSException, "mmap relocated.");
        goto fail;
    }
    cds_status.archive_header = (struct CDSArchiveHeader *)shm;
    close(cds_status.archive_fd);
    cds_status.archive_fd = 0;

#if M_POPULATE == 0
    for (size_t i = 0; i < cds_status.archive_header->used; i += 4096) {
        ((char volatile *)shm)[i] += 0;
    }
#endif

    if (cds_status.archive_header->none_addr) {
        cds_status.shift =
            (void *)Py_None - (void *)cds_status.archive_header->none_addr;
    }
    if (cds_status.archive_header->obj != NULL) {
        assert(!cds_status.traverse_error);
        PyCDS_PatchPyObject(&cds_status.archive_header->obj);
        // if patch failed, Python exception is set
        if (cds_status.traverse_error) {
            return NULL;
        }

#ifdef INTERN_HEAP_STRING
        struct StringRefList *str_refs =
            cds_status.archive_header->all_string_ref;
        while (str_refs != NULL) {
            if (PyCDS_STR_INTERNED(str_refs->str)) {
                PyObject *heap_str = str_refs->str;
                PyObject *borrowed_str = str_refs->str;
                PyCDS_Verbose(2, "check string interns at %p.", str_refs->str);
                // temporary assign to not interned, for following intern.
                PyCDS_STR_INTERNED(str_refs->str) = SSTATE_NOT_INTERNED;
                PyUnicode_InternInPlace(&borrowed_str);
                if (borrowed_str != heap_str) {
                    // an extra decref is done in previous
                    // PyUnicode_InternInPlace, so we start from -1 here.
                    size_t ref_count = -1;

                    PyCDS_Verbose(
                        2, "string already interned, updating in-heap refs.");
                    // reassign references
                    struct StringRefItem *ref = str_refs->refs;
                    while (ref != NULL) {
                        ref_count++;
                        *(ref->ref) = borrowed_str;
                        ref = ref->next;
                    }

                    Py_SET_REFCNT(heap_str, Py_REFCNT(heap_str) - ref_count);
                    Py_SET_REFCNT(borrowed_str,
                                  Py_REFCNT(borrowed_str) + ref_count);

                    // rc >= 1 for in-heap references (e.g. from tuple) to str.
                    assert(Py_REFCNT(heap_str) >= 1);
                }
                PyCDS_Verbose(2, "string singleton @ %p.", borrowed_str);
            }

            str_refs = str_refs->next;
        }
#endif
    }

    return shm;

fail:
    close(cds_status.archive_fd);
    cds_status.archive_fd = 0;
    return NULL;
}

void
PyCDS_InitMoveIn()
{
    assert(cds_status.move_in_ctx == NULL);
    cds_status.move_in_ctx = malloc(sizeof(struct MoveInContext));
    cds_status.move_in_ctx->map_orig_pyobject_to_in_heap_pyobject =
        PyCDS_Table_New();
    cds_status.move_in_ctx->map_in_heap_str_to_string_ref_list =
        PyCDS_Table_New();
}
void
PyCDS_FinalizeMoveIn()
{
    assert(cds_status.move_in_ctx != NULL);

    PyCDS_Table_Destroy(
        cds_status.move_in_ctx->map_orig_pyobject_to_in_heap_pyobject);
    PyCDS_Table_Destroy(
        cds_status.move_in_ctx->map_in_heap_str_to_string_ref_list);
    free(cds_status.move_in_ctx);
}

/**
 * Set Python exception and return NULL if error occured.
 */
void
PyCDS_MoveInRec(PyObject *op, PyObject **target)
{
    *target = NULL;
    if (op == NULL) {
        return;
    }
    assert(!cds_status.traverse_error);
    PyTypeObject *ty = Py_TYPE(op);

    PyCDS_Verbose(2, "move %s@%p into %p", Py_TYPE(op)->tp_name, op, target);

#define UNTRACK(obj)                \
    do {                            \
        /* Extra refcnt to          \
         * avoid `tp_dealloc` */    \
        Py_SET_REFCNT((obj), 1);    \
        PyObject_GC_UnTrack((obj)); \
    } while (0)

#define SIMPLE_MOVE_IN(obj_type, type, copy)            \
    obj_type *res = PyCDS_Malloc(_PyObject_SIZE(type)); \
    PyObject_INIT(res, type);                           \
    do {                                                \
        copy                                            \
    } while (0);                                        \
    *target = (PyObject *)res;                          \
    UNTRACK(*target);

    if (ty == &PyBool_Type || ty == &_PyNone_Type || ty == &PyEllipsis_Type) {
        *target = op;
    }
    else if (ty == &PyBytes_Type) {
        // PyBytesObject_SIZE
        Py_ssize_t size = Py_SIZE(op);
        PyBytesObject *res = (PyBytesObject *)PyCDS_Malloc(
            offsetof(PyBytesObject, ob_sval) + 1 + size);

        (void)PyObject_INIT_VAR(res, &PyBytes_Type, size);
#if PY_MINOR_VERSION < 11
        res->ob_shash = -1;
#endif
        memcpy(res->ob_sval, ((PyBytesObject *)op)->ob_sval, size + 1);

        *target = (PyObject *)res;
        UNTRACK(*target);
    }
    else if (ty == &PyComplex_Type) {
        SIMPLE_MOVE_IN(PyComplexObject, &PyComplex_Type,
                       { res->cval = ((PyComplexObject *)op)->cval; })
    }
    else if (ty == &PyFloat_Type) {
        SIMPLE_MOVE_IN(PyFloatObject, &PyFloat_Type,
                       { res->ob_fval = ((PyFloatObject *)op)->ob_fval; })
    }
    else if (ty == &PyLong_Type) {
        // todo: also support small ints
        // but small int doesn't have public API.

        // _PyLong_Copy starts
        PyLongObject *src = (PyLongObject *)op;
        Py_ssize_t size = Py_SIZE(src);
        if (size < 0)
            size = -(size);

        // _PyLong_New starts
        PyLongObject *res = PyCDS_Malloc(offsetof(PyLongObject, ob_digit) +
                                         size * sizeof(digit));
        PyObject_INIT_VAR((PyVarObject *)res, &PyLong_Type, Py_SIZE(src));
        // _PyLong_New ends

        while (--size >= 0) {
            res->ob_digit[size] = src->ob_digit[size];
        }
        // _PyLong_Copy ends

        *target = (PyObject *)res;
        UNTRACK(*target);
    }
    else if (ty == &PyUnicode_Type) {
        PyObject *res;

        if ((res = PyCDS_Table_Get(
                 cds_status.move_in_ctx->map_orig_pyobject_to_in_heap_pyobject,
                 op)) != NULL) {
            *target = res;
            Py_INCREF(*target);
        }
        else {
            // basically copied from unicodeobject.c, todo: optimize

            // _PyUnicode_Copy starts
            Py_ssize_t size = PyUnicode_GET_LENGTH(op);
            Py_UCS4 maxchar = PyUnicode_MAX_CHAR_VALUE(op);

            // PyUnicode_New(Py_ssize_t size, Py_UCS4 maxchar) starts
            PyCompactUnicodeObject *unicode;
            void *data;
            enum PyUnicode_Kind kind;
            int is_sharing, is_ascii;
            Py_ssize_t char_size;
            Py_ssize_t struct_size;

            is_ascii = 0;
            is_sharing = 0;
            struct_size = sizeof(PyCompactUnicodeObject);
            if (maxchar < 128) {
                kind = PyUnicode_1BYTE_KIND;
                char_size = 1;
                is_ascii = 1;
                struct_size = sizeof(PyASCIIObject);
            }
            else if (maxchar < 256) {
                kind = PyUnicode_1BYTE_KIND;
                char_size = 1;
            }
            else if (maxchar < 65536) {
                kind = PyUnicode_2BYTE_KIND;
                char_size = 2;
                if (sizeof(wchar_t) == 2)
                    is_sharing = 1;
            }
            else {
                kind = PyUnicode_4BYTE_KIND;
                char_size = 4;
                if (sizeof(wchar_t) == 4)
                    is_sharing = 1;
            }

            res =
                (PyObject *)PyCDS_Malloc(struct_size + (size + 1) * char_size);
            PyObject_Init(res, &PyUnicode_Type);

            unicode = (PyCompactUnicodeObject *)res;
            if (is_ascii)
                data = ((PyASCIIObject *)res) + 1;
            else
                data = unicode + 1;
            (((PyASCIIObject *)(unicode))->length) = size;
            (((PyASCIIObject *)(unicode))->hash) = -1;
            // used to determine strings to be interned when loading archive
            PyCDS_STR_INTERNED(unicode) = PyCDS_STR_INTERNED(op);
            (((PyASCIIObject *)(unicode))->state).kind = kind;
            (((PyASCIIObject *)(unicode))->state).compact = 1;
            (((PyASCIIObject *)(unicode))->state).ready = 1;
            (((PyASCIIObject *)(unicode))->state).ascii = is_ascii;
            if (is_ascii) {
                ((char *)data)[size] = 0;
                (((PyASCIIObject *)(unicode))->wstr) = NULL;
            }
            else if (kind == PyUnicode_1BYTE_KIND) {
                ((char *)data)[size] = 0;
                (((PyASCIIObject *)(unicode))->wstr) = NULL;
                (((PyCompactUnicodeObject *)(unicode))->wstr_length) = 0;
                unicode->utf8 = NULL;
                unicode->utf8_length = 0;
            }
            else {
                unicode->utf8 = NULL;
                unicode->utf8_length = 0;
                if (kind == PyUnicode_2BYTE_KIND)
                    ((Py_UCS2 *)data)[size] = 0;
                else /* kind == PyUnicode_4BYTE_KIND */
                    ((Py_UCS4 *)data)[size] = 0;
                if (is_sharing) {
                    (((PyCompactUnicodeObject *)(unicode))->wstr_length) =
                        size;
                    (((PyASCIIObject *)(unicode))->wstr) = (wchar_t *)data;
                }
                else {
                    (((PyCompactUnicodeObject *)(unicode))->wstr_length) = 0;
                    (((PyASCIIObject *)(unicode))->wstr) = NULL;
                }
            }
            // PyUnicode_New(Py_ssize_t size, Py_UCS4 maxchar) ends

            memcpy(PyUnicode_DATA(res), PyUnicode_DATA(op),
                   size * PyUnicode_KIND(op));
            // _PyUnicode_Copy ends

            *target = res;
            UNTRACK(*target);

            PyCDS_Table_Insert(
                cds_status.move_in_ctx->map_orig_pyobject_to_in_heap_pyobject,
                op, *target);
        }

        struct StringRefList *str_refs = PyCDS_Table_Get(
            cds_status.move_in_ctx->map_in_heap_str_to_string_ref_list, res);

        if (str_refs == NULL) {
            PyCDS_Verbose(2, "create identity in table: %p", res);
            str_refs = PyCDS_Malloc(sizeof(struct StringRefList));
            str_refs->str = res;
            str_refs->refs = NULL;
            str_refs->next = cds_status.archive_header->all_string_ref;
            cds_status.archive_header->all_string_ref = str_refs;

            if (PyCDS_STR_INTERNED(res)) {
                // extra rc to prevent being freed when interning
                Py_INCREF(res);
            }
            PyCDS_Table_Insert(
                cds_status.move_in_ctx->map_in_heap_str_to_string_ref_list,
                res, str_refs);
        }
        struct StringRefItem *n = PyCDS_Malloc(sizeof(struct StringRefItem));
        n->ref = target;
        n->next = str_refs->refs;
        str_refs->refs = n;
    }
    else if (ty == &PyTuple_Type || ty == &PyFrozenSet_Type) {
        // PyTuple_New starts
        PyTupleObject *res;
        PyTupleObject *src;
        if (ty != &PyTuple_Type)
            src = (PyTupleObject *)PySequence_Tuple(op);
        else
            src = (PyTupleObject *)op;
        Py_ssize_t nitems = PyTuple_Size((PyObject *)src);

        // tuple_alloc & _PyObject_GC_NewVar starts
        size_t var_size = _PyObject_VAR_SIZE(&PyTuple_Type, nitems);

        PyVarObject *var = (PyVarObject *)PyCDS_Malloc(var_size);
        PyObject_INIT_VAR(var, &PyTuple_Type, nitems);
        // tuple_alloc & _PyObject_GC_NewVar ends

        res = (PyTupleObject *)var;

        for (Py_ssize_t i = 0; i < nitems; i++) {
            PYCDS_MOVEIN_REC_RETURN(src->ob_item[i], &res->ob_item[i]);
            Py_INCREF(res->ob_item[i]);
        }
        *target = (PyObject *)res;
        UNTRACK(*target);
    }
    else if (ty == &PyCode_Type) {
        PyCodeObject *src = (PyCodeObject *)op;

#if PY_MINOR_VERSION < 11
        Py_ssize_t *cell2arg = NULL;
        int n_cellvars = PyTuple_GET_SIZE(src->co_cellvars);
        if (n_cellvars && src->co_cell2arg) {
            cell2arg = PyCDS_Malloc(n_cellvars * sizeof(Py_ssize_t));
            for (int i = 0; i < n_cellvars; i++) {
                cell2arg[i] = src->co_cell2arg[i];
            }
        }
#endif

        PyCodeObject *res;
#if PY_MINOR_VERSION >= 11
        Py_ssize_t code_count, code_size;
        code_count = ((PyVarObject *)src)->ob_size;
        code_size = code_count * sizeof(_Py_CODEUNIT);
        res = (PyCodeObject *)PyCDS_Malloc(
            offsetof(PyCodeObject, co_code_adaptive) + code_size);
        Py_SET_SIZE(res, code_count);
#else
        res = (PyCodeObject *)PyCDS_Malloc(_PyObject_SIZE(&PyCode_Type));
#endif
        PyObject_INIT(res, &PyCode_Type);

#define SIMPLE_HANDLER(field)    \
    do {                         \
        res->field = src->field; \
    } while (0)
#define PATCH_HANDLER(field) PYCDS_MOVEIN_REC_RETURN(src->field, &res->field)
        UNWIND_CODE_FIELDS
#undef PATCH_HANDLER
#undef SIMPLE_HANDLER

        res->co_weakreflist = NULL;
        res->co_extra = NULL;

#if PY_MINOR_VERSION >= 11
        res->co_warmup = QUICKENING_INITIAL_WARMUP_VALUE;

        _Py_CODEUNIT *instructions = _PyCode_CODE(res);
        memcpy(instructions, src->co_code_adaptive, code_size);

        // codeobject.c:deopt_code()
        for (int i = 0; i < code_count; ++i) {
            _Py_CODEUNIT instruction = instructions[i];
            int opcode = _PyOpcode_Original[_Py_OPCODE(instruction)];
            int caches = _PyOpcode_Caches[opcode];
            instructions[i] = _Py_MAKECODEUNIT(opcode, _Py_OPARG(instruction));
            while (caches--) {
                instructions[++i] = _Py_MAKECODEUNIT(CACHE, 0);
            }
        }
#else  // PY_MINOR_VERSION < 11
        res->co_cell2arg = cell2arg;

        res->co_opcache_map = NULL;
        res->co_opcache = NULL;
        res->co_opcache_flag = 0;
        res->co_opcache_size = 0;
#if PY_MINOR_VERSION < 10
        res->co_zombieframe = NULL;
#endif
#endif

        *target = (PyObject *)res;
        UNTRACK(*target);
    }
    else {
        PyErr_SetObject(CDSException,
                        PyUnicode_FromFormat("Trying to move in a %s object.",
                                             ty->tp_name));
        cds_status.traverse_error = true;
        return;
    }

#undef UNTRACK
}

void
PyCDS_PatchPyObject(PyObject **ref)
{
#ifdef FAST_PATCH
    if (cds_status.shift == 0) {
        return;
    }
#endif
    PyObject *op = *ref;
    if (op == NULL) {
        return;
    }
    else if (op == cds_status.archive_header->none_addr ||
             op == cds_status.archive_header->true_addr ||
             op == cds_status.archive_header->false_addr ||
             op == cds_status.archive_header->ellipsis_addr) {
        PyCDS_Verbose(2, "patching basic types.");
        *ref = UNSHIFT(op, cds_status.shift, PyObject);
    }
    else if (Py_TYPE(op) == &PyUnicode_Type) {
        PyCDS_Verbose(2, "string singleton already patched.");
    }
    else {
        PyTypeObject *ty =
            UNSHIFT(Py_TYPE(op), cds_status.shift, PyTypeObject);

        if (/* numbers */ ty == &PyComplex_Type || ty == &PyLong_Type ||
            ty == &PyFloat_Type ||
            /* strings */ ty == &PyBytes_Type || ty == &PyUnicode_Type) {
            // simple patch
            Py_SET_TYPE(*ref, ty);
        }
        else if (ty == &PyTuple_Type) {
            Py_SET_TYPE(op, &PyTuple_Type);
            PyTupleObject *tuple_op = (PyTupleObject *)op;
            for (Py_ssize_t i = Py_SIZE(tuple_op); --i >= 0;) {
                PYCDS_PATCHPYOBJECT_RETURN(&tuple_op->ob_item[i]);
            }
        }
        else if (ty == &PyCode_Type) {
            Py_SET_TYPE(op, &PyCode_Type);
            PyCodeObject *code_op = (PyCodeObject *)op;

#define SIMPLE_HANDLER(field)
#define PATCH_HANDLER(field) PYCDS_PATCHPYOBJECT_RETURN(&code_op->field)
            UNWIND_CODE_FIELDS
#undef PATCH_HANDLER
#undef SIMPLE_HANDLER
        }
        else {
            PyErr_SetString(CDSException, "cannot patch type.");
            cds_status.traverse_error = true;
        }
    }
}

void
PyCDS_Verbose(int verbosity, const char *fmt, ...)
{
    if (cds_status.verbose >= verbosity) {
        va_list arg;
        va_start(arg, fmt);
        fprintf(stderr, "[cds] ");
        vfprintf(stderr, fmt, arg);
        fprintf(stderr, "\n");
        va_end(arg);
        fflush(stderr);
    }
}

PyObject *
PyCDS_SetInitializedWithMode(int new_flag)
{
    if (!(new_flag == CDS_MODE_DISABLED || new_flag == CDS_MODE_DUMP_ARCHIVE ||
          new_flag == CDS_MODE_DUMP_LIST || new_flag == CDS_MODE_SHARE ||
          new_flag == CDS_MODE_MANUALLY)) {
        set_cds_exception_from_format("invalid mode: %d.", new_flag);
        return NULL;
    }
    else if (cds_status.initialized) {
        if (cds_status.mode == CDS_MODE_MANUALLY ||
            (cds_status.mode == CDS_MODE_DISABLED &&
             new_flag == CDS_MODE_DUMP_ARCHIVE)) {
            // good, supposed to change mode after initialization
            PyCDS_Verbose(1, "change mode after initialization");
        }
        else {
            set_cds_exception_from_format(
                "cds already initialized, current mode: %d.", cds_status.mode);
            return NULL;
        }
    }
    else {
        cds_status.initialized = true;
    }

    cds_status.mode = new_flag;
    Py_XDECREF(PyStructSequence_GET_ITEM(cds_status.flags, 0));
    PyStructSequence_SET_ITEM(cds_status.flags, 0, PyLong_FromLong(new_flag));
    return Py_NewRef(Py_None);
}

PyObject *
PyCDS_SetVerbose(int new_flag)
{
    if (!(new_flag >= 0 && new_flag <= 2)) {
        set_cds_exception_from_format("invalid verbose: %d.", new_flag);
        return NULL;
    }
    cds_status.verbose = new_flag;
    Py_XDECREF(PyStructSequence_GET_ITEM(cds_status.flags, 1));
    PyStructSequence_SET_ITEM(cds_status.flags, 1, PyLong_FromLong(new_flag));
    return Py_NewRef(Py_None);
}
