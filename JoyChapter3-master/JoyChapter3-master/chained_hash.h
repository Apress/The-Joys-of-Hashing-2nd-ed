
#ifndef CHAINED_HASH_H
#define CHAINED_HASH_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "linked_lists.h"

struct hash_table {
  struct link **bins;
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

#endif
