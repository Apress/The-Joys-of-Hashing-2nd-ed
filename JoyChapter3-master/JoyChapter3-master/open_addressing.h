
#ifndef OPEN_ADDRESSING_H
#define OPEN_ADDRESSING_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

struct bin {
  unsigned int key;
  int in_probe : 1; // The bin is part of a sequence of used bins
  int is_empty : 1; // The bin does not contain a value (but might still be in
                    // a probe sequence)
};

struct hash_table {
  struct bin *bins;
  unsigned int size;
};

struct hash_table *
new_table(unsigned int size);
void
free_table(struct hash_table *table);

void
insert_key(struct hash_table *table, unsigned int key);
bool
contains_key(struct hash_table *table, unsigned int key);
void
delete_key(struct hash_table *table, unsigned int key);

// For debugging
void
print_table(struct hash_table *table);

#endif
