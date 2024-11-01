
#ifndef OPEN_ADDRESSING_H
#define OPEN_ADDRESSING_H

#include <stdbool.h>
#include <stdint.h>

#define R 4
#define Q 32
#define HASH_FUNC_WORDS (Q * (1 << R) / sizeof(uint32_t))

typedef uint32_t hash_func[HASH_FUNC_WORDS];

struct bin {
  unsigned int user_key; // User (not hashed) key
  int in_probe : 1;      // The bin is part of a sequence of used bins
  int is_empty : 1; // The bin does not contain a value (but might still be in
                    // a probe sequence)
};

struct hash_table {
  struct bin *bins;
  unsigned int size;
  unsigned int used;
  unsigned int active;

  // sampled hash function
  hash_func hash_func;
  // counter to check if it is time to rehash
  unsigned int ops_since_rehash;
};

struct hash_table *new_table(void);
void delete_table(struct hash_table *table);

void insert_key(struct hash_table *table, unsigned int user_key);
bool contains_key(struct hash_table *table, unsigned int user_key);
void delete_key(struct hash_table *table, unsigned int user_key);

// For debugging
void print_table(struct hash_table *table);

#endif
