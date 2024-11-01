
#include "special_key_hash.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct hash_table *
new_table(unsigned int size)
{
  // Allocate table and bins
  struct hash_table *table = malloc(sizeof *table);
  unsigned int *bins = malloc(size * sizeof *bins);
  if (!table || !bins)
    goto error;

  *table = (struct hash_table){.size = size, .bins = bins};

  unsigned int *beg = table->bins, *end = beg + size;
  for (unsigned int *bin = beg; bin != end; bin++) {
    *bin = RESERVED_KEY;
  }

  return table;

error:
  free(table);
  free(bins);
  return NULL;
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

static inline unsigned int *
hash_bin(struct hash_table *table, unsigned int key)
{
  return table->bins + hash_bin_index(table, key);
}

void
insert_key(struct hash_table *table, unsigned int key)
{
  assert(key != RESERVED_KEY);
  unsigned int *bin = hash_bin(table, key);
  if (*bin == RESERVED_KEY) {
    *bin = key;
  } else {
    // There is already a key here, so we have a
    // collision. We cannot deal with this yet.
  }
}

bool
contains_key(struct hash_table *table, unsigned int key)
{
  return *hash_bin(table, key) == key;
}

void
delete_key(struct hash_table *table, unsigned int key)
{
  unsigned int *bin = hash_bin(table, key);
  if (*bin == key) {
    *bin = RESERVED_KEY;
  }
}
