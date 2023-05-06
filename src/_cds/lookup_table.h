#ifndef PYCDS_LOOKUP_TABLE_H
#define PYCDS_LOOKUP_TABLE_H

#include <stddef.h>

#define IS_HASHTABLE 1
// for debugging hash table
#define IS_LINKED_LIST !IS_HASHTABLE

#if IS_HASHTABLE
#include <Python.h>
#endif

#if IS_HASHTABLE
#include <internal/pycore_hashtable.h>
typedef struct _table {
    _Py_hashtable_t *pyHashtable;
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
