
#ifndef LINKED_LISTS_H
#define LINKED_LISTS_H

#include <stdbool.h>
#include <stdlib.h>

struct link {
  unsigned int key;
  struct link *next;
};
typedef struct link **LIST;

#define EMPTY_LIST &((struct link *){NULL})

LIST
new_owned_list();
void
free_owned_list(LIST list);
void
free_list(LIST list);

void
add_element(LIST list, unsigned int key);
void
delete_element(LIST list, unsigned int key);
bool
contains_element(LIST list, unsigned int key);

#endif
