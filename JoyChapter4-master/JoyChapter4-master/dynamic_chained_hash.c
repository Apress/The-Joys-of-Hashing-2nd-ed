
#include "dynamic_chained_hash.h"

#include <stdio.h>
#include <stdlib.h>

#include "linked_lists.h"

static const unsigned int SUBTABLE_BITS = 3; // 8 bins to a sub-table

// A sub-table is an array of pointers to links.
// A sub-table plus an index is also a struct link **
// which by good fortune is a LIST.
typedef struct link **subtable;
struct hash_table {
  subtable *tables; // Tables is an array of sub-tables

  unsigned int table_bits; // Bits used for indexing into sub-tables
  unsigned int split;      // Pointer to the bin we need to split/merge

  unsigned int allocated_subtables; // Number of sub-tables allocated
};

// Size of a word with `bits` bits
static inline unsigned int
bits_size(unsigned int bits)
{
  return 1 << bits;
}
// The range [0, split + m) are initialised. The range [split + m, 2m)
// is where we are adding new initialised bins through splitting.
static inline unsigned int
m(struct hash_table *table)
{
  return bits_size(table->table_bits + SUBTABLE_BITS);
}

// The largest bin that is currently in use
static inline unsigned int
max_index(struct hash_table *table)
{
  return m(table) + table->split;
}

// Mask for the lower `bits` bits
static inline unsigned int
bit_mask(unsigned int bits)
{
  return bits_size(bits) - 1;
}
// A mask for the parts of hash keys we are currently considering
static inline unsigned int
key_mask(struct hash_table *table)
{
  return bit_mask(table->table_bits + 1 + SUBTABLE_BITS);
}

// The bins up to split + m are valid, the higher indices are not.
// If we are below this index, we can use the index, otherwise we need
// to use the smaller range [0, m).
static inline unsigned int
key_in_table_range(struct hash_table *table, unsigned int hash_key)
{
  unsigned int masked_key = hash_key & key_mask(table);
  return (masked_key < max_index(table)) ? masked_key : (masked_key - m(table));
}

static inline unsigned int
table_index(struct hash_table *table, unsigned int hash_key)
{
  return hash_key >> SUBTABLE_BITS;
}
static inline unsigned int
bin_index(struct hash_table *table, unsigned int hash_key)
{
  return hash_key & bit_mask(SUBTABLE_BITS);
}

// Get a bin from an index
static inline LIST
get_bin(struct hash_table *table, unsigned int hash_key)
{
  unsigned int tab_idx = table_index(table, hash_key);
  unsigned int bin_idx = bin_index(table, hash_key);
  return &table->tables[tab_idx][bin_idx];
}

struct hash_table *
new_table()
{
  struct hash_table *table = malloc(sizeof *table);

  // Initialial size olds 2 table-pointers, [0,m) and [m,2m).
  table->tables = malloc(2 * sizeof *table->tables);

  // Allocate and initialise the first table only.
  table->tables[0] =
      malloc(bits_size(SUBTABLE_BITS) * sizeof *table->tables[0]);
  for (unsigned int i = 0; i < bits_size(SUBTABLE_BITS); i++) {
    table->tables[0][i] = NULL;
  }
  table->allocated_subtables = 1;

  table->table_bits = 0; // we only use bin bits initially
  table->split = 0;      // we start splitting at the first bin

  return table;
}

void
delete_table(struct hash_table *table)
{
  // Delete lists in all initialised bins
  for (unsigned int bin = 0; bin < max_index(table); bin++) {
    free_list(get_bin(table, bin));
  }

  // Delete subtables.
  for (unsigned int tbl = 0; tbl < table->allocated_subtables; tbl++) {
    free(table->tables[tbl]);
  }

  // And finally free the tables array and the table
  free(table->tables);
  free(table);
}

void
init_next_subtable(struct hash_table *table)
{
  // Grow table if we have inserted m elements.
  if (table->split == m(table)) {
    // Use one more bit for table indices
    table->table_bits++;

    // Alloc more table pointers (but don't initialise, we do that
    // incrementally). The first half of the new size handles the
    // new [0,m) and the second the new [m,2m) range. The new [0,m)
    // range is already initialised.
    size_t new_size = 2 * bits_size(table->table_bits) * sizeof *table->tables;
    table->tables = realloc(table->tables, new_size);

    // Reset split pointer
    table->split = 0;
  }

  unsigned int tab_index = table_index(table, max_index(table));
  if (tab_index == table->allocated_subtables) {
    // If we are moving into a new sub-table, we need to allocate it
    table->tables[tab_index] =
        malloc(bits_size(SUBTABLE_BITS) * sizeof *table->tables[tab_index]);
    table->allocated_subtables++;
  }
}

void
split_bin(LIST from_bin, LIST to_bin, unsigned int split_bit)
{
  struct link *link = *from_bin; // Catch list before we clear the bin.
  *to_bin = NULL;                // Initialise if it isn't already
  *from_bin = NULL;              // Make bin ready for new values

  while (link) {
    struct link *next = link->next;
    if (link->key & split_bit) {
      // Move link
      link->next = *to_bin;
      *to_bin = link;
    } else {
      // Put link back into its current bin
      link->next = *from_bin;
      *from_bin = link;
    }
    link = next;
  }
}

static void
split(struct hash_table *table)
{
  // Initialise the target bin at split + m.
  init_next_subtable(table);

  // Get the split bin and if there are elements there, split them.
  LIST from_bin = get_bin(table, table->split);
  LIST to_bin = get_bin(table, max_index(table));
  split_bin(from_bin, to_bin, m(table));

  // Update counter to reflect that we have split
  table->split++;
}

void
insert_key(struct hash_table *table, unsigned int key)
{
  LIST bin = get_bin(table, key_in_table_range(table, key));
  if (!contains_element(bin, key)) {
    add_element(bin, key);
    split(table);
  }
}

bool
contains_key(struct hash_table *table, unsigned int key)
{
  LIST bin = get_bin(table, key_in_table_range(table, key));
  return contains_element(bin, key);
}

static void
merge_bins(LIST from_bin, LIST to_bin)
{
  struct link *link = *from_bin;
  while (link) {
    struct link *next = link->next;
    link->next = *to_bin;
    *to_bin = link;
    link = next;
  }
}

static void
shrink_tables(struct hash_table *table)
{
  // Checking when we point to the beginning of [0,2m).
  if (table->split == 0 &&
      bits_size(table->table_bits) < table->allocated_subtables / 4) {
    unsigned int new_no_tables = bits_size(table->table_bits + 1);
    for (unsigned int i = new_no_tables; i < table->allocated_subtables; i++) {
      free(table->tables[i]);
    }
    table->tables =
        realloc(table->tables, new_no_tables * sizeof *table->tables);
    table->allocated_subtables = new_no_tables;
  }
}

// Decrement split. If it is a zero, we need to
// decrement table_bits and m instead, and set split to m - 1.
static inline void
dec_split(struct hash_table *table)
{
  if (table->split > 0) {
    table->split--;
  } else {
    table->table_bits--;
    table->split = m(table) - 1;
  }
}

static void
merge(struct hash_table *table)
{
  // Decrement split. If it is a zero, we need to
  // decrement table_bits and m instead, and set split to m - 1.
  dec_split(table);

  // Merge largest bin into split bin (well, one before the split bin so the
  // indices match)
  merge_bins(get_bin(table, max_index(table)), get_bin(table, table->split));

  shrink_tables(table);
}

void
delete_key(struct hash_table *table, unsigned int key)
{
  LIST bin = get_bin(table, key_in_table_range(table, key));
  if (contains_element(bin, key)) {
    delete_element(bin, key);
    merge(table);
  }
}

void
print_table(struct hash_table *table)
{
  unsigned int slot;
  for (slot = 0; slot < max_index(table); slot++) {
    if ((slot & bit_mask(SUBTABLE_BITS)) == 0)
      printf("\n");
    LIST bin = get_bin(table, slot);
    char *sep = "";
    if (slot == table->split)
      printf("->");
    printf("[");
    struct link *x;
    for (x = *bin; x; x = x->next) {
      printf("%s%u", sep, x->key);
      sep = "|";
    }
    printf("]");
  }
  printf("\n");
  printf("Allocated sutables: %u\n", table->allocated_subtables);
  printf("\n");
  printf("Usage is %u\n", max_index(table));
}
