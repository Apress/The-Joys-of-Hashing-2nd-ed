
#include "is_free_hash.h"

#include <stdlib.h>

struct hash_table *
new_table(unsigned int size)
{
  // Allocate table and bins
  struct hash_table *table = malloc(sizeof *table);
  table->size = size;
  table->bins = malloc(size * sizeof *table->bins);

  // Set all bins to free
  struct bin *beg = table->bins, *end = beg + size;
  for (struct bin *bin = beg; bin != end; bin++) {
    bin->is_free = true;
  }

  return table;
}

void
delete_table(struct hash_table *table)
{
  free(table->bins);
  free(table);
}

static inline unsigned int
hash_bin_index(struct hash_table *table, unsigned int key)
{
  // Using bit masking to get the bin index.
  // Alternatively, we can use key % table->size;
  unsigned int mask = table->size - 1;
  unsigned int index = key & mask;
  return index;
}

static inline struct bin *
hash_bin(struct hash_table *table, unsigned int key)
{
  return table->bins + hash_bin_index(table, key);
}

void
insert_key(struct hash_table *table, unsigned int key)
{
  struct bin *bin = hash_bin(table, key);
  if (bin->is_free) {
    bin->key = key;
    bin->is_free = false;
  } else {
    // There is already a key here, so we have a
    // collision. We cannot deal with this yet.
  }
}

bool
contains_key(struct hash_table *table, unsigned int key)
{
  struct bin *bin = hash_bin(table, key);
  // The bin contains the key if it isn't empty and the key
  // it contains is the one we are looking for.
  return !bin->is_free && (bin->key == key);
}

void
delete_key(struct hash_table *table, unsigned int key)
{
  // Set the bin to free if the key matches, otherwise not (it contains a
  // different key)
  struct bin *bin = hash_bin(table, key);
  bin->is_free = (bin->key == key);
}
