
#ifndef HASH_H
#define HASH_H

#include <stdbool.h>

#define RESERVED_KEY ((unsigned int)0)

struct hash_table {
  unsigned int size;
  unsigned int *bins;
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
