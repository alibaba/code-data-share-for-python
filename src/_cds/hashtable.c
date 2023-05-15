#include "lookup_table.h"

#if PYCDS_HASHTABLE

#include "hashtable.h"

#define HASHTABLE_MIN_SIZE 16
#define HASHTABLE_HIGH 0.50
#define HASHTABLE_LOW 0.10
#define HASHTABLE_REHASH_FACTOR 2.0 / (HASHTABLE_LOW + HASHTABLE_HIGH)

/* makes sure the real size of the buckets array is a power of 2 */
static size_t
round_size(size_t s)
{
    size_t i;
    if (s < HASHTABLE_MIN_SIZE)
        return HASHTABLE_MIN_SIZE;
    i = 1;
    while (i < s) i <<= 1;
    return i;
}

static size_t
_hash_func_impl(const void *p)
{
    size_t y = (size_t)p;
    /* bottom 3 or 4 bits are likely to be 0; rotate y by 4 to avoid
       excessive hash collisions for dicts and sets */
    y = (y >> 4) | (y << (8 * sizeof(void *) - 4));
    return (size_t)y;
}

static int
_hashtable_rehash(hashtable_t *ht)
{
    size_t new_nbuckets =
        round_size((size_t)(ht->nentries * HASHTABLE_REHASH_FACTOR));
    if (new_nbuckets == ht->nbuckets) {
        return 0;
    }

    hashtable_entry_list *new_buckets =
        calloc(new_nbuckets, sizeof(ht->buckets[0]));
    if (new_buckets == NULL) {
        return -1;
    }

    for (size_t bucket = 0; bucket < ht->nbuckets; bucket++) {
        hashtable_entry_t *entry = ht->buckets[bucket].head;
        while (entry != NULL) {
            //            assert(ht->hash_func(entry->key) == entry->key_hash);
            hashtable_entry_t *next = entry->next;
            size_t new_index = entry->key_hash & (new_nbuckets - 1);

            // insert new bucket
            entry->next = new_buckets[new_index].head;
            new_buckets[new_index].head = entry;

            // next
            entry = next;
        }
    }

    free(ht->buckets);
    ht->nbuckets = new_nbuckets;
    ht->buckets = new_buckets;
    return 0;
}

hashtable_t *
hashtable_new()
{
    hashtable_t *ht = calloc(1, sizeof(hashtable_t));
    if (ht == NULL) {
        free(ht);
        return NULL;
    }

    ht->nbuckets = HASHTABLE_MIN_SIZE;
    ht->nentries = 0;
    size_t buckets_size = ht->nbuckets * sizeof(ht->buckets[0]);
    ht->buckets = calloc(ht->nbuckets, sizeof(ht->buckets[0]));
    if (ht->buckets == NULL) {
        free(ht->buckets);
        free(ht);
        return NULL;
    }

    return ht;
}

int
hashtable_set(hashtable_t *ht, const void *key, void *value)
{
    hashtable_entry_t *entry = calloc(1, sizeof(hashtable_entry_t));
    if (entry == NULL) {
        return -1;
    }
    entry->key_hash = _hash_func_impl(key);
    entry->key = (void *)key;
    entry->value = value;

    ht->nentries++;
    if ((float)ht->nentries / (float)ht->nbuckets > HASHTABLE_HIGH) {
        if (_hashtable_rehash(ht) < 0) {
            ht->nentries--;
            free(entry);
            return -1;
        }
    }

    size_t index = entry->key_hash & (ht->nbuckets - 1);
    entry->next = ht->buckets[index].head;
    ht->buckets[index].head = entry;

    return 0;
}

void *
hashtable_get(hashtable_t *ht, const void *key)
{
    size_t key_hash = _hash_func_impl(key);
    size_t index = key_hash & (ht->nbuckets - 1);
    hashtable_entry_t *entry = ht->buckets[index].head;
    while (entry != NULL) {
        if (entry->key_hash == key_hash && entry->key == key) {
            break;
        }
        entry = entry->next;
    }

    if (entry != NULL) {
        return entry->value;
    }
    else {
        return NULL;
    }
}

void
hashtable_destroy(hashtable_t *ht)
{
    for (size_t i = 0; i < ht->nbuckets; i++) {
        hashtable_entry_t *entry = ht->buckets[i].head;
        while (entry != NULL) {
            hashtable_entry_t *entry_next = entry->next;
            free(entry);
            entry = entry_next;
        }
    }

    free(ht->buckets);
    free(ht);
}

#endif
