
#include "chained_hash.h"

#include <stdlib.h>

#include "linked_lists.h"

#define MIN_SIZE 8

LIST
get_key_bin(struct hash_table *table, unsigned int key)
{
  unsigned int mask = table->size - 1;
  unsigned int index = key & mask;
  return table->bins + index;
}

static void
init_bins(struct hash_table *table)
{
  for (LIST bin = table->bins; bin < table->bins + table->size; bin++) {
    *bin = NULL;
  }
}

struct hash_table *
new_table()
{
  struct hash_table *table = malloc(sizeof *table);
  struct link **bins = malloc(MIN_SIZE * sizeof *bins);
  *table = (struct hash_table){.bins = bins, .size = MIN_SIZE, .used = 0};
  init_bins(table);
  return table;
}

void
free_table(struct hash_table *table)
{
  for (LIST bin = table->bins; bin < table->bins + table->size; bin++) {
    free_list(bin);
  }
  free(table->bins);
  free(table);
}

static void
copy_links(struct hash_table *table, LIST from, LIST to)
{
  for (; from < to; from++) {
    while (*from) {
      struct link *link = *from;
      // remove first link from old bin
      *from = link->next;
      // connect link to the new bin
      LIST new_bin = get_key_bin(table, link->key);
      link->next = *new_bin;
      *new_bin = link;
    }
  }
}

static void
resize(struct hash_table *table, unsigned int new_size)
{
  // remember these so we can copy and free the old bins
  struct link **old_bins = table->bins, **old_from = old_bins,
              **old_to = old_from + table->size;

  // set up the new table
  table->bins = malloc(new_size * sizeof *table->bins);
  table->size = new_size;
  init_bins(table);

  // copy keys
  copy_links(table, old_from, old_to);

  // free the old bins memory
  free(old_bins);
}

void
insert_key(struct hash_table *table, unsigned int key)
{
  LIST bin = get_key_bin(table, key);
  if (!contains_element(bin, key)) {
    add_element(bin, key);
    table->used++;
    if (table->size == table->used) {
      resize(table, 2 * table->size);
    }
  }
}

bool
contains_key(struct hash_table *table, unsigned int key)
{
  return contains_element(get_key_bin(table, key), key);
}

void
delete_key(struct hash_table *table, unsigned int key)
{
  LIST bin = get_key_bin(table, key);
  if (contains_element(bin, key)) {
    delete_element(bin, key);
    table->used--;
    if (table->size > MIN_SIZE && table->used < table->size / 4) {
      resize(table, table->size / 2);
    }
  }
}
