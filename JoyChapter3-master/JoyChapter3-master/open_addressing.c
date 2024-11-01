
#include "open_addressing.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static unsigned int
p(unsigned int key, unsigned int i, unsigned int m)
{
  return (key + i) & (m - 1);
}

struct hash_table *
new_table(unsigned int size)
{
  struct hash_table *table = malloc(sizeof *table);
  table->size = size;
  table->bins = malloc(size * sizeof *table->bins);

  struct bin empty_bin = {.in_probe = false, .is_empty = true};
  for (unsigned int i = 0; i < size; i++) {
    table->bins[i] = empty_bin;
  }

  table->size = size;
  return table;
}

void
free_table(struct hash_table *table)
{
  free(table->bins);
  free(table);
}

// Find the bin containing key, or the first bin past the end of its probe
struct bin *
find_key(struct hash_table *table, unsigned int key)
{
  for (unsigned int i = 0; i < table->size; i++) {
    struct bin *bin = table->bins + p(key, i, table->size);
    if (bin->key == key || !bin->in_probe)
      return bin;
  }
  // The table is full. We cannot handle that yet!
  assert(false);
}

// Find the first empty bin in its probe.
struct bin *
find_empty(struct hash_table *table, unsigned int key)
{
  for (unsigned int i = 0; i < table->size; i++) {
    struct bin *bin = table->bins + p(key, i, table->size);
    if (bin->is_empty)
      return bin;
  }
  // The table is full. We cannot handle that yet!
  assert(false);
}

void
insert_key(struct hash_table *table, unsigned int key)
{
  if (!contains_key(table, key)) {
    *find_empty(table, key) =
        (struct bin){.in_probe = true, .is_empty = false, .key = key};
  }
}

bool
contains_key(struct hash_table *table, unsigned int key)
{
  struct bin *bin = find_key(table, key);
  return bin->key == key && !bin->is_empty;
}

void
delete_key(struct hash_table *table, unsigned int key)
{
  find_key(table, key)->is_empty = true;
}

void
print_table(struct hash_table *table)
{
  for (unsigned int i = 0; i < table->size; i++) {
    if (i > 0 && i % 8 == 0) {
      printf("\n");
    }
    struct bin *bin = table->bins + i;
    if (bin->in_probe && !bin->is_empty) {
      printf("[%u]", bin->key);
    } else if (bin->in_probe && bin->is_empty) {
      printf("[*]");
    } else {
      printf("[ ]");
    }
  }
  printf("\n----------------------\n");
}
