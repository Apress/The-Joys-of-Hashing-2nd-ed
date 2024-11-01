
#include "open_addressing_map.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Probing
unsigned int static p(unsigned int k, unsigned int i, unsigned int m)
{
  return (k + i) & (m - 1);
}

// Helpers
static inline unsigned int
hash(struct hash_table *table, void const *key)
{
  return table->key_type->hash(key);
}

static inline void *
copy_key(struct hash_table *table, void const *key)
{
  return table->key_type->cpy(key);
}

static inline void *
copy_val(struct hash_table *table, void const *val)
{
  return table->value_type->cpy(val);
}

static inline void
free_key(struct hash_table *table, void *key)
{
  table->key_type->del(key);
}

static inline void
free_val(struct hash_table *table, void *val)
{
  table->value_type->del(val);
}

static inline bool
is_active_bin(struct bin *bin)
{
  return bin->in_probe && !bin->is_empty;
}

// Creating and resizing tables

// add_map_internal is a helper function for add_map that expects us to have
// already computed the hash_key for the key and copied the key and value. It
// inserts the hash_key/key -> value mapping in the table.
static void
add_map_internal(struct hash_table *table, unsigned int hash_key,
                 void *key_copy, void *value_copy);

// Initialize the table with `size` bins, and then copy the bins from `begin` to
// `end` into the table.
static void
init_table(struct hash_table *table, unsigned int size, struct bin *begin,
           struct bin *end)
{
  // Initialize table members
  table->bins = malloc(size * sizeof *table->bins);
  table->size = size;
  table->used = 0;
  table->active = 0;

  // Initialize bins
  struct bin empty_bin = {.in_probe = false, .is_empty = true};
  for (unsigned int i = 0; i < table->size; i++) {
    table->bins[i] = empty_bin;
  }

  // Copy the old bins to the new table
  for (struct bin *bin = begin; bin != end; bin++) {
    if (!bin->is_empty) {
      add_map_internal(table, bin->hash_key, bin->key, bin->val);
    }
  }
}

#define MIN_SIZE 8

struct hash_table *
new_table(struct key_type const *key_type, struct value_type const *value_type)
{
  struct hash_table *table = malloc(sizeof *table);
  table->key_type = key_type;
  table->value_type = value_type;
  init_table(table, MIN_SIZE, NULL, NULL);
  return table;
}

static void
resize(struct hash_table *table, unsigned int new_size)
{
  // remember the old bins until we have moved them.
  struct bin *old_bins_begin = table->bins,
             *old_bins_end = old_bins_begin + table->size;

  // Update table and copy the old active bins to it.
  init_table(table, new_size, old_bins_begin, old_bins_end);

  // finally, free memory for old bins
  free(old_bins_begin);
}

// Deleting tables

// If there is data in a bin, free it
static inline void
free_bin(struct hash_table *table, struct bin *bin)
{
  if (is_active_bin(bin)) {
    free_key(table, bin->key);
    free_val(table, bin->val);
    bin->is_empty = true; // Delete the bin
    table->active--;      // Same bins in use but one less active
  }
}

void
delete_table(struct hash_table *table)
{
  for (struct bin *bin = table->bins; bin != table->bins + table->size; ++bin) {
    free_bin(table, bin);
  }
  free(table->bins);
  free(table);
}

// Lookup

// Check if the bin contains the key. We first check if the bin is active,
// then if the hash keys match (if they don't, we don't need to call a
// potentially expensive key comparison function), and finally we compare the
// keys.
static inline bool
key_in_bin(struct hash_table *table, struct bin *bin, unsigned int hash_key,
           void const *key)
{
  return is_active_bin(bin) && bin->hash_key == hash_key &&
         table->key_type->cmp(bin->key, key);
}

// Find the bin containing key, or the first bin past the end of its probe.
// It will never return a bin that is in a probe and empty, since those
// cannot contain the key and if we need an empty bin we will search for
// the earliest in the probe using find_empty().
struct bin *
find_key(struct hash_table *table, unsigned int hash_key, void const *key)
{
  for (unsigned int i = 0; i < table->size; i++) {
    struct bin *bin = table->bins + p(hash_key, i, table->size);
    if (!bin->in_probe ||                      // end of probe, or
        key_in_bin(table, bin, hash_key, key)) // found the key
      return bin;
  }
  assert(false); // We should never get here
}

void *const
lookup_key(struct hash_table *table, void const *key)
{
  struct bin *bin = find_key(table, hash(table, key), key);
  return bin->in_probe ? bin->val : NULL;
}

// Find the first empty bin in its probe.
struct bin *
find_empty(struct hash_table *table, unsigned int hash_key)
{
  for (unsigned int i = 0; i < table->size; i++) {
    struct bin *bin = table->bins + p(hash_key, i, table->size);
    if (bin->is_empty)
      return bin;
  }
  assert(false); // We should never get here
}

// Insertion
static inline void
store_in_bin(struct hash_table *table, struct bin *bin, unsigned int hash_key,
             void *key, void *value)
{
  // Update counters based on current state of bin.
  table->active += !!bin->is_empty; // inc if the bin is empty
  table->used += !bin->in_probe;    // inc if the bin hasn't been used before

  // Free any key or value currently in the bin.
  free_bin(table, bin);

  // Store the new key and value in the bin.
  *bin = (struct bin){
      .in_probe = true,
      .is_empty = false,
      .hash_key = hash_key,
      .key = key,
      .val = value,
  };
}

struct bin *
get_bin(struct hash_table *table, unsigned int hash_key, void *const key)
{
  struct bin *bin = find_key(table, hash_key, key);
  return bin->in_probe ? bin : find_empty(table, hash_key);
}

static void
add_map_internal(struct hash_table *table, unsigned int hash_key,
                 void *key_copy, void *value_copy)
{
  struct bin *bin = get_bin(table, hash_key, key_copy);
  store_in_bin(table, bin, hash_key, key_copy, value_copy);

  if (table->used > table->size / 2)
    resize(table, table->size * 2);
}

void
add_map(struct hash_table *table, void const *key, void const *value)
{
  unsigned int hash_key = hash(table, key);
  void *key_copy = copy_key(table, key);
  void *value_copy = copy_val(table, value);
  add_map_internal(table, hash_key, key_copy, value_copy);
}

// Deletion

void
delete_key(struct hash_table *table, void const *key)
{
  struct bin *bin = find_key(table, hash(table, key), key);
  free_bin(table, bin);

  if (table->active < table->size / 8 && table->size > MIN_SIZE)
    resize(table, table->size / 2);
}
