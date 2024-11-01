
#ifndef GENERIC_LINKED_LISTS_H
#define GENERIC_LINKED_LISTS_H

#include <stdbool.h>
#include <stdlib.h>

// The generated data structure has a link structure with a next pointer and
// a key value. A list a struct with a `head` member that points to a link.
// An iterator is a pointer to a pointer to a link.

// clang-format off
#define NEW_LIST() { .head = NULL } // Initialiser that works for any list

#define LIST(LIST_NAME) struct LIST_NAME##_list
#define ITR(LIST) typeof(LIST->head) *

#define ITR_BEG(LIST)  (&((LIST)->head))   // Turn list into iter
#define ITR_END(ITR)   (!*(ITR))           // Check if we are at the end of the iteration
#define ITR_NEXT(ITR)  (&((*(ITR))->next)) // Get next element in the iterator
#define ITR_DEREF(ITR) (*(ITR))            // Get the current iterator element
// clang-format on

#define GEN_LIST_STRUCTS(LIST_NAME, KEY_TYPE)                                  \
  struct LIST_NAME##_link {                                                    \
    struct LIST_NAME##_link *next;                                             \
    KEY_TYPE key;                                                              \
  };                                                                           \
  struct LIST_NAME##_list {                                                    \
    struct LIST_NAME##_link *head;                                             \
  };

// Rest of the interface
#define PUSH_NEW_LINK(ITR)                                                     \
  do {                                                                         \
    typeof(**ITR) *link = malloc(sizeof *link);                                \
    link->next = *(ITR);                                                       \
    *(ITR) = link;                                                             \
  } while (0)

#define DELETE_LINK(ITR)                                                       \
  do {                                                                         \
    typeof(**ITR) *next = (*(ITR))->next;                                      \
    free(*(ITR));                                                              \
    *(ITR) = next;                                                             \
  } while (0)

#define GEN_LIST_ADD_KEY(LIST_NAME, KEY_TYPE)                                  \
  void LIST_NAME##_add_key(LIST(LIST_NAME) * list, KEY_TYPE key)               \
  {                                                                            \
    PUSH_NEW_LINK(ITR_BEG(list));                                              \
    ITR_DEREF(ITR_BEG(list))->key = key;                                       \
  }

#define GEN_LIST_FREE_LIST(LIST_NAME, KEY_TYPE, FREE_KEY)                      \
  void LIST_NAME##_free_list(LIST(LIST_NAME) * list)                           \
  {                                                                            \
    ITR(list) itr = ITR_BEG(list);                                             \
    while (!ITR_END(itr)) {                                                    \
      FREE_KEY(ITR_DEREF(itr)->key);                                           \
      DELETE_LINK(itr);                                                        \
    }                                                                          \
  }

#define GEN_LIST_DELETE_KEY(LIST_NAME, KEY_TYPE, IS_EQ, FREE_KEY)              \
  void LIST_NAME##_delete_key(LIST(LIST_NAME) * list, const KEY_TYPE key)      \
  {                                                                            \
    for (ITR(list) itr = ITR_BEG(list); !ITR_END(itr); itr = ITR_NEXT(itr)) {  \
      if (IS_EQ(ITR_DEREF(itr)->key, key)) {                                   \
        FREE_KEY(ITR_DEREF(itr)->key);                                         \
        DELETE_LINK(itr);                                                      \
        return;                                                                \
      }                                                                        \
    }                                                                          \
  }

#define GEN_LIST_CONTAINS_KEY(LIST_NAME, KEY_TYPE, IS_EQ)                      \
  bool LIST_NAME##_contains_key(LIST(LIST_NAME) * list, const KEY_TYPE key)    \
  {                                                                            \
    for (ITR(list) itr = ITR_BEG(list); !ITR_END(itr); itr = ITR_NEXT(itr)) {  \
      if (IS_EQ(ITR_DEREF(itr)->key, key)) {                                   \
        return true;                                                           \
      }                                                                        \
    }                                                                          \
    return false;                                                              \
  }

#define GEN_LIST(LIST_NAME, KEY_TYPE, IS_EQ, FREE_KEY)                         \
  GEN_LIST_STRUCTS(LIST_NAME, KEY_TYPE);                                       \
  GEN_LIST_ADD_KEY(LIST_NAME, KEY_TYPE);                                       \
  GEN_LIST_DELETE_KEY(LIST_NAME, KEY_TYPE, IS_EQ, FREE_KEY);                   \
  GEN_LIST_CONTAINS_KEY(LIST_NAME, KEY_TYPE, IS_EQ);                           \
  GEN_LIST_FREE_LIST(LIST_NAME, KEY_TYPE, FREE_KEY);

#endif
