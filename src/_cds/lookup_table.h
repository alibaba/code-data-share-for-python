#ifndef PYCDS_LOOKUP_TABLE_H
#define PYCDS_LOOKUP_TABLE_H

#include <stddef.h>

#define IS_HASHTABLE 1
// for debugging hash table
#define IS_LINKED_LIST 0

#if IS_HASHTABLE
#include <Python.h>
#if PY_MINOR_VERSION >= 9
#define CPYTHON_HASHTABLE 1
#define PYCDS_HASHTABLE 0
#else
#define CPYTHON_HASHTABLE 0
#define PYCDS_HASHTABLE 1
#endif
#endif

#if IS_HASHTABLE && CPYTHON_HASHTABLE
#include <internal/pycore_hashtable.h>
typedef struct _table {
    _Py_hashtable_t *pyHashtable;
} table;
#elif IS_HASHTABLE && PYCDS_HASHTABLE
#include "hashtable.h"
typedef struct _table {
    hashtable_t *ht;
} table;
#elif IS_LINKED_LIST
typedef struct _list_node {
    void *key;
    void *value;
    struct _list_node *next;
} list_node;
typedef struct _table {
    list_node *head;
} table;
#else
#error "No table implementation"
#endif

table *
PyCDS_Table_New();

void
PyCDS_Table_Insert(table *t, void *key, void *value);

void *
PyCDS_Table_Get(table *t, void *key);

void
PyCDS_Table_Destroy(table *t);

#endif
