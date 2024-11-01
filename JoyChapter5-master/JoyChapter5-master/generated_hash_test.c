
#include "generated_hash_set.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static unsigned int
random_int_key()
{
  unsigned int key = (unsigned int)rand();
  return key;
}

char *
itoa(unsigned int i)
{
  // Not super safe itoa, but good enough for an example like this.
  char *buf = malloc(sizeof(char) * 20);
  sprintf(buf, "%d", i);
  return buf;
}

char *
strdup(const char *s)
{
  char *new = malloc(strlen(s) + 1);
  strcpy(new, s);
  return new;
}

static char *
random_string_key()
{
  unsigned int key = (unsigned int)rand();
  return itoa(key);
}

// comparison, (dummy) hash function, and dummy destructor for int keys
#define EQ_CMP(A, B) ((A) == (B))
#define HASH(KEY) ((KEY) ^ (0xdeadbeef))
#define NOP_DESTRUCTOR(KEY)

GEN_HASH_TABLE(integer, unsigned int, EQ_CMP, HASH, NOP_DESTRUCTOR);

void
test_int_table(int no_elms)
{
  unsigned int *keys = malloc(no_elms * sizeof *keys);
  for (int i = 0; i < no_elms; ++i) {
    keys[i] = random_int_key();
  }
  struct integer_hash_table *table = integer_new_table();
  clock_t start = clock();
  printf("Inserting %d elements\n", no_elms);
  for (int i = 0; i < no_elms; ++i) {
    printf("Inserting key %d\n", keys[i]);
    integer_insert_key(table, keys[i]);
  }
  printf("Checking if the elemers are now in the table.\n");
  for (int i = 0; i < no_elms; ++i) {
    bool contains = integer_contains_key(table, keys[i]);
    printf("Checking if %d is in the table? %d\n", keys[i], contains);
  }
  for (int i = 0; i < no_elms; ++i) {
    assert(integer_contains_key(table, keys[i]));
  }

  printf("Deleting all elements.\n");
  for (int i = 0; i < no_elms; ++i) {
    integer_delete_key(table, keys[i]);
  }
  printf("Checking if the elemers are still in the table.\n");
  for (int i = 0; i < no_elms; ++i) {
    bool contains = integer_contains_key(table, keys[i]);
    printf("Checking if %d is in the table? %d\n", keys[i], contains);
  }
  for (int i = 0; i < no_elms; ++i) {
    assert(!integer_contains_key(table, keys[i]));
  }

  clock_t end = clock();
  double elapsed_time = (end - start) / (double)CLOCKS_PER_SEC;
  printf("%g\n", elapsed_time);

  free(keys);
  integer_free_table(table);
}

// String table (where the table takes ownership and frees elements
// through the generated list code).
#define STR_EQ(A, B) (strcmp(A, B) == 0)
unsigned int
u32_hash(char *x)
{
  unsigned int h = 0;
  for (char *p = x; *p; p++) {
    h = 31 * h + *p;
  }
  return h;
}
GEN_HASH_TABLE(string, char *, STR_EQ, u32_hash, free);

void
test_string_table(int no_elms)
{
  char **keys = malloc(no_elms * sizeof *keys);
  for (int i = 0; i < no_elms; ++i) {
    keys[i] = random_string_key();
  }
  struct string_hash_table *table = string_new_table();
  clock_t start = clock();
  printf("Inserting %d elements\n", no_elms);
  for (int i = 0; i < no_elms; ++i) {
    printf("Inserting key %s\n", keys[i]);
    string_insert_key(table,
                      strdup(keys[i])); // strdub because table takes ownership
  }
  printf("Checking if the elemers are now in the table.\n");
  for (int i = 0; i < no_elms; ++i) {
    bool contains = string_contains_key(table, keys[i]);
    printf("Checking if %s is in the table? %d\n", keys[i], contains);
  }
  for (int i = 0; i < no_elms; ++i) {
    assert(string_contains_key(table, keys[i]));
  }

  printf("Deleting all elements.\n");
  for (int i = 0; i < no_elms; ++i) {
    string_delete_key(table, keys[i]);
  }
  printf("Checking if the elemers are still in the table.\n");
  for (int i = 0; i < no_elms; ++i) {
    bool contains = string_contains_key(table, keys[i]);
    printf("Checking if %s is in the table? %d\n", keys[i], contains);
  }
  for (int i = 0; i < no_elms; ++i) {
    assert(!string_contains_key(table, keys[i]));
  }

  clock_t end = clock();
  double elapsed_time = (end - start) / (double)CLOCKS_PER_SEC;
  printf("%g\n", elapsed_time);

  for (int i = 0; i < no_elms; ++i) {
    free(keys[i]); // we have ownership of these...
  }

  free(keys);
  string_free_table(table);
}

int
main(int argc, const char *argv[])
{
  if (argc != 2) {
    printf("Usage: %s no_elements\n", argv[0]);
    return EXIT_FAILURE;
  }

  int no_elms = atoi(argv[1]);
  test_int_table(no_elms);
  test_string_table(no_elms);

  return EXIT_SUCCESS;
}
