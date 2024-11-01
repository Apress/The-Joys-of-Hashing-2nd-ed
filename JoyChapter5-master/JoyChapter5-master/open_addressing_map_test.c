
#include "open_addressing_map.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static uint32_t
random_key()
{
  uint32_t key = (uint32_t)random();
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

void *
str_dup(const void *p)
{
  const char *s = p;
  char *new = malloc(strlen(s) + 1);
  strcpy(new, s);
  return new;
}

static void *
u32_dup(void const *p)
{
  uint32_t *new = malloc(sizeof(uint32_t));
  *new = *(uint32_t *)p;
  return new;
}

static bool
u32_cmp(void const *ap, void const *bp)
{
  uint32_t a = *(uint32_t *)ap;
  uint32_t b = *(uint32_t *)bp;
  return a == b;
}

static bool
str_cmp(void const *ap, void const *bp)
{
  char *a = (char *)ap;
  char *b = (char *)bp;
  return strcmp(a, b) == 0;
}

unsigned int
u32_hash(void const *key)
{
  // truely stupid hash but we need something for the test
  return *(uint32_t *)key ^ 0xdeadbeef;
}

#define STR_EQ(A, B) (strcmp(A, B) == 0)
unsigned int
str_hash(void const *p)
{
  char *x = (char *)p;
  unsigned int h = 0;
  for (char *p = x; *p; p++) {
    h = 31 * h + *p;
  }
  return h;
}

struct key_type ui32_key_type = {
    .cmp = u32_cmp, .del = free, .hash = u32_hash, .cpy = u32_dup};
struct value_type ui32_val_type = {.del = free, .cpy = u32_dup};

struct key_type str_key_type = {
    .cmp = str_cmp, .del = free, .hash = str_hash, .cpy = str_dup};
struct value_type str_val_type = {.del = free, .cpy = str_dup};

static void
test_intp(int no_elms)
{
  uint32_t *keys = (uint32_t *)malloc(no_elms * sizeof(uint32_t));
  for (int i = 0; i < no_elms; ++i) {
    keys[i] = random_key();
  }
  struct hash_table *map = new_table(&ui32_key_type, &ui32_val_type);
  clock_t start = clock();
  for (int i = 0; i < no_elms; ++i) {
    add_map(map, &keys[i], &keys[i]);
  }
  for (int i = 0; i < no_elms; ++i) {
    void *val = lookup_key(map, &keys[i]);
    assert(u32_cmp(val, &keys[i]));
  }
  uint32_t unused_key = 0;
  for (int i = 0; i < no_elms; ++i) {
    void *val = lookup_key(map, &unused_key);
    assert(val == 0);
  }
  for (int i = 0; i < no_elms; ++i) {
    delete_key(map, &keys[i]);
  }
  for (int i = 0; i < no_elms; ++i) {
    void *val = lookup_key(map, &keys[i]);
    assert(val == 0);
  }
  clock_t end = clock();
  double elapsed_time = (end - start) / (double)CLOCKS_PER_SEC;
  printf("%g\n", elapsed_time);

  free(keys);

  printf("active: %u\n", map->active);
  printf("used: %u\n", map->used);

  delete_table(map);
}

static char *
random_string_key()
{
  unsigned int key = (unsigned int)rand();
  return itoa(key);
}
static void
test_str(int no_elms)
{
  char **keys = malloc(no_elms * sizeof *keys);
  for (int i = 0; i < no_elms; ++i) {
    keys[i] = random_string_key();
  }

  struct hash_table *map = new_table(&str_key_type, &str_val_type);
  clock_t start = clock();
  for (int i = 0; i < no_elms; ++i) {
    add_map(map, keys[i], keys[i]);
  }
  for (int i = 0; i < no_elms; ++i) {
    void *val = lookup_key(map, keys[i]);
    assert(strcmp(val, keys[i]) == 0);
  }
  for (int i = 0; i < no_elms; ++i) {
    delete_key(map, keys[i]);
  }
  for (int i = 0; i < no_elms; ++i) {
    void *val = lookup_key(map, keys[i]);
    assert(val == 0);
  }
  clock_t end = clock();
  double elapsed_time = (end - start) / (double)CLOCKS_PER_SEC;
  printf("%g\n", elapsed_time);

  free(keys);

  printf("active: %u\n", map->active);
  printf("used: %u\n", map->used);

  delete_table(map);
}

int
main(int argc, const char *argv[])
{
  if (argc != 2) {
    printf("Usage: %s no_elements\n", argv[0]);
    return EXIT_FAILURE;
  }

  int no_elms = atoi(argv[1]);
  test_intp(no_elms);
  test_str(no_elms);

  return EXIT_SUCCESS;
}
