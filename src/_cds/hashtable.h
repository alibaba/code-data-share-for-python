#ifndef PYCDS_HASHTABLE_H
#define PYCDS_HASHTABLE_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct hashtable_entry_t {
    struct hashtable_entry_t *next;
    size_t key_hash;
    void *key;
    void *value;
} hashtable_entry_t;

typedef struct {
    hashtable_entry_t *head;
} hashtable_entry_list;

typedef struct {
    size_t nentries;  // Total number of entries in the table
    size_t nbuckets;
    hashtable_entry_list *buckets;
} hashtable_t;

hashtable_t *
hashtable_new();

int
hashtable_set(hashtable_t *ht, const void *key, void *value);

void *
hashtable_get(hashtable_t *ht, const void *key);

void
hashtable_destroy(hashtable_t *ht);

static size_t
_hash_func_impl(const void *p);

static int
_hashtable_rehash(hashtable_t *ht);

#endif
