#ifndef PYCDS__CDSCONTEXT_H
#define PYCDS__CDSCONTEXT_H

#include <Python.h>
#include <stdbool.h>

#include "lookup_table.h"
#include "platforms.h"

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
    int n_alloc;

    table *orig_pyobject_to_in_heap_pyobject_map;
    table *in_heap_str_to_string_ref_list_map;

#if PY_MINOR_VERSION >= 12
    PyObject *static_strings;
#endif
};

struct CDSStatus {
    int verbose;
    int mode;

    // Prevent re-entry of cds.init_from_env()
    bool initialized;

    long shift;
    bool traverse_error;

    const char *archive;
    fd_type archive_fd;
#if IS_WINDOWS
    fd_type mapping;
#endif

    struct CDSArchiveHeader *archive_header;

    PyObject *flags;

    struct MoveInContext *move_in_ctx;
};

#endif  // PYCDS__CDSCONTEXT_H
