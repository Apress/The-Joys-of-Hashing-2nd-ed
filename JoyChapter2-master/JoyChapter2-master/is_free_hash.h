
#ifndef HASH_H
#define HASH_H

#include <stdbool.h>

struct bin {
  int is_free : 1;
  unsigned int key;
};

struct hash_table {
  unsigned int size;
  struct bin *bins;
};

struct hash_table *
new_table(unsigned int size);
void
delete_table(struct hash_table *table);

void
insert_key(struct hash_table *table, unsigned int key);
bool
contains_key(struct hash_table *table, unsigned int key);
void
delete_key(struct hash_table *table, unsigned int key);

#endif
