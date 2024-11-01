
#include "open_addressing.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MIN_SIZE 8

// Sample a new function and place it in f
void tabulation_sample(hash_func f) {
  uint32_t *start = f;
  uint32_t *end = start + HASH_FUNC_WORDS;
  while (start != end)
    *(start++) = rand();
}

// clang-format off
// tabulation hashing, r=4, q=32
uint32_t hash(uint32_t x, hash_func f) {
  const uint32_t no_cols = 1 << R;
  const uint32_t mask = no_cols - 1;

  uint32_t y = 0;
  y ^= f[0 * no_cols + (x & mask)];  x >>= R;
  y ^= f[1 * no_cols + (x & mask)];  x >>= R;
  y ^= f[2 * no_cols + (x & mask)];  x >>= R;
  y ^= f[3 * no_cols + (x & mask)];  x >>= R;
  y ^= f[4 * no_cols + (x & mask)];  x >>= R;
  y ^= f[5 * no_cols + (x & mask)];  x >>= R;
  y ^= f[6 * no_cols + (x & mask)];  x >>= R;
  y ^= f[7 * no_cols + (x & mask)];

  return y;
}
// clang-format on

unsigned int static p(unsigned int k, unsigned int i, unsigned int m) {
  return (k + i) & (m - 1);
}

static void init_table(struct hash_table *table, unsigned int size,
                       struct bin *begin, struct bin *end) {
  // Initialize table members
  table->bins = malloc(size * sizeof *table->bins);
  table->size = size;
  table->used = 0;
  table->active = 0;
  table->ops_since_rehash = 0;

  // Initialise the hash table with a new function from the hash family
  tabulation_sample(table->hash_func);

  // Initialize bins
  struct bin empty_bin = {.in_probe = false, .is_empty = true};
  for (unsigned int i = 0; i < table->size; i++) {
    table->bins[i] = empty_bin;
  }

  // Copy the old bins to the new table
  for (struct bin *bin = begin; bin != end; bin++) {
    if (!bin->is_empty) {
      insert_key(table, bin->user_key);
    }
  }
}

static void resize(struct hash_table *table, unsigned int new_size) {
  // remember the old bins until we have moved them.
  struct bin *old_bins_begin = table->bins,
             *old_bins_end = old_bins_begin + table->size;

  // Update table and copy the old active bins to it.
  init_table(table, new_size, old_bins_begin, old_bins_end);

  // finally, free memory for old bins
  free(old_bins_begin);
}

static void rehash(struct hash_table *table) {
  // Resizing and rehashing is the same code, except we don't change the size.
  resize(table, table->size);
}

struct hash_table *new_table() {
  struct hash_table *table = malloc(sizeof *table);
  init_table(table, MIN_SIZE, NULL, NULL);
  return table;
}

void delete_table(struct hash_table *table) {
  free(table->bins);
  free(table);
}

// Find the bin containing key, or the first bin past the end of its probe
struct bin *find_key(struct hash_table *table, unsigned int user_key,
                     uint32_t hash_key) {
  for (unsigned int i = 0; i < table->size; i++) {
    struct bin *bin = table->bins + p(hash_key, i, table->size);
    if (bin->user_key == user_key || !bin->in_probe)
      return bin;
  }
  // The table is full. This should not happen!
  assert(false);
}

// Find the first empty bin in its probe.
struct bin *find_empty(struct hash_table *table, uint32_t hash_key) {
  for (unsigned int i = 0; i < table->size; i++) {
    struct bin *bin = table->bins + p(hash_key, i, table->size);
    if (bin->is_empty)
      return bin;
  }
  // The table is full. This should not happen!
  assert(false);
}

void insert_key(struct hash_table *table, unsigned int user_key) {
  if (table->ops_since_rehash++ > table->size)
    rehash(table);

  uint32_t hash_key = hash(user_key, table->hash_func);
  struct bin *bin = find_key(table, user_key, hash_key);

  if (bin->user_key != user_key || bin->is_empty) {
    struct bin *key_bin = find_empty(table, hash_key);

    table->active++;
    if (!key_bin->in_probe)
      table->used++; // We are using a new bin

    *key_bin =
        (struct bin){.in_probe = true, .is_empty = false, .user_key = user_key};

    if (table->used > table->size / 2)
      resize(table, table->size * 2);
  }
}

bool contains_key(struct hash_table *table, unsigned int user_key) {
  if (table->ops_since_rehash++ > table->size)
    rehash(table);

  uint32_t hash_key = hash(user_key, table->hash_func);
  struct bin *bin = find_key(table, user_key, hash_key);
  return bin->user_key == user_key && !bin->is_empty;
}

void delete_key(struct hash_table *table, unsigned int user_key) {
  if (table->ops_since_rehash++ > table->size)
    rehash(table);

  uint32_t hash_key = hash(user_key, table->hash_func);
  struct bin *bin = find_key(table, user_key, hash_key);
  if (bin->user_key != user_key)
    return; // Nothing more to do

  bin->is_empty = true; // Delete the bin
  table->active--;      // Same bins in use but one less active

  if (table->active < table->size / 8 && table->size > MIN_SIZE)
    resize(table, table->size / 2);
}

void print_table(struct hash_table *table) {
  for (unsigned int i = 0; i < table->size; i++) {
    if (i > 0 && i % 8 == 0) {
      printf("\n");
    }
    struct bin *bin = table->bins + i;
    if (bin->in_probe && !bin->is_empty) {
      printf("[%u]", bin->user_key);
    } else if (bin->in_probe && bin->is_empty) {
      printf("[*]");
    } else {
      printf("[ ]");
    }
  }
  printf("\n----------------------\n");
}
