#ifndef HT_H
#define HT_H

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// -------------------------------------------------
// --------------- type declarations ---------------
// -------------------------------------------------

typedef struct {
  KEY_TYPE key;
  VAL_TYPE val;
  char occupied;
} KV;

typedef struct {
  KV* items;
  size_t count;
  size_t capacity;
} HT;

// -------------------------------------------------
// ------------- function declarations -------------
// -------------------------------------------------

// creates a new hash table with initial size.
HT ht_init(size_t inital_size);

// frees all memory associated with hash table
void ht_deinit(HT ht);

// insert key-value pair with key and val,
// if an item with the same key exists, its value will be overwritten.
//
// if the hash table is full, it will reallocate, this is quite expensive,
// to counteract this, just increase the initial size in ht_init,
// or define NO_RESIZE, the hash table will then just error.
void ht_insert(HT* ht, KEY_TYPE key, VAL_TYPE val);

// get a reference to the key-value pair with the corresponding key,
// if it does not exist, a new item is inserted.
KV* ht_update(HT* ht, KEY_TYPE key);

// get a reference to the key-value pair with the corresponding key,
// if it does not exist, this function returns NULL.
KV* ht_find(HT ht, KEY_TYPE key);

// deletes key-value pair with corresponding key.
void ht_delete(HT ht, KEY_TYPE key);


// internal function, should not be used,
// changes the size of ht, according to s,
// re-inserts all items.
void __ht_realloc(HT* ht, size_t s);

// -------------------------------------------------
// ------------ function implementation ------------
// -------------------------------------------------

#ifndef HT_NO_IMPLEMENTATION

HT ht_init(size_t initial_size) {
  HT ht = {0};
  ht.capacity = initial_size;
  ht.items = calloc(initial_size, sizeof(KV));
  if(!ht.items) {
    fprintf(stderr, "ERROR: memory allocation failed: %s", strerror(errno));
  }

  return ht;
}

void ht_deinit(HT ht) {
  free(ht.items);
}

void ht_insert(HT* ht, KEY_TYPE key, VAL_TYPE val) {
  if(ht->count == ht->capacity) __ht_realloc(ht, ht->capacity * 2);

  uint64_t h = HASH(key) % ht->capacity;

  while(ht->items[h].occupied && EQ(ht->items[h].key, key) == 0) h = (h + 1) % ht->capacity;

  if(!ht->items[h].occupied) ht->items[h].key = key;
  ht->items[h].occupied = 1;
  ht->items[h].val = val;
  ht->count++;
}

KV* ht_update(HT* ht, KEY_TYPE key) {
  if(ht->count == ht->capacity) __ht_realloc(ht, ht->capacity * 2);

  uint64_t h = HASH(key) % ht->capacity;

  for(size_t i = 0; i < ht->capacity; i++) {
    if(!ht->items[h].occupied) {
      ht->items[h].occupied = 1;
      ht->items[h].key = key;
      ht->count++;
      return &ht->items[h];
    }
    if(EQ(ht->items[h].key, key) == 0) return &ht->items[h];
    h = (h + 1) % ht->capacity;
  }

  assert(0 && "unreachable");
}

KV* ht_find(HT ht, KEY_TYPE key) {
  uint64_t h = HASH(key) % ht.capacity;

  for(size_t i = 0; i < ht.capacity; i++) {
    if(!ht.items[h].occupied) break;
    if(EQ(ht.items[h].key, key) == 0) return &ht.items[h];
    h = (h + 1) % ht.capacity;
  }

  return NULL;
}

void ht_delete(HT* ht, KEY_TYPE key) {
  KV* kv = ht_find(ht, key);
  kv->occupied = 0;
  ht->count--;

  if(ht->count < (ht->capacity / 2) - (ht->capacity / 2 / 10)) {
    __ht_realloc(ht, ht->capacity / 2);
  }
}

void __ht_realloc(HT *ht, size_t s) {
#ifdef NO_RESIZE
  fprintf(stderr, "ERROR: hashtable full\n");
  exit(1);
#else // NO_RESIZE
  KV* old_items = ht->items;
  size_t old_capacity = ht->capacity;
  ht->items = calloc(s, sizeof(KV));
  ht->capacity = s;
  for(size_t i = 0; i < old_capacity; i++) {
    if(old_items[i].occupied) {
      uint64_t h = HASH(old_items[i].key) % ht->capacity;

      while(ht->items[h].occupied && EQ(ht->items[h].key, old_items[i].key) == 0) h++;

      ht->items[h].occupied = 1;
      ht->items[h].key = old_items[i].key;
      ht->items[h].val = old_items[i].val;
    }
  }

  free(old_items);
#endif // NO_RESIZE
}

#endif // HT_NO_IMPLEMENTATION

#endif // HT_H
