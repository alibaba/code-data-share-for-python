#ifndef PYCDS__CDSMODULE_H
#define PYCDS__CDSMODULE_H

#include <Python.h>
// sizeof(PyGC_Head)
#if PY_VERSION_HEX >= 0x03090000
#include <internal/pycore_gc.h>
#elif PY_VERSION_HEX >= 0x03080000
#include <objimpl.h>
#endif

#if PY_VERSION_HEX < 0x03080000
#error Requires CPython 3.8+.
#endif

#include <stdbool.h>

#include "clinic/_cdsmodule.c.h"
#include "lookup_table.h"
#include "pythoncapi_compat.h"

#define CDS_MAX_IMG_SIZE (1024 * 1024 * 1024)
#define CDS_REQUESTING_ADDR ((void *)0x280000000L)

#define FAST_PATCH
#define INTERN_HEAP_STRING

struct StringRefItem {
    PyObject **ref;
    struct StringRefItem *next;
};

struct StringRefList {
    PyObject *str;
    struct StringRefItem *refs;
    struct StringRefList *next;
};

struct CDSArchiveHeader {
    void *mapped_addr;
    void *none_addr;
    void *true_addr;
    void *false_addr;
    void *ellipsis_addr;
    size_t used;

    PyObject *obj;

    struct StringRefList *all_string_ref;
};

struct MoveInContext {
    //    int serialized_count;
    //    HeapSerializedObject *serialized_array;

    int n_alloc;

    table *map_orig_pyobject_to_in_heap_pyobject;
    table *map_in_heap_str_to_string_ref_list;
};

/*
 * Internal representation of PYCDSMODE.
 * See `config_read_env_vars` for detail.
 *
 * 0: disable
 * 1: dumped name list (from PYDUMPMODULELIST)
 * 2: dumped archive from name list (PYCDSMODE=DUMP, from PYCDSLIST &
 * PYCDSARCHIVE)
 * 3: use archive (PYCDSMODE=SHARE, from PYCDSARCHIVE) (for debug
 * and test only)
 * -1: manually select mode in user's code (PYCDSMODE=MANUALLY)
 */

#define CDS_MODE_DISABLED (0)
#define CDS_MODE_DUMP_LIST (1)
#define CDS_MODE_DUMP_ARCHIVE (2)
#define CDS_MODE_SHARE (3)
#define CDS_MODE_MANUALLY (-1)

#define ALIEN_TO(size, align) (((size) + ((align)-1)) & ~((align)-1))
#define UNSHIFT(p, shift, ty) ((ty *)((void *)(p) + (shift)))

struct CDSStatus {
    int verbose;
    int mode;

    // Prevent re-entry of cds.init_from_env()
    bool initialized;

    long shift;
    bool traverse_error;

    const char *archive;
    int archive_fd;

    struct CDSArchiveHeader *archive_header;

    PyObject *flags;

    struct MoveInContext *move_in_ctx;
};

void *PyCDS_Malloc(size_t);
void
PyCDS_Free(void *);
bool
PyCDS_InHeap(void *);

// for PyMemAllocatorEx
void *
PyCDS_AllocatorMalloc(void *, size_t);
void *
PyCDS_AllocatorCalloc(void *, size_t, size_t);
void *
PyCDS_AllocatorRealloc(void *, void *, size_t);
void
PyCDS_AllocatorFree(void *, void *);

PyAPI_DATA(struct CDSStatus) cds_status;

PyMODINIT_FUNC
PyInit__cds(void);

void *
PyCDS_CreateArchive(const char *archive);

void *
PyCDS_LoadArchive(const char *archive);

void
PyCDS_InitMoveIn();
void
PyCDS_MoveInRec(PyObject *op, PyObject **target);
void
PyCDS_FinalizeMoveIn();

// we want to use different implementation of moving-in,
// e.g. manually copy, patching python's allocator
#define PYCDS_MOVEIN_IMPL PyCDS_MoveInRec

#define PYCDS_MOVEIN_REC_RETURN(src, target) \
    do {                                     \
        PYCDS_MOVEIN_IMPL((src), (target));  \
        if (cds_status.traverse_error) {     \
            return;                          \
        }                                    \
    } while (0)

void
PyCDS_PatchPyObject(PyObject **ref);
#define PYCDS_PATCHPYOBJECT_RETURN(ref)  \
    do {                                 \
        PyCDS_PatchPyObject((ref));      \
        if (cds_status.traverse_error) { \
            return;                      \
        }                                \
    } while (0)

void
PyCDS_Verbose(int verbosity, const char *fmt, ...);

PyObject *
PyCDS_SetInitializedWithMode(int new_flag);

PyObject *
PyCDS_SetVerbose(int new_flag);

// after PEP 674 -- Disallow using macros as l-value
// is accepted we might need to change this
#define PyCDS_STR_INTERNED(op) PyUnicode_CHECK_INTERNED(op)

// Formatter doesn't work well on #if inside #define,
// so we use following function-like masks.
#if PY_VERSION_HEX >= 0x030A0000
#define IF_10_OR_LATER(xx) xx
#define IF_9_OR_EARLIER(xx)
#else
#define IF_10_OR_LATER(_)
#define IF_9_OR_EARLIER(xx) xx
#endif

#if PY_VERSION_HEX >= 0x030B0000
#define IF_11_OR_LATER(xx) xx
#define IF_10_OR_EARLIER(xx)
#else
#define IF_11_OR_LATER(_)
#define IF_10_OR_EARLIER(xx) xx
#endif

// Follow the order in latest cpython.
#define UNWIND_CODE_FIELDS                             \
    PATCH_HANDLER(co_consts);                          \
    PATCH_HANDLER(co_names);                           \
    PATCH_HANDLER(co_code);                            \
    IF_11_OR_LATER(PATCH_HANDLER(co_exceptiontable);)  \
                                                       \
    SIMPLE_HANDLER(co_flags);                          \
    IF_11_OR_LATER(SIMPLE_HANDLER(co_warmup);)         \
                                                       \
    SIMPLE_HANDLER(co_argcount);                       \
    SIMPLE_HANDLER(co_posonlyargcount);                \
    SIMPLE_HANDLER(co_kwonlyargcount);                 \
    SIMPLE_HANDLER(co_stacksize);                      \
    SIMPLE_HANDLER(co_flags);                          \
    SIMPLE_HANDLER(co_firstlineno);                    \
                                                       \
    IF_11_OR_LATER(PATCH_HANDLER(co_localsplusnames);) \
    IF_11_OR_LATER(PATCH_HANDLER(co_localspluskinds);) \
    PATCH_HANDLER(co_filename);                        \
    PATCH_HANDLER(co_name);                            \
                                                       \
    IF_11_OR_LATER(PATCH_HANDLER(co_qualname);)        \
    IF_10_OR_LATER(PATCH_HANDLER(co_linetable);)       \
    IF_11_OR_LATER(PATCH_HANDLER(co_columntable);)     \
                                                       \
    IF_11_OR_LATER(SIMPLE_HANDLER(co_nlocalsplus);)    \
    SIMPLE_HANDLER(co_nlocals);                        \
    IF_11_OR_LATER(SIMPLE_HANDLER(co_nplaincellvars);) \
    IF_11_OR_LATER(SIMPLE_HANDLER(co_ncellvars);)      \
    IF_11_OR_LATER(SIMPLE_HANDLER(co_nfreevars);)      \
                                                       \
    PATCH_HANDLER(co_varnames);                        \
    PATCH_HANDLER(co_freevars);                        \
    PATCH_HANDLER(co_cellvars);                        \
                                                       \
    /* co_lnotab in 3.9 -> co_linetable after 3.10 */  \
    IF_9_OR_EARLIER(PATCH_HANDLER(co_lnotab);)

#endif  // PYCDS__CDSMODULE_H
