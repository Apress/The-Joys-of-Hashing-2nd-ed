
#include "linked_lists.h"

#include <stdio.h>

LIST
new_owned_list()
{
  struct link **ptr = malloc(sizeof *ptr);
  *ptr = NULL;
  return ptr;
}

static void
free_head(LIST list)
{
  struct link *next = (*list)->next;
  free(*list);
  *list = next;
}

void
free_list(LIST list)
{
  while (*list) {
    free_head(list);
  }
}

void
free_owned_list(LIST list)
{
  free_list(list);
  free(list);
}

struct link *
new_link(unsigned int key, struct link *next)
{
  struct link *link = malloc(sizeof *link);
  *link = (struct link){.key = key, .next = next};
  return link;
}

void
add_element(LIST list, unsigned int key)
{
  // Build link and put it at the front of the list.
  // The hash table checks for duplicates if we want to
  // avoid those
  *list = new_link(key, *list);
}

LIST
find_key(LIST list, unsigned int key)
{
  for (; *list; list = &(*list)->next) {
    if ((*list)->key == key)
      return list;
  }
  return NULL;
}

void
delete_element(LIST list, unsigned int key)
{
  if ((list = find_key(list, key))) {
    free_head(list);
  }
}

bool
contains_element(LIST list, unsigned int key)
{
  return find_key(list, key) != 0;
}
