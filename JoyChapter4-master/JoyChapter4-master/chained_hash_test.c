
#include "chained_hash.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static unsigned int
random_key()
{
  unsigned int key = (unsigned int)rand();
  return key;
}

int
main(int argc, const char *argv[])
{
  if (argc != 2) {
    printf("Usage: %s no_elements\n", argv[0]);
    return EXIT_FAILURE;
  }

  int no_elms = atoi(argv[1]);
  unsigned int *keys = malloc(no_elms * sizeof *keys);
  for (int i = 0; i < no_elms; ++i) {
    keys[i] = random_key();
  }
  struct hash_table *table = new_table();
  clock_t start = clock();
  for (int i = 0; i < no_elms; ++i) {
    insert_key(table, keys[i]);
  }
  for (int i = 0; i < no_elms; ++i) {
    assert(contains_key(table, keys[i]));
  }
  for (int i = 0; i < no_elms; ++i) {
    contains_key(table, random_key());
  }
  for (int i = 0; i < no_elms; ++i) {
    delete_key(table, keys[i]);
  }
  for (int i = 0; i < no_elms; ++i) {
    assert(!contains_key(table, keys[i]));
  }
  clock_t end = clock();
  double elapsed_time = (end - start) / (double)CLOCKS_PER_SEC;
  printf("%g\n", elapsed_time);

  free(keys);
  free_table(table);

  return EXIT_SUCCESS;
}
