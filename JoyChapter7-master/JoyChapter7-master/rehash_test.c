
#include "open_addressing.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static unsigned int random_key() { return (unsigned int)random(); }

bool power_of_two(uint32_t x) { return (x != 0) && ((x & (x - 1)) == 0); }

int main(int argc, const char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s size no_elements\n", argv[0]);
    return EXIT_FAILURE;
  }

  uint32_t size = atoi(argv[1]);
  uint32_t no_elms = atoi(argv[2]);

  if (!power_of_two(size)) {
    printf("the size must be a power of two.\n");
    return EXIT_FAILURE;
  }

  // for random states, use: srand((unsigned) time(&t));
  srand(0); // fix the seed for experiments

  unsigned int *keys = malloc(no_elms * sizeof *keys);
  for (int i = 0; i < no_elms; ++i) {
    keys[i] = random_key();
  }

  struct hash_table *table = new_table();
  for (int i = 0; i < no_elms; ++i) {
    printf("Inserting key %u\n", keys[i]);
    insert_key(table, keys[i]);
    assert(contains_key(table, keys[i]));
  }
  for (int i = 0; i < no_elms; ++i) {
    printf("Checking key %u\n", keys[i]);
    assert(contains_key(table, keys[i]));
  }
  for (int i = 0; i < no_elms; ++i) {
    printf("Checking a random key...\n");
    contains_key(table, random_key());
  }
  for (int i = 0; i < no_elms; ++i) {
    printf("Deleting key %u\n", keys[i]);
    delete_key(table, keys[i]);
    assert(!contains_key(table, keys[i]));
  }
  for (int i = 0; i < no_elms; ++i) {
    printf("Checking key %u\n", keys[i]);
    assert(!contains_key(table, keys[i]));
  }
  free(keys);
  delete_table(table);

  printf("DONE\n");

  return EXIT_SUCCESS;
}
