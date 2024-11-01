
#ifndef OPEN_ADDRESSING_H
#define OPEN_ADDRESSING_H

#include <stdbool.h>
#include <stdint.h>

typedef unsigned int (*hash_func)(void const *);
typedef bool (*compare_func)(void const *, void const *);
typedef void (*destructor_func)(void *);
typedef void *(*copy_func)(void const *);

struct key_type {
  hash_func hash;
  compare_func cmp;
  copy_func cpy;
  destructor_func del;
};

struct value_type {
  copy_func cpy;
  destructor_func del;
};

struct bin {
  int in_probe : 1; // The bin is part of a sequence of used bins
  int is_empty : 1; // The bin does not contain a value (but might still be in
                    // a probe sequence)

  unsigned int hash_key; // cached hash key
  void *key;             // pointer to the actual key
  void *val;             // pointer to the value
};

struct hash_table {
  struct bin *bins;
  unsigned int size;
  unsigned int used;
  unsigned int active;
  struct key_type const *key_type;
  struct value_type const *value_type;
};

struct hash_table *
new_table(struct key_type const *key_type, struct value_type const *value_type);

void
delete_table(struct hash_table *table);

void
add_map(struct hash_table *table, void const *key, void const *value);
void
delete_key(struct hash_table *table, void const *key);
void *const
lookup_key(struct hash_table *table, void const *key);

#endif
