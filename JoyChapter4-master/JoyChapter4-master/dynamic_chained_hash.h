
#ifndef CHAINED_HASH_H
#define CHAINED_HASH_H

#include <stdbool.h>

struct hash_table; // Forward declaration

struct hash_table *
new_table();
void
delete_table(struct hash_table *table);
void
insert_key(struct hash_table *table, unsigned int key);
bool
contains_key(struct hash_table *table, unsigned int key);
void
delete_key(struct hash_table *table, unsigned int key);

// For testing...
void
print_table(struct hash_table *table);

#endif
