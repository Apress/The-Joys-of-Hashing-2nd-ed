
#include "generated_list.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EQ_CMP(A, B) ((A) == (B))
#define NOP_DESTRUCTOR(KEY)

GEN_LIST(int,            // name of the list type
         unsigned int,   // underlying type
         EQ_CMP,         // how we compare keys
         NOP_DESTRUCTOR) // how we free keys;

#define DEREF_EQ_CMP(A, B) (*(A) == *(B))
GEN_LIST(intp, unsigned int *, DEREF_EQ_CMP, free);

#define STR_EQ(A, B) (strcmp(A, B) == 0)
GEN_LIST(str, char *, STR_EQ, free);

static void
test_int_list(void)
{
  unsigned int some_keys[] = {
      1, 2, 3, 4, 5,
  };
  size_t n = sizeof(some_keys) / sizeof(*some_keys);
  struct int_list owner = NEW_LIST();

  for (unsigned int i = 0; i < n; i++) {
    printf("inserting key %u\n", some_keys[i]);
    int_add_key(&owner, some_keys[i]);
  }
  printf("\n");

  for (unsigned int i = 0; i < n; i++) {
    printf("is key %u in list? %d\n", some_keys[i],
           int_contains_key(&owner, some_keys[i]));
    assert(int_contains_key(&owner, some_keys[i]));
  }
  printf("\n");

  printf("Removing keys 3 and 4\n");
  int_delete_key(&owner, 3);
  int_delete_key(&owner, 4);
  printf("\n");

  for (unsigned int i = 0; i < n; i++) {
    printf("is key %u in &owner? %d\n", some_keys[i],
           int_contains_key(&owner, some_keys[i]));
    if (some_keys[i] == 3 || some_keys[i] == 4)
      assert(!int_contains_key(&owner, some_keys[i]));
    else
      assert(int_contains_key(&owner, some_keys[i]));
  }
  printf("\n");

  int_free_list(&owner);
}

static unsigned int *
dup_ui(unsigned int key)
{
  unsigned int *res = malloc(sizeof *res);
  *res = key;
  return res;
}

static void
test_intp_list(void)
{
  unsigned int some_ints[] = {
      1, 2, 3, 4, 5,
  };
  size_t n = sizeof(some_ints) / sizeof(*some_ints);
  unsigned int *some_keys[n];
  for (int i = 0; i < n; i++) {
    some_keys[i] = dup_ui(some_ints[i]);
  }

  struct intp_list owner = NEW_LIST();

  for (unsigned int i = 0; i < n; i++) {
    printf("inserting key %u\n", *some_keys[i]);
    intp_add_key(&owner, some_keys[i]);
    some_keys[i] = NULL; // We moved it to the list
  }
  printf("\n");

  for (unsigned int i = 0; i < n; i++) {
    printf("is key %u in list? %d\n", some_ints[i],
           intp_contains_key(&owner, &some_ints[i]));
    assert(intp_contains_key(&owner, &some_ints[i]));
  }
  printf("\n");

  printf("Removing keys 3 and 4\n");
  intp_delete_key(&owner, &((unsigned int){3}));
  intp_delete_key(&owner, &((unsigned int){4}));
  printf("\n");

  for (unsigned int i = 0; i < n; i++) {
    printf("is key %u in &owner? %d\n", some_ints[i],
           intp_contains_key(&owner, &some_ints[i]));
    if (some_ints[i] == 3 || some_ints[i] == 4)
      assert(!intp_contains_key(&owner, &some_ints[i]));
    else
      assert(intp_contains_key(&owner, &some_ints[i]));
  }
  printf("\n");

  intp_free_list(&owner);
}

static char *
str_dup(const char *key)
{
  size_t n = strlen(key) + 1;
  char *res = malloc(n);
  strncpy(res, key, n);
  return res;
}

static void
test_str_list(void)
{
  const char *some_str[] = {
      "a", "b", "c", "d", "e",
  };
  size_t n = sizeof(some_str) / sizeof(*some_str);
  char *some_keys[n];
  for (int i = 0; i < n; i++) {
    some_keys[i] = str_dup(some_str[i]);
  }

  struct str_list owner = NEW_LIST();

  for (unsigned int i = 0; i < n; i++) {
    printf("inserting key %u\n", *some_keys[i]);
    str_add_key(&owner, some_keys[i]);
    some_keys[i] = NULL; // We moved it to the list
  }
  printf("\n");

  for (unsigned int i = 0; i < n; i++) {
    printf("is key %s in list? %d\n", some_str[i],
           str_contains_key(&owner, some_str[i]));
    assert(str_contains_key(&owner, some_str[i]));
  }
  printf("\n");

  printf("Removing keys 'c' and 'd'\n");
  str_delete_key(&owner, "c");
  str_delete_key(&owner, "d");
  printf("\n");

  for (unsigned int i = 0; i < n; i++) {
    printf("is key %s in &owner? %d\n", some_str[i],
           str_contains_key(&owner, some_str[i]));
    if (STR_EQ(some_str[i], "c") || STR_EQ(some_str[i], "d"))
      assert(!str_contains_key(&owner, some_str[i]));
    else
      assert(str_contains_key(&owner, some_str[i]));
  }
  printf("\n");

  str_free_list(&owner);
}

int
main()
{
  printf("generated unsigned int list\n");
  test_int_list();
  printf("generated unsigned int* list\n");
  test_intp_list();
  printf("generated char* list\n");
  test_str_list();

  return EXIT_SUCCESS;
}
