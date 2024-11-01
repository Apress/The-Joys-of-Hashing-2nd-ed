
#include "hash_flex.h"

#include <stdio.h>
#include <stdlib.h>

static unsigned int random_key() {
  unsigned int key = (unsigned int)rand();
  if (key == 0 || key == 1) return 2;
  return key;
}

int main() {
  int n = 10;
  unsigned int keys[n];
  for (int i = 0; i < n; ++i) {
    keys[i] = random_key();
  }

  struct hash_table *table = new_table(32);
  for (int i = 0; i < n; ++i) {
    printf("inserting key %u\n", keys[i]);
    insert_key(table, keys[i]);
  }
  printf("\n");

  for (int i = 0; i < n; ++i) {
    printf("is key %u in table? %d\n", keys[i], contains_key(table, keys[i]));
  }
  printf("\n");

  printf("Removing keys 3 and 4 (%u and %u)\n", keys[3], keys[4]);
  delete_key(table, keys[3]);
  delete_key(table, keys[4]);
  printf("\n");

  for (int i = 0; i < n; ++i) {
    printf("is key %u in table? %d\n", keys[i], contains_key(table, keys[i]));
  }
  printf("\n");

  delete_table(table);

  return EXIT_SUCCESS;
}
