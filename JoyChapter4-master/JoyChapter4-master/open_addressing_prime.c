
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "open_addressing.h"

#define UPPER_LOAD_LIMIT 0.5
#define LOWER_LOAD_LIMIT 0.251

// Primes for 1.66 growth
int primes[] = {11,     19,      37,      67,      113,     191,     331,
                557,    929,     1543,    2579,    4283,    7121,    11821,
                19661,  32647,   54217,   90001,   149411,  248033,  411737,
                683489, 1134607, 1883459, 3126547, 5190071, 8615527, 14301779};

static size_t no_primes = (sizeof primes) / sizeof(*primes);

static unsigned int
p(unsigned int k, unsigned int i, unsigned int m)
{
  return (k + i) % m;
}

static void
init_table(struct hash_table *table, unsigned int prime_idx, struct bin *begin,
           struct bin *end)
{
  unsigned int size = primes[prime_idx];

  // Initialize table members
  struct bin *bins = malloc(size * sizeof *bins);
  *table = (struct hash_table){.bins = bins,
                               .size = size,
                               .used = 0,
                               .active = 0,
                               .primes_idx = prime_idx};

  // Initialize bins
  struct bin empty_bin = {.in_probe = false, .is_empty = true};
  for (unsigned int i = 0; i < table->size; i++) {
    table->bins[i] = empty_bin;
  }

  // Copy the old bins to the new table
  for (struct bin *bin = begin; bin != end; bin++) {
    if (!bin->is_empty) {
      insert_key(table, bin->key);
    }
  }
}

struct hash_table *
new_table()
{
  struct hash_table *table = malloc(sizeof *table);
  init_table(table, 0, NULL, NULL);
  return table;
}

static void
resize(struct hash_table *table, unsigned int new_primes_idx)
{
  // remember the old bins until we have moved them.
  struct bin *old_bins_begin = table->bins,
             *old_bins_end = old_bins_begin + table->size;

  // Update table and copy the old active bins to it.
  init_table(table, new_primes_idx, old_bins_begin, old_bins_end);

  // finally, free memory for old bins
  free(old_bins_begin);
}
void
delete_table(struct hash_table *table)
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
  // The table is full. This should not happen!
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
  // The table is full. This should not happen!
  assert(false);
}

void
insert_key(struct hash_table *table, unsigned int key)
{
  if (!contains_key(table, key)) {
    struct bin *key_bin = find_empty(table, key);

    table->active++;
    if (!key_bin->in_probe)
      table->used++; // We are using a new bin

    *key_bin = (struct bin){.in_probe = true, .is_empty = false, .key = key};

    if (table->used > table->size / 2) {
      assert(table->primes_idx + 1 < no_primes);
      resize(table, table->primes_idx + 1);
    }
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
  struct bin *bin = find_key(table, key);
  if (bin->key != key)
    return; // Nothing more to do

  bin->is_empty = true; // Delete the bin
  table->active--;      // Same bins in use but one less active

  if (table->active < table->size / 8 && table->primes_idx > 0) {
    resize(table, table->primes_idx - 1);
  }
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
