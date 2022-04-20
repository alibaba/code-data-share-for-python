#include "lookup_table.h"

#include <assert.h>
#include <stdlib.h>

table *
PyCDS_Table_New()
{
#if IS_PY_HASHTABLE
    table *t = malloc(sizeof(table));
    t->pyHashtable = _Py_hashtable_new(_Py_hashtable_hash_ptr,
                                       _Py_hashtable_compare_direct);
    return t;
#elif IS_LINKED_LIST
    table *t = malloc(sizeof(table));
    return t;
#endif
}

void
PyCDS_Table_Insert(table *t, void *key, void *value)
{
#if IS_PY_HASHTABLE
    assert(value != NULL);
    _Py_hashtable_set(t->pyHashtable, key, value);
#elif IS_LINKED_LIST
    list_node *n = malloc(sizeof(list_node));
    n->key = key;
    n->value = value;
    n->next = t->head;
    t->head = n;
#endif
}

void *
PyCDS_Table_Get(table *t, void *key)
{
#if IS_PY_HASHTABLE
    return _Py_hashtable_get(t->pyHashtable, key);
#elif IS_LINKED_LIST
    list_node *p = t->head;
    while (p != NULL) {
        if (p->key == key) {
            return p->value;
        }
    }
    return NULL;
#endif
}

void
PyCDS_Table_Destroy(table *t)
{
#if IS_PY_HASHTABLE
    _Py_hashtable_destroy(t->pyHashtable);
    free(t);
#elif IS_LINKED_LIST
    list_node *p = t->head, *p_next;
    while (p != NULL) {
        p_next = p->next;
        free(p);
        p = p_next;
    }
    free(p);
#endif
}
