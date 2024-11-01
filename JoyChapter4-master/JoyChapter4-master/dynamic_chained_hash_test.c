
#include "dynamic_chained_hash.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int
main(int argc, const char *argv[])
{
  if (argc != 2) {
    printf("Usage: %s no_elements\n", argv[0]);
    return EXIT_FAILURE;
  }

  int no_elms = atoi(argv[1]);

  srandom((unsigned int)time(NULL));
  unsigned int *keys = malloc(no_elms * sizeof *keys);
  for (int i = 0; i < no_elms; ++i) {
    keys[i] = rand();
  }

  struct hash_table *table = new_table();
  print_table(table);

  insert_key(table, 1);
  insert_key(table, 2);
  print_table(table);
  delete_key(table, 2);
  print_table(table);

  printf("Inserting keys\n");
  printf("-------------------------------\n");
  for (int i = 0; i < no_elms; ++i) {
    // insert_key(table, (1 << 2) | i);
    printf("inserting %u\n", i);
    insert_key(table, i);
    print_table(table);

    printf("inserting %u\n", keys[i]);
    insert_key(table, keys[i]);
    print_table(table);

    for (int j = 0; j < i; j++) {
      assert(contains_key(table, j));
      assert(contains_key(table, keys[j]));
    }
  }
  printf("Done inserting\n");
  print_table(table);

  // just checking that we do not add existing keys
  printf("----duplications---\n");
  for (int i = 0; i < no_elms; ++i) {
    assert(contains_key(table, i));
    assert(contains_key(table, keys[i]));
    insert_key(table, i);
    insert_key(table, keys[i]);
  }
  print_table(table);

  printf("----deleting---\n");
  for (int i = 0; i < no_elms; ++i) {
    printf("Checking that we have key %u\n", keys[i]);
    print_table(table);
    assert(contains_key(table, keys[i]));
    delete_key(table, keys[i]);
    assert(!contains_key(table, keys[i]));
  }
  print_table(table);
  for (int i = 0; i < no_elms; ++i) {
    printf("deleting %u\n", i);
    assert(contains_key(table, i));
    delete_key(table, i);
    assert(!contains_key(table, i));
    print_table(table);
  }

  free(keys);
  delete_table(table);

  return EXIT_SUCCESS;
}
