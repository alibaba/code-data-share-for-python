#include "lookup_table.h"

#include <assert.h>
#include <stdlib.h>

table *
PyCDS_Table_New()
{
    table *t = calloc(1, sizeof(table));
#if IS_HASHTABLE && CPYTHON_HASHTABLE
    t->pyHashtable = _Py_hashtable_new(_Py_hashtable_hash_ptr,
                                       _Py_hashtable_compare_direct);
#elif IS_HASHTABLE && PYCDS_HASHTABLE
    t->ht = hashtable_new();
#elif IS_LINKED_LIST
#endif
    return t;
}

void
PyCDS_Table_Insert(table *t, void *key, void *value)
{
    assert(value != NULL);
#if IS_HASHTABLE && CPYTHON_HASHTABLE
    _Py_hashtable_set(t->pyHashtable, key, value);
#elif IS_HASHTABLE && PYCDS_HASHTABLE
    hashtable_set(t->ht, key, value);
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
#if IS_HASHTABLE && CPYTHON_HASHTABLE
    return _Py_hashtable_get(t->pyHashtable, key);
#elif IS_HASHTABLE && PYCDS_HASHTABLE
    return hashtable_get(t->ht, key);
#elif IS_LINKED_LIST
    list_node *p = t->head;
    while (p != NULL) {
        if (p->key == key) {
            return p->value;
        }
        p = p->next;
    }
    return NULL;
#endif
}

void
PyCDS_Table_Destroy(table *t)
{
#if IS_HASHTABLE && CPYTHON_HASHTABLE
    _Py_hashtable_destroy(t->pyHashtable);
#elif IS_HASHTABLE && PYCDS_HASHTABLE
    hashtable_destroy(t->ht);
#elif IS_LINKED_LIST
    list_node *p = t->head, *p_next;
    while (p != NULL) {
        p_next = p->next;
        free(p);
        p = p_next;
    }
    free(p);
#endif
    free(t);
}
