
#ifndef OPEN_ADDRESSING_H
#define OPEN_ADDRESSING_H

#include <stdbool.h>

struct bin {
  int in_probe : 1; // The bin is part of a sequence of used bins
  int is_empty : 1; // The bin does not contain a value (but might still be in
                    // a probe sequence)
  unsigned int key;
};

struct hash_table {
  struct bin *bins;
  unsigned int size;
  unsigned int used;
  unsigned int active;
  // only used in primes code, but we share the header, so...
  unsigned int primes_idx;
};

struct hash_table *
new_table(void);
void
delete_table(struct hash_table *table);

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
